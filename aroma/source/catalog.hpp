// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstddef>
#include <string>
#include <vector>

namespace hbl {
struct App {
    std::string path; // Relative to the SD root, as required by RPXLoadingModule.
    std::string label;
};
struct Catalog {
    std::vector<App> apps;
    size_t legacyCount = 0;
    size_t rejectedCount = 0;
    std::string error;
};
// Scan wiiu/apps and one directory level beneath it, without modifying files.
Catalog scan(const std::string &sdRoot, const std::string &selfPath);
std::string displayText(const std::string &text, size_t limit);
}
