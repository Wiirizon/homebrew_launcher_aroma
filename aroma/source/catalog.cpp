// SPDX-License-Identifier: GPL-3.0-or-later
#include "catalog.hpp"
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>

namespace hbl {
namespace {
constexpr size_t MaxApps = 2048;
constexpr size_t MaxPath = 256; // RPXLoadingModule's launch request buffer.
std::string lower(std::string text) {
    for (char &ch : text)
        if (ch >= 'A' && ch <= 'Z') ch += 'a' - 'A';
    return text;
}
bool endsWith(const std::string &text, const std::string &suffix) {
    return text.size() >= suffix.size() &&
           text.compare(text.size() - suffix.size(), suffix.size(), suffix) == 0;
}
void scanDirectory(Catalog &result, const std::string &root,
                   const std::string &relative, bool descend) {
    DIR *dir = opendir((root + "/" + relative).c_str());
    if (!dir) {
        if (descend) result.error = "Cannot open wiiu/apps: " + std::string(strerror(errno));
        else ++result.rejectedCount;
        return;
    }
    while (auto *entry = readdir(dir)) {
        const std::string name = entry->d_name;
        if (name.empty() || name[0] == '.') continue;
        const std::string path = relative + "/" + name;
        const std::string filename = lower(name);
        struct stat info {};
        if (stat((root + "/" + path).c_str(), &info) != 0) {
            ++result.rejectedCount;
            continue;
        }
        if (S_ISDIR(info.st_mode)) {
            if (descend) scanDirectory(result, root, path, false);
            continue;
        }
        if (!S_ISREG(info.st_mode)) continue;
        if (endsWith(filename, ".elf")) {
            ++result.legacyCount;
            continue;
        }
        if (!endsWith(filename, ".rpx") && !endsWith(filename, ".wuhb")) continue;
        if (path.size() >= MaxPath || info.st_size <= 0 || result.apps.size() >= MaxApps) {
            ++result.rejectedCount;
            continue;
        }
        result.apps.push_back({path, path.substr(std::string("wiiu/apps/").size())});
    }
    closedir(dir);
}
}

Catalog scan(const std::string &sdRoot, const std::string &selfPath) {
    Catalog result;
    scanDirectory(result, sdRoot, "wiiu/apps", true);
    std::sort(result.apps.begin(), result.apps.end(), [](const App &a, const App &b) {
        return lower(a.path) < lower(b.path);
    });
    // Prefer a bundle to an RPX with the same path/stem. These are normally
    // two build outputs for the same app; keep unrelated apps in the same folder.
    const auto candidates = result.apps;
    auto self = lower(selfPath);
    const auto dot = self.find_last_of('.');
    if (dot != std::string::npos) self.resize(dot);
    result.apps.erase(std::remove_if(result.apps.begin(), result.apps.end(), [&](const App &app) {
        const auto path = lower(app.path);
        const auto stem = path.substr(0, path.find_last_of('.'));
        if (!self.empty() && stem == self) return true;
        if (!endsWith(path, ".rpx")) return false;
        return std::any_of(candidates.begin(), candidates.end(), [&](const App &other) {
            return lower(other.path) == stem + ".wuhb";
        });
    }), result.apps.end());
    return result;
}

std::string displayText(const std::string &text, size_t limit) {
    std::string result = text.substr(0, limit);
    // OSScreen's font is ASCII. Do not let filenames inject console newlines.
    for (char &ch : result)
        if (static_cast<unsigned char>(ch) < 32 || static_cast<unsigned char>(ch) > 126) ch = '?';
    return result;
}
}
