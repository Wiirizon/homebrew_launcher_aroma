# Contributing

## Scope

This repository contains two distinct areas:

- The historical legacy Homebrew Launcher and its ELF-based tooling.
- The separate experimental Simple Aroma Launcher under `aroma/`.

Native Aroma changes must not claim to preserve legacy HBL behavior. All
contributions, comments, documentation, variables, and commit messages must be
written in English.

## Native Aroma Setup

Install public devkitPro tooling for devkitPPC, WUT, wut-tools, make, Git, and
Node.js. The native build does not require Nintendo's Cafe SDK or proprietary
MakeRPL tools.

On Windows, run from a devkitPro-compatible shell:

```powershell
./aroma/build.ps1 -DevkitPro C:/devkitPro
```

The script checks out the pinned public `librpxloader` revision into the
ignored `aroma/.deps` directory and writes native outputs under `aroma/`.

## Validation

Run the host catalog tests:

```sh
mkdir -p aroma/build
cd aroma/build
g++ -std=c++17 -Wall -Wextra -Werror ../tests/catalog_tests.cpp ../source/catalog.cpp -o catalog_tests
./catalog_tests
cd ../..
node aroma/tests/sanitize-elf.test.mjs
node aroma/tests/verify-build.mjs
```

The tests do not replace Wii U hardware testing. Record the exact firmware,
Aroma version, controllers, application types, and tested exit paths when
reporting console results.

## Packaging

Package only from a clean committed checkout after building:

```powershell
./aroma/package.ps1 -Version vX.Y.Z
```

Inspect the generated binary and source archives, checksums, license notices,
and explicit file list. Never package the Cafe SDK, local settings, debug
outputs, private files, credentials, or unrelated legacy artifacts.

## Documentation and Commits

Update the relevant README, limitations, notices, and release audit whenever
behavior or packaging changes. AI-assisted commits must include the standard
`Co-authored-by` trailer using the assisting agent's official contributor name
and routing email; do not hardcode a particular agent identity in project
guidance.

Preserve original authorship, third-party notices, and license terms. Do not
rewrite published history without explicit approval.
