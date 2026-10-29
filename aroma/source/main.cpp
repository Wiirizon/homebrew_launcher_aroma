// SPDX-License-Identifier: GPL-3.0-or-later
#include "catalog.hpp"
#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <padscore/kpad.h>
#include <padscore/wpad.h>
#include <proc_ui/procui.h>
#include <rpxloader/rpxloader.h>
#include <sysapp/launch.h>
#include <vpad/input.h>
#include <whb/log.h>
#include <whb/log_console.h>
#include <whb/proc.h>
#include <whb/sdcard.h>
#include <algorithm>
#include <cstdio>

namespace {
constexpr size_t PageSize = 9;
enum Action { Up = 1, Down = 2, Launch = 4, Exit = 8, Refresh = 16 };
unsigned mapButtons(uint32_t pressed, uint32_t up, uint32_t down,
                    uint32_t a, uint32_t b, uint32_t plus) {
    return ((pressed & up) ? Up : 0) | ((pressed & down) ? Down : 0) |
           ((pressed & a) ? Launch : 0) | ((pressed & b) ? Exit : 0) |
           ((pressed & plus) ? Refresh : 0);
}
unsigned readInput() {
    unsigned actions = 0;
    VPADStatus vpad {};
    VPADReadError verror;
    if (VPADRead(VPAD_CHAN_0, &vpad, 1, &verror) > 0 && verror == VPAD_READ_SUCCESS)
        actions |= mapButtons(vpad.trigger, VPAD_BUTTON_UP, VPAD_BUTTON_DOWN,
                              VPAD_BUTTON_A, VPAD_BUTTON_B, VPAD_BUTTON_PLUS);
    for (int channel = 0; channel < 4; ++channel) {
        KPADStatus pad {};
        KPADError error;
        if (KPADReadEx(static_cast<KPADChan>(channel), &pad, 1, &error) <= 0 || error != KPAD_ERROR_OK)
            continue;
        if (pad.extensionType == WPAD_EXT_PRO_CONTROLLER)
            actions |= mapButtons(pad.pro.trigger, WPAD_PRO_BUTTON_UP, WPAD_PRO_BUTTON_DOWN,
                                  WPAD_PRO_BUTTON_A, WPAD_PRO_BUTTON_B, WPAD_PRO_BUTTON_PLUS);
        else if (pad.extensionType == WPAD_EXT_CLASSIC || pad.extensionType == WPAD_EXT_MPLUS_CLASSIC)
            actions |= mapButtons(pad.classic.trigger, WPAD_CLASSIC_BUTTON_UP, WPAD_CLASSIC_BUTTON_DOWN,
                                  WPAD_CLASSIC_BUTTON_A, WPAD_CLASSIC_BUTTON_B, WPAD_CLASSIC_BUTTON_PLUS);
        // Also allow the Wii Remote's own buttons when an extension is attached.
        actions |= mapButtons(pad.trigger, WPAD_BUTTON_UP, WPAD_BUTTON_DOWN,
                              WPAD_BUTTON_A, WPAD_BUTTON_B, WPAD_BUTTON_PLUS);
    }
    return actions;
}
void draw(const hbl::Catalog &catalog, size_t selected, const std::string &message) {
    // The WHB console holds 16 lines. Replace all lines for each menu redraw.
    WHBLogPrint("Homebrew Launcher - Aroma (experimental)");
    WHBLogPrint("D-pad: select  A: launch  B: menu  +: rescan");
    WHBLogPrintf("%u apps | %u legacy ELFs hidden | %u skipped",
                 static_cast<unsigned>(catalog.apps.size()),
                 static_cast<unsigned>(catalog.legacyCount),
                 static_cast<unsigned>(catalog.rejectedCount));
    WHBLogPrint("----------------------------------------------");
    const size_t start = selected / PageSize * PageSize;
    for (size_t row = 0; row < PageSize; ++row) {
        const size_t index = start + row;
        if (index < catalog.apps.size())
            WHBLogPrintf("%c %s", index == selected ? '>' : ' ',
                         hbl::displayText(catalog.apps[index].label, 46).c_str());
        else if (row == 0) WHBLogPrint("No RPX/WUHB apps found in SD:/wiiu/apps.");
        else WHBLogPrint("");
    }
    WHBLogPrint("----------------------------------------------");
    WHBLogPrint(hbl::displayText(message, 48).c_str());
    WHBLogPrintf("Page %u/%u | ELF apps need separate ports.",
                 static_cast<unsigned>(start / PageSize + 1),
                 static_cast<unsigned>(std::max(size_t(1), (catalog.apps.size() + PageSize - 1) / PageSize)));
}
}

