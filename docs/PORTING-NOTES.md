# Porting notes: Let's Go Recomp → Pokémon Sword / Shield

This tree started as a copy of Let's Go Recomp 0.7.6. These notes record what was changed for Sword / Shield, what
was deliberately left alone, and what still has to be done before the game can be expected to run.

**Status: not yet run against the game.** Nothing below has been checked against a real Sword / Shield dump, and the
code has not been compiled since the changes (no toolchain on the machine the port was prepared on). The first job is to
build it (`docs/BUILDING-WINDOWS.md`), set up a dump, and read `lasrecomp.log`.

## Changed

| Area | Files | What |
|---|---|---|
| Game detection | `runtime/gamesetup.c/.h` | Accepts program ids `0100ABF008968000` (Sword) and `01008DB008C2C000` (Shield). `game_name()` returns "Sword" / "Shield". Every build is `GAME_BUILD_OTHER` (no address tables). |
| Version | `runtime/gamesetup.c` | `game_version()` reads `DisplayVersion` (+0x3060) from a `control.nacp` found with the dump and copied to `extracted/`; "1.0.0" without one. Reported by `am` GetDisplayVersion. |
| RomFS discovery | `runtime/gamesetup.c` | A RomFS folder is one with `bin/message`, or a folder named `romfs` with a `bin/` inside. The search doesn't walk into it. |
| RomFS check | `runtime/romfs_build.c` | Minimum file count lowered from Let's Go's 27 000 to 1 000 (the real count is unknown here; measure and raise it). |
| Module layout | `runtime/modules.h` (new), `loader.c`, `gamebuild.c`, `rt.h`, `gamelib.h`, `tools/gamelib_entry.c`, `Makefile` | Let's Go had exactly `main, sdk, subsdk0, subsdk1` at fixed bases. Now `main` (512 MB slot), `sdk` (64 MB), and any `subsdk0..9` present (64 MB each), all read from the dump. The loader stops with a clear message if a module outgrows its slot. The generated library lists its modules through `gamelib_modules.h`. `GAMELIB_ABI` is 4, so libraries built with the old layout are rebuilt. |
| Function hooks | `runtime/fnhook.c` | Let's Go's address tables removed; `rt_addr()` is 0 for every build. |
| Generator patches | `tools/patches.py`, `tools/gen.py` | Let's Go's instruction patches removed; the mechanism (`PATCHES_BUILD`, `BUILD_ADDRS`) is kept for Sword / Shield ones. |
| Saves | `runtime/services/fs.c` | Shield saves to `save-shield/`, Sword to `save/`. |
| Mods | `runtime/mods.c` | Packs with both games' folders are searched under the set-up game's title id first. |
| Defaults | `runtime/settings.c` | Shadow filter defaults to the game's own (the smooth filter was tuned to Let's Go's 11-tap shadows). |
| Branding | `paths.h`, `windows.rc`, launcher, `Makefile` | "SwSh Recomp", `SwShRecomp.exe`, version 0.1.0, own data folder on Linux / macOS. |
| Release safety | `Makefile` `dist`, `.gitignore` | `make dist` fails if any game file, key, NACP or game library is in the release folder; `.gitignore` covers dumps, NACPs, `lasgame.*`, `save-shield/`. |

Let's Go Recomp's research documents (HISTORICO, DEPURACAO, MODO-BATALHA, PESQUISA-60FPS...) weren't copied: they
describe Let's Go. Code comments still cite them; read them in the
[Let's Go Recomp repository](https://github.com/raulzrrs/letsgo-recomp/tree/main/docs). Its debugging variables
(`LARECOMP_*`, e.g. `LARECOMP_HOOK_DUMP`, `LARECOMP_TRACE_STORES`) all still work here.

## Kept as they are (inert for Sword / Shield)

These Let's Go features hook that game's functions by address. With no address table they never register, and their
options (wild battles, controller skip, the frame rate) are hidden in the launcher and the F12 menu. They reappear
for a build that gets an address table (`known` in `runtime/ui/launcher_lg.mm`, `ui_page_graphics`):

- `runtime/hle/fps60.c`, `fps60_research.c`: 60 / 120 fps overworld
- `runtime/hle/wildbattle.c`, `catch_button.c` (+ `catch_button_art.h`, drawn from scratch, no game art): wild battles
- `runtime/hle/controller_skip.c`: skip the controller screen
- `runtime/hle/cutscene.c`: cutscene camera

They are kept as working examples of the hook mechanism; remove them once Sword / Shield equivalents exist or if they
get in the way. Research tools in `tools/` (addrmap, capture scripts...) are kept too.

## Still to do (expected work, roughly in the order it will come up)

1. **Build and boot.** Compile, set up a dump, run with the console visible. Expect missing service commands
   (`[ipc]` / `[fs] ... cmd N` lines in the log) and missing HLE functions: implement them in `runtime/services/` and
   `runtime/hle/`.
2. **Module slots.** Confirm Sword / Shield's exefs modules and sizes fit `runtime/modules.h` (the loader logs each
   module's base and size).
3. **Memory.** Sword / Shield may want more heap than Let's Go (`HEAP_MAX`, 3.25 GB, in `runtime/rt.h`) or other
   `svcGetInfo` values; check the log for heap / mapping failures.
4. **Graphics.** The NVN → Vulkan layer was exercised only by Let's Go. Expect untranslated shader instructions
   (`runtime/nvn/maxwell/translate.c`), texture formats (`textures.c`) and NVN entry points (`objects.c`, `cmdbuf.c`).
   The renderer's extras (SSAO, GI, TAA motion vectors) assume Let's Go's scene layout; keep them off until checked.
5. **Movies and audio.** Let's Go played movies through `subsdk1`'s multimedia library (`runtime/hle/movie.c`) and
   mixed audio through `audout`. Sword / Shield may use other paths.
6. **Input.** `runtime/hle/input.c` presents the PC controller the way Let's Go wanted it (no Pro Controller style).
   Sword / Shield accept a Pro Controller; consider presenting one.
7. **RomFS minimum.** Measure the games' RomFS file counts and raise `ROMFS_MIN_FILES`.
8. **Game-specific options.** Research Sword / Shield's code for frame rate, etc., give the builds `GAME_BUILD_*` values
   in `runtime/gamesetup.h`, and their hooks address tables in `runtime/fnhook.c`.
9. **The 2x interface** (`runtime/ui2x.c`) reads Let's Go's `bin/app/*.arc` UI archives; Sword / Shield pack their UI
   differently, so it will find nothing until adapted. Its launcher section is hidden (`k_ui2x_supported` in
   `runtime/ui/launcher_lg.mm`); set it to true once `ui2x.c` reads Sword / Shield's UI files.
