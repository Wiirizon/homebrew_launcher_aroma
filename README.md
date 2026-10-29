# Simple Aroma Launcher (experimental)

**This is NOT a direct port of the original Homebrew Launcher.** It is a
separate, simplified text-based launcher for Aroma-compatible RPX/WUHB apps.
It does **not** support legacy ELF applications or preserve all original HBL
features. The original animated interface, icons, audio, touch controls,
metadata/arguments, and network loader are absent.

This fork contains the separate native launcher in [`aroma/`](aroma/README.md).
It browses Aroma-compatible apps using a text menu and launches them through
Aroma's RPXLoadingModule. It does **not** make legacy ELF applications compatible.
The user reported successful operation on a Wii U on October 29, 2026. This is
still experimental; the full hardware test checklist has not been verified.
Download the installable build from this fork's [Releases](https://github.com/Wiirizon/homebrew_launcher_aroma/releases).
See the Aroma README for installation, controls, limitations, and build
instructions using public homebrew tools only. Nintendo's Cafe SDK is not used
or distributed.

This is an unofficial community application, with no Nintendo or original-HBL
endorsement. See [UNOFFICIAL.md](UNOFFICIAL.md) for scope and safe-use notes and
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for licenses and source/relinking
instructions. Release ZIPs contain the applicable notices; the source ZIP
includes the native application and its pinned LGPL loader library source.

## Original Homebrew Launcher documentation (legacy only)

The following documentation and inherited source belong to the original
Homebrew Launcher by dimok789 and contributors. They describe the legacy
launcher, **not the simplified Aroma application released by this fork**.

The Homebrew Launcher is a WiiU homebrew that lists homebrew applications located on a SD card and permits launching them (similar to the Homebrew Channel of the Wii).

#### Usage

To use the Homebrew Launcher (or HBL, for short) you must copy homebrew_launcher.elf into SD:/wiiu/apps/homebrew_launcher/homebrew_launcher.elf, and run the installer throught your WiiU browser.

The apps that will be listed are should be in the following path /wiiu/apps/homebrew_name/some_elf_name.elf on the root of the SD card. A meta.xml and an icon.png (256x96) are optional. Here is an example:

- sd:/
  - wiiu/
    - apps/
     - homebrew_launcher/
        - homebrew_launcher.elf
        - meta.xml
        - icon.png
     - loadiine_gx2/
       - loadiine_gx2.elf
       - meta.xml
       - icon.png
     - ddd/
       - ddd.elf
       - meta.xml
       - icon.png
     - ftpiiu/
       - ftpiiu.elf
       - meta.xml
       - icon.png

#### Building the Homebrew Launcher
To build the main application devkitPPC is required as well as some additionally libraries. If not yet done export the path of devkitPPC and devkitPro to the evironment variables DEVKITPRO and DEVKITPPC.

Some can be found in the "libs" folder of this repository. Extract the "portlibs.zip" into your devkitPro directory. This package includes:  
- zlib

In order to build this application you need serval libs:

- [dynamic_libs](https://github.com/Maschell/dynamic_libs/tree/lib) for access to the functions.
- [libutils](https://github.com/Maschell/libutils) for common functions.
- [libgui](https://github.com/Maschell/libgui) for the gui elements.

Install them (in this order) according to their README's. Don't forget the dependencies of the libs itself.

All remaining is to enter the main application path and enter "make". You should get a homebrew_launcher.elf and a homebrew__launcher_dbg.elf in the main path.

To compile the installer application enter the "installer" path on the source code and type "make".

### Building using the Docker

It's possible to use a docker image for building. This way you don't need anything installed on your host system.

```
# make 
docker run -it --rm -v ${PWD}:/project wiiulegacy/hbl_builder:0.1 make

# make clean
docker run -it --rm -v ${PWD}:/project wiiulegacy/hbl_builder:0.1 make clean
```

#### Building an application / homebrew (ELF) for the Homebrew Launcher 
For an example on how to build an application for the HBL check out the [Hello World example](https://github.com/dimok789/hello_world) application or the port of the libwiiu application [Pong](https://github.com/dimok789/pong_port) for HBL.

#### Meta XML

The meta.xml is optional and can be put in the same path as the homebrew ELF file to display additional information about the homebrew.

Here is a XML example:

    <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
    <app version="1">
      <name>name</name>
      <coder>coder</coder>
      <version>app version</version>
      <release_date>app release date</release_date>
      <short_description>short description</short_description> 
      <long_description>long description</long_description> 
    </app>

#### Icon PNG
The icon.png has to be of the resolution 256 x 96 and can be placed in the same path as the homebrew ELF file. This file is optional and shows an icon for the homebrew inside the homebrew launcher.

