# Third-party notices and source

The native application is GPL-3.0-or-later; see LICENSE and its SPDX headers.
Required third-party notices are retained verbatim, including public author
names or contact details required by those licenses. They are not user data.

- Original Homebrew Launcher: dimok789 and contributors, GPL-3.0. The fork keeps
  its historical source and notices. The native release does not include its
  GUI assets, ELF loader, or installers.
- librpxloader: wiiu-env contributors, LGPL-3.0; pinned revision
  `7b055f15f2c0e31af5b0a4ceda44ecb1ec39cc34`. Statically linked into the native
  app. See licenses/LGPL-3.0.txt and LICENSE. The release source ZIP includes
  the library source and the application source/build scripts needed to rebuild
  and relink with a modified library. Modification and reverse engineering for
  debugging such modifications are permitted under the applicable licenses.
- WUT, including WHB helpers: devkitPro and WUT authors, zlib-style license;
  see licenses/WUT.txt. Source: https://github.com/devkitPro/wut.
- newlib: multiple copyright holders and permissive licenses;
  see licenses/NEWLIB.txt. Source: https://github.com/devkitPro/newlib.
- GCC runtime libraries: Free Software Foundation and contributors,
  GPL-3.0 with GCC Runtime Library Exception 3.1 where applicable;
  see LICENSE and licenses/GCC-RUNTIME-EXCEPTION.txt.
  Source: https://gcc.gnu.org/.

No upstream branding implies endorsement. Build tools are not distributed in
the native installation ZIP. Their licenses continue to apply to those tools.

The source ZIP contains no Git history or machine-local build products. With a
public devkitPro/WUT toolchain and Node.js installed, build the bundled library
with `make -C third_party/librpxloader CPPFLAGS="-ffile-prefix-map=$(pwd)=source"`, then run
`make -C aroma RPXLOADER="$(pwd)/third_party/librpxloader"`. Install the resulting
WUHB using the same SD-copy instructions as an unmodified build; no signature
or activation key is required. Keep this notice and all license texts when
redistributing binary or source packages.