int main() {
    WHBProcInit();
    WHBLogConsoleInit(); // Registers callbacks to release/reacquire screen memory.
    WHBLogConsoleSetColor(0x142b42ff);
    VPADInit();
    KPADInit();
    WPADEnableURCC(TRUE);

    const auto loader = RPXLoader_InitLibrary();
    const bool loaderReady = loader == RPX_LOADER_RESULT_SUCCESS;
    bool mounted = WHBMountSdCard();
    char self[256] {};
    if (loaderReady) RPXLoader_GetPathOfRunningExecutable(self, sizeof(self));
    hbl::Catalog catalog;
    size_t selected = 0;
    bool pendingTransition = false;
    bool dirty = true;
    std::string message;
    auto rescan = [&] {
        if (!mounted) mounted = WHBMountSdCard();
        catalog = mounted ? hbl::scan(std::string("fs:") + WHBGetSdCardMountPath(), self) : hbl::Catalog{};
        selected = std::min(selected, catalog.apps.empty() ? size_t(0) : catalog.apps.size() - 1);
        if (!mounted) message = "SD unavailable. Insert card and press +.";
        else if (!catalog.error.empty()) message = catalog.error;
        else if (!loaderReady) message = "Aroma RPXLoadingModule unavailable.";
        else message = "Choose an Aroma-compatible app.";
        dirty = true;
    };
    rescan();
    while (WHBProcIsRunning()) {
        // WHBProcIsRunning processes ProcUI messages even while in background.
        // Only touch screens/controllers in the foreground.
        if (!ProcUIInForeground()) {
            OSSleepTicks(OSMillisecondsToTicks(20));
            continue;
        }
        const auto input = pendingTransition ? 0U : readInput();
        if (input & Exit) {
            SYSLaunchMenu();
            pendingTransition = true;
            message = "Returning to the Wii U Menu...";
            dirty = true;
        } else if (input & Refresh) rescan();
        else if (!catalog.apps.empty()) {
            if (input & Up) {
                selected = (selected + catalog.apps.size() - 1) % catalog.apps.size();
                dirty = true;
            } else if (input & Down) {
                selected = (selected + 1) % catalog.apps.size();
                dirty = true;
            } else if (input & Launch) {
                if (!loaderReady) {
                    message = "Loader error: " + std::to_string(static_cast<int>(loader));
                } else {
                    // The module validates the file and handles WUHB content/save
                    // redirection. Never copy legacy ELFs into Aroma's memory.
                    const auto status = RPXLoader_LaunchHomebrew(catalog.apps[selected].path.c_str());
                    pendingTransition = status == RPX_LOADER_RESULT_SUCCESS;
                    message = pendingTransition ? "Launching..." :
                        "Launch failed: " + std::to_string(static_cast<int>(status));
                }
                dirty = true;
            }
        }
        if (dirty) {
            draw(catalog, selected, message);
            dirty = false;
        }
        WHBLogConsoleDraw();
        // Do not return immediately after SYSLaunch*: drain ProcUI until EXITING.
        OSSleepTicks(OSMillisecondsToTicks(16));
    }
    if (loaderReady) RPXLoader_DeInitLibrary();
    if (mounted) WHBUnmountSdCard();
    KPADShutdown();
    WHBLogConsoleFree();
    WHBProcShutdown();
    return 0;
}
