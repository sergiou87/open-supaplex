# Platform Support

This document is generated from `.github/platform-builds.json`.

## Status

| Platform | Backend | Build System | Port Folder | CI | Release |
| --- | --- | --- | --- | --- | --- |
| macOS | SDL2 | Xcode project | `macos` | Yes | Yes |
| Windows x86_64 | SDL2 | Makefile | `windows` | Yes | Yes |
| Linux | SDL2 | Makefile | `linux` | Yes | Yes |
| PSP | SDL1 | Makefile | `psp` | Yes | Yes |
| PS Vita | SDL2 | CMake | `vita` | Yes | Yes |
| PlayStation 2 | SDL2 | Makefile | `ps2` | No | No |
| PS3 | SDL2 | Makefile | `ps3` | Yes | Yes |
| Nintendo DS | SDL1 | Makefile | `nds` | Yes | Yes |
| Nintendo 3DS | SDL1 | Makefile | `3ds` | Yes | Yes |
| Wii | SDL1 | Makefile | `wii` | Yes | Yes |
| Wii U | SDL2 | Makefile | `wiiu` | Yes | Yes |
| Nintendo Switch | SDL2 | Makefile | `switch` | Yes | Yes |
| RISC OS | SDL1 | Makefile | `riscos` | Yes | Yes |

## Notes

- CI and release build metadata live in `.github/platform-builds.json`.
- Release asset naming is defined per platform in `.github/platform-builds.json`.
- There is also a `wasm` folder in the repository, but it is not currently listed as a supported platform in the main documentation or wired into CI/release automation.
- PlayStation 2 currently requires a local PS2DEV/PS2SDK toolchain plus ps2-packer. The repository now includes basic helper scripts in ps2/, but it still does not provide a CI toolchain image or automated runner for that platform.
