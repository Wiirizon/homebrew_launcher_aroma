# Release audit — October 29, 2026

Scope: the native Simple Aroma Launcher, its build scripts, installation/source
packages, repository configuration, release notes, and reachable Git history.
This is an engineering review, not a guarantee of legal compliance or safety.

## Checks and corrections

- No credential-signature matches were found in the 589 historical file blobs
  reachable at the start of the audit. The signature scan covered private-key
  headers and common GitHub/AWS token formats; it is not an exhaustive secret
  detector. No personal files, SDK files, or generated build files were added
  to the audit commit.
- RPX/WUHB verification inspects decompressed sections, rejects home-directory
  paths and credential signatures, and verifies the embedded RPX matches.
  Toolchain assertion paths are scrubbed in the linked ELF before compression.
  The scrub preserves section sizes/offsets and executable sections; a dedicated
  test verifies those properties. No path allowlist bypass remains.
- Native metadata and the visible title consistently say Simple Aroma Launcher.
  The project remains explicitly separate from HBL, without legacy ELF support.
- UNOFFICIAL.md documents lack of endorsement and relevant experimental-use
  risks. No proprietary assets or original HBL installers enter native packages.
- Required dependency license notices are distributed with the binary. The
  source ZIP includes the native application and pinned LGPL loader source,
  build scripts, and installation/relinking instructions.
- Packaging uses explicit file lists and excludes Git history, local settings,
  debug/maps, SDKs, and unrelated legacy assets. Public source URLs and required
  third-party license/author notices are retained for provenance and compliance.
- The installation and source ZIPs were inspected for forbidden build/private
  files. The bundled source independently rebuilt and relinked successfully,
  and its resulting RPX/WUHB passed verification. Clean CI cross-build, catalog
  sanitizer tests, and diagnostic-scrub tests also passed.
- The native application has no added network listener, account access,
  telemetry, NAND installer, or system-title modification. Selected third-party
  applications are outside this review; only launch homebrew you trust.

## Unresolved limits

Existing public Git history includes original upstream author metadata and the
configured Git identity used by earlier project commits. No public history has
been rewritten by this audit. Hosting-provider account/PR metadata and cached
commits cannot be erased by editing files or publishing a corrected release.
Required upstream attribution must remain intact. New audit commits use a
generic project author with no email or account mapping.

The initial successful console report does not establish coverage of every
firmware, controller, HOME/resume scenario, or target application. The hardware
checklist in aroma/README.md remains applicable.
