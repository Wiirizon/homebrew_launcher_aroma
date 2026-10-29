// SPDX-License-Identifier: GPL-3.0-or-later
#include "../source/catalog.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
void file(const fs::path &path) {
    fs::create_directories(path.parent_path());
    std::ofstream(path).put('x');
}
int main() {
    // Fixtures belong to this test's build directory, never the user's SD tree.
    const auto root = fs::current_path() / "catalog-fixture";
    assert(!fs::exists(root));
    file(root / "wiiu/apps/Zebra.RPX");
    file(root / "wiiu/apps/alpha/boot.rpx");
    file(root / "wiiu/apps/alpha/boot.WUHB");
    file(root / "wiiu/apps/alpha/other.rpx");
    file(root / "wiiu/apps/legacy/boot.elf");
    file(root / "wiiu/apps/launcher/self.wuhb");
    file(root / "wiiu/apps/launcher/self.rpx");
    file(root / "wiiu/apps/deep/nested/ignored.rpx");
    file(root / "wiiu/apps/.hidden.rpx");
    file(root / "wiiu/apps/fake.rpx/notes.txt");
    fs::create_directories(root / "wiiu/apps/empty");
    std::ofstream(root / "wiiu/apps/empty/zero.rpx");
    // The loader uses a 256-byte path buffer. Do not silently truncate requests.
#ifndef _WIN32
    // Windows' legacy MAX_PATH limit prevents creating this fixture.
    file(root / "wiiu/apps" / std::string(240, 'a') / "too-long.rpx");
#endif

    const auto catalog = hbl::scan(root.string(), "wiiu/apps/launcher/self.wuhb");
    assert(catalog.error.empty());
    assert(catalog.apps.size() == 3);
    assert(catalog.legacyCount == 1);
#ifdef _WIN32
    assert(catalog.rejectedCount == 1);
#else
    assert(catalog.rejectedCount == 2);
#endif
    assert(catalog.apps[0].path == "wiiu/apps/alpha/boot.WUHB");
    assert(catalog.apps[1].path == "wiiu/apps/alpha/other.rpx");
    assert(catalog.apps[2].path == "wiiu/apps/Zebra.RPX");
    assert(hbl::displayText("a\nb\t%c", 5) == "a?b?%");
    assert(hbl::scan((root / "missing").string(), "").apps.empty());
    assert(!hbl::scan((root / "missing").string(), "").error.empty());
    assert(fs::exists(root / "wiiu/apps/legacy/boot.elf"));
    fs::remove_all(root);
    std::cout << "Catalog tests passed\n";
}
