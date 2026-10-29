# Simple Aroma Launcher — experimental

**This is NOT a direct port of Homebrew Launcher.** It is a separate, simplified
text-based application for browsing and launching Aroma-compatible apps.
It does not preserve the original interface or all original functionality,
and **cannot launch legacy ELF apps**. The original HBL source inherited from
dimok789 and contributors remains in this repository, but the native app in
this directory is a separate implementation. It builds as
RPX/WUHB with devkitPPC and WUT, and uses Aroma's public
[librpxloader](https://github.com/wiiu-env/librpxloader) API. The historical ELF
source and root Makefile remain available separately.

**Status: experimental; user-reported successful operation on a Wii U on
October 29, 2026.** The exact firmware, Aroma version, controllers, and tested
scenarios were not supplied. The full console checklist below remains unverified.

## Installation and controls

1. Download the experimental ZIP or WUHB from this fork's
   [Releases](https://github.com/Wiirizon/homebrew_launcher_aroma/releases),
   build the project, or use the `aroma-launcher` GitHub Actions artifact.
   The release ZIP contains `wiiu/apps/homebrew_launcher_aroma.wuhb`; extract
   it to the SD root. Keep its README, license, and source link for reference.
2. Copy **only** `homebrew_launcher_aroma.wuhb` to
   `SD:/wiiu/apps/homebrew_launcher_aroma.wuhb`.
3. Launch it from the Wii U Menu while running Aroma. Use an up-to-date, complete
   Aroma installation, including RPXLoadingModule and its dependencies. This app
   does not install, replace, or patch those modules.
4. Put compatible `.rpx` or `.wuhb` apps in `SD:/wiiu/apps` or one subdirectory
   beneath it. The menu displays relative filenames, sorted alphabetically.

Use D-pad up/down to select, **A** to launch, **B** to return to the Wii U Menu,
and **+** to rescan. GamePad, Wii U Pro Controller, Wii Remote, and Classic
Controller buttons are supported in code; verify them on hardware. HOME uses
the normal application lifecycle. Launching another app drains ProcUI messages
until the OS signals exit; the launcher does not jump into another app's code.

A bundle is preferred over an RPX with the same folder and basename. The running
launcher and its matching RPX/WUHB counterpart are hidden when the loader reports
its path. Empty files, paths that exceed the loader's 255-byte limit, dotfiles,
and directories deeper than one level are skipped. Discovery is capped at 2048
candidates to bound memory use. The loader validates selected executables;
listing a file does not establish that the app itself is Aroma compatible.

## Scope and limitations

- Legacy `.elf` applications are counted but cannot be launched. Converting the
  file container does not relocate hardcoded addresses or remove HBL ABI
  assumptions. Each legacy app needs a source port. Aroma's developer explains
  the conflicting memory layouts and ProcUI requirements
  [here](https://maschell.github.io/homebrew/2023/04/30/incompatible-homebrews.html).
- This first native interface is a paged text list on the TV and GamePad.
  The original animated GUI, icons, audio, touch interaction, metadata display,
  `meta.xml` arguments, and TCP/wiiload receiver are not ported.
- Apps launched from this menu follow Aroma's usual exit behavior; they do not
  automatically return to this launcher.
- Missing SD cards, missing loader modules, and synchronous launch failures
  produce messages. A target app that fails after the OS accepts the launch
  cannot report its failure back to this process.
- No installation to NAND, system title modification, kernel hooks, custom
  executable copying, or network listener is implemented in the new launcher.
  It asks Aroma's existing loader to launch the user-selected SD file.

## Build

Requires public devkitPro devkitPPC, WUT, wut-tools (`elf2rpl`, `wuhbtool`), make,
and Git. **Nintendo's Cafe SDK and MakeRPL are not used.** The ELF emitted during
this build is a WUT linker intermediate; it is not the historical HBL ELF.

On Windows, with devkitPro installed:

```powershell
./aroma/build.ps1 -DevkitPro C:/devkitPro
```

The script downloads a pinned public dependency into ignored `aroma/.deps`,
checks the revision, builds it, and produces the WUHB and RPX in `aroma/`.
It never stages or commits files. Paths containing spaces require an MSYS
compatible installation path for this Makefile/toolchain.

On Linux or in a devkitPPC build environment:

```sh
git clone https://github.com/wiiu-env/librpxloader.git /tmp/hbl-librpxloader
git -C /tmp/hbl-librpxloader checkout --detach 7b055f15f2c0e31af5b0a4ceda44ecb1ec39cc34
make -C /tmp/hbl-librpxloader -j2
make -C aroma RPXLOADER=/tmp/hbl-librpxloader -j2
```

The CI image is the dated public `ghcr.io/wiiu-env/devkitppc:20260225` image used
by the dependency. Do not put the proprietary SDK, personal SD contents,
credentials, or unrelated local files in this repository. Keep GPL-3.0 source
and existing notices with distributions. These source and packaging choices do
not guarantee freedom from third-party complaints; this is an unofficial project.

## Validation

The host catalog tests cover directory depth, case-insensitive extensions and
sorting, empty files, duplicate build outputs, self filtering, missing SD paths,
legacy ELF exclusion, filename display sanitization, and (on Linux) overlong
loader paths. Run from a dedicated build directory:

```sh
mkdir -p aroma/build
cd aroma/build
g++ -std=c++17 -Wall -Wextra -Werror ../tests/catalog_tests.cpp ../source/catalog.cpp -o catalog_tests
./catalog_tests
```

The tests create and remove only their own `catalog-fixture` subdirectory and
refuse to run if it already exists. These tests do not exercise Cafe OS APIs.
After building, run `node aroma/tests/verify-build.mjs` from the repository root
to check the RPX header and confirm that WUHB embeds exactly the built RPX.

The following detailed checks remain outstanding despite the initial successful
user report. Check on an actual Wii U before claiming broad compatibility:

- Cold launch from the Aroma Wii U Menu; verify TV and GamePad output.
- Navigate multiple pages with each supported controller, including disconnects.
- Open HOME, resume, and repeat; exit through HOME, B, and the console power button.
- Launch a known Aroma-compatible RPX and a WUHB with bundled content; verify the
  target app's content and save paths, then exit it normally.
- Check empty/missing apps folders, missing SD, rescan, malformed selected files,
  missing loader module, and an SD removal between scan and launch.
- Confirm legacy ELF entries never launch, and other apps and saves are intact.
