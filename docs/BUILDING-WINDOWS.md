# Building and running on Windows

From source to the game window. **To just play, use a release zip** (see the [README](../README.md#how-to-play)): it
needs none of this.

## What you need
- Windows 10 or 11, 64-bit
- A CPU with SSE4.2; with AVX2 the game code runs faster. `make` builds the game code for `GENARCH` (default
  `-march=x86-64-v3`; `GENARCH=-march=x86-64-v2` for a CPU without AVX2, after deleting `build/obj/`); the launcher
  picks by itself from the CPU
- A GPU with Vulkan 1.3 and a current driver
- About 40 GB free (game files, generated code, build)
- [Git for Windows](https://git-scm.com/download/win) (for Git Bash)
- **Your own game files**: *Pokémon Sword* or *Pokémon Shield*, extracted from your own copy. This project doesn't
  provide them, the keys, or instructions for getting them.

## 1. Install the tools (once)
In PowerShell, in the project folder:

```powershell
powershell -ExecutionPolicy Bypass -File tools\setup_windows.ps1
```

It installs **MSYS2** in `C:\msys64` (clang, make, SDL3, FFmpeg, Vulkan, shaderc) and **Python 3.12** with the code
generator's environment in `.venv`, and sets the clone to LF line endings. It can be run again if something fails.

## 2. Build the runtime
Open **Git Bash** in the project folder and put MSYS2 on the PATH (for that window):

```bash
export PATH="/c/msys64/ucrt64/bin:/c/msys64/usr/bin:$PATH"
make -j20 build/swsh-recomp.exe
```

This builds `build/swsh-recomp.exe`: the launcher and everything that stands in for the Switch. To start it with a
double click (or from PowerShell without MSYS2 on the PATH), copy the DLLs it needs next to it once:

```bash
make dlls
```

Without that, Windows stops it with "SDL3.dll / avcodec-63.dll ... was not found". From the source tree, the
launcher's game setup uses the `.venv` generator and MSYS2's clang (`C:\msys64\ucrt64\bin\clang.exe`) when there's no
bundled Zig.

## 3. Set up the game from the launcher (simplest)

```bash
./build/swsh-recomp.exe
```

Click **Select game folder...** and pick the folder with the extracted game (the one holding `exefs` and `romfs`, or a
`romfs.bin`). The launcher checks it's Sword or Shield (program id in `main.npdm`), copies the files into `extracted/`,
generates the game code with `.venv` and compiles it with MSYS2's clang (`extracted/lasgame.dll`). Then click **Play**.

## 3b. Or by hand
Create `extracted/` in the project root:

```
extracted/
  exefs/         main, main.npdm, rtld, sdk, subsdk* (the game's ExeFS: whichever subsdk modules it has)
  romfs.bin      the game's RomFS image
  control.nacp   optional: the game's version is read from it
```

Translate the game code into C (into `generated/`, outside git) and compile. **Always use `-j`**:

```bash
make gen
make -j20
```

`make gen` translates `main`, `sdk` and each `subsdk0`..`subsdk9` present, at the bases in `runtime/modules.h`, and
writes `generated/gamelib_modules.h` (the module list for `tools/gamelib_entry.c`). `make -j20` then builds
`build/lasgame.dll`.

`extracted/` and `generated/` are in `.gitignore` and must never be committed.

## 4. Play

```bash
./build/swsh-recomp.exe
```

- Saves go in `save/` (`save-shield/` for Shield), settings in `larecomp.ini`, shader caches in `cache/`.
- `--play` skips the launcher: `./build/swsh-recomp.exe extracted --play`.
- `make jogo` puts a copy in `build/jogo/` that opens with a double click and lets you rebuild `build/` while playing.

## 5. The release zip

```bash
make deps-windows          # LGPL-only FFmpeg with the decoders needed (needs MSYS2's nasm)
make gen-helper            # the frozen generator (PyInstaller: no Python on the player's PC)
tools/fetch_zig.sh         # the Zig compiler, pinned and checked by sha256
make dist                  # build/dist/SwShRecomp-<version>-windows.zip
```

The zip holds `swsh-recomp.exe`, its DLLs, the generator, a slimmed Zig and the headers the game code compiles
against. **Nothing from the game goes in it**: `make dist` fails if the release folder contains any game file, key or
game code (exefs, romfs, NSO/NCA/NSP/XCI, NACP, `lasgame.*`). The version is `RT_APP_VERSION` in `runtime/paths.h`.

## Common problems
| Symptom | What to do |
|---|---|
| "avutil-...dll / SDL3.dll not found" | run from Git Bash with MSYS2 on the PATH, or use the `make jogo` copy |
| the launcher asks to set the game up again | the generator, headers or module layout changed: pick the folder again (or `make gen` and `make -j20`) |
| `make gen` fails | check `.venv` (run `setup_windows.ps1` again) and that `extracted/exefs/main` exists |
| "... is 0x... bytes, more than its slot" at start | a module outgrew its slot in `runtime/modules.h`: enlarge it there and in the Makefile's `gen` rule, bump `GAMELIB_ABI` |
| black screen or Vulkan error | update the GPU driver; Vulkan 1.3 is required |
| the game closes | attach the terminal output (or the release's `lasrecomp.log`) to an issue |

## Working on the code
- [PORTING-NOTES.md](PORTING-NOTES.md): what was changed from Let's Go Recomp and what's left for Sword / Shield.
- Changed `runtime/cpu.h`, `runtime/gamelib.h` or `runtime/modules.h`? The game library must be rebuilt (`make -j20`).
