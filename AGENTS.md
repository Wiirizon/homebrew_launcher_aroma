# AI Agent Instructions

## Project Context

This repository contains the original legacy Wii U Homebrew Launcher and a
separate experimental native Aroma application in `aroma/`. The native
application is called Simple Aroma Launcher. It is not a direct port of the
original HBL, does not preserve the original GUI or all HBL behavior, and
cannot launch legacy ELF applications.

The native application discovers RPX and WUHB files under `SD:/wiiu/apps`,
displays a bounded text menu, and delegates launches to Aroma's public
RPXLoadingModule through `librpxloader`. The historical HBL source, installer,
browser loader, assets, and ELF ABI remain separate legacy code and must not be
silently represented as part of the native Aroma feature.

The native implementation is in `aroma/source/`. Its catalog logic is in
`catalog.cpp`, its input and lifecycle handling is in `main.cpp`, and its
packaging/build verification is in `aroma/`. Public devkitPro tooling and the
pinned `librpxloader` source are used; the Cafe SDK and proprietary MakeRPL
tools are not required or distributed.

The Codex project history confirms the intended scope: provide a useful,
simplified Aroma launcher, clearly distinguish it from the original HBL, keep
legacy ELF support out of the native release, and avoid publishing SDK files,
personal files, credentials, local paths, or unrelated build output.

## Required Rules

### AI Co-Author Rule

Any time an AI agent modifies, creates, or refactors code, it MUST append itself
as a co-author in the git commit message using the standard GitHub co-author
format. If the AI agent does not know its official GitHub contributor routing
email, it must check the repository's commit history to find previous examples
of its own attribution, or search the web to find its correct official
co-author tag. Do not hardcode a specific agent name, email address, or tag in
this rule.

### Documentation Sync Rule

Any time an AI agent changes, adds, or removes code, it MUST simultaneously
update the relevant documentation (README, comments, or wikis) to reflect
those changes.

### Language Rule

All contributions to this repository, including code comments, variables,
commit messages, and documentation, MUST be written in English.

## Coding Standards

- Keep native Aroma changes inside `aroma/` unless a shared root change is
  strictly required.
- Preserve the separation between the legacy ELF launcher and the native
  RPX/WUHB launcher.
- Do not add ELF compatibility claims to the native application.
- Keep discovery bounded, deterministic, case-insensitive, and safe for display.
- Use the existing `librpxloader` API for launches; do not copy applications
  into reserved memory or implement a second loader.
- Preserve ProcUI foreground/background handling and normal Aroma exit behavior.
- Keep executable sanitization limited to known non-executable diagnostic data;
  never alter code sections.
- Preserve GPL, LGPL, zlib, newlib, GCC runtime, and original HBL attribution.

## Workflow Guidelines

1. Work from a checkout with the required public submodules/dependencies.
2. Do not add `Cafe_SDK`, proprietary tools, personal SD contents, credentials,
   local settings, or generated binaries to Git.
3. For native development, use devkitPPC, WUT, wut-tools, Node.js, and make.
4. Build the public dependency at its pinned revision before building Aroma.
5. Run catalog tests, sanitization tests, and RPX/WUHB verification after builds.
6. Use the explicit packaging script and inspect its file list before release.
7. Review privacy scans, license notices, and release checksums before publishing.
8. Record hardware results as scoped evidence, not as universal compatibility.
9. Keep legacy-root changes separate from native Aroma changes.
10. Push only validated commits and never rewrite published history without
    explicit approval.

## Release and Safety Boundaries

- The native release is experimental and must be described as a separate
  simplified launcher, not a direct HBL port.
- Legacy ELF applications require source-level ports and are intentionally not
  launched by the native application.
- Launch only trusted homebrew. A valid RPX/WUHB file can still contain unsafe
  behavior.
- Do not distribute Nintendo firmware, keys, proprietary SDK material, dumped
  executables, or third-party themes/assets.
- Keep `THIRD_PARTY_NOTICES.md`, `UNOFFICIAL.md`, and all applicable license
  texts with binary and source distributions.
