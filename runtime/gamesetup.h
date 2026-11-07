// Game file setup on the start screen (runtime/gamesetup.c): the player picks the folder with their own extracted
// Pokémon Sword or Pokémon Shield, any version (exefs/ and romfs/, or a romfs.bin image), it is checked, set up in
// <data root>/extracted (exefs copied, the RomFS image built) and the game code is recompiled there from it
// (runtime/gamebuild.c). Runs on a background thread; the launcher polls its state.
#pragma once
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// The two games (program ids, main.npdm's ACI0)
#define TITLE_SWORD 0x0100ABF008968000ull
#define TITLE_SHIELD 0x01008DB008C2C000ull

enum {
    SETUP_IDLE,         // nothing running
    SETUP_LOOKING,      // looking for the game files in the picked folder
    SETUP_COPYING,      // copying exefs / the RomFS image
    SETUP_IMAGE,        // building the RomFS image from romfs/
    SETUP_READING,      // generator: translating the game code
    SETUP_COMPILING,    // compiler
    SETUP_FINISHING,    // linking
    SETUP_DONE,         // ready to play
    // failures (setup_state)
    SETUP_FAIL_NOT_FOUND,   // no exefs/main + romfs in that folder
    SETUP_FAIL_PACKAGE,     // .xci / .nsp found instead of extracted files
    SETUP_FAIL_VERSION,     // not Pokémon Sword / Shield (main.npdm's program id)
    SETUP_FAIL_COPY,        // disk full / unreadable files
    SETUP_FAIL_BUILD,       // the game code didn't build
    SETUP_FAIL_TOOLS,       // the generator / compiler are missing from this copy
    SETUP_FAIL_IMAGE,       // the game data image (romfs.bin, the picked one or ours) isn't a RomFS the game can read
};

// The game files in <data root>/extracted: ready to play (exefs, romfs.bin and game code built for this version).
bool setup_ready(void);
// The game files set up, but no game code built for this version (a new version unzipped over the folder, or the
// other CPU level): the launcher rebuilds it by itself (setup_start(NULL)).
bool setup_needs_build(void);
// The game in that exefs/ folder: "Sword", "Shield" or NULL.
const char *game_name(const char *exefs);
// Its program (title) id from main.npdm (0: none / unreadable).
uint64_t game_program_id(const char *exefs);
// The build of that exefs/main: one with address tables (runtime/fnhook.c, tools/patches.py) for game-specific
// runtime hooks, or GAME_BUILD_OTHER. No Sword / Shield build has address tables yet, so every build is OTHER and the
// runtime runs the game's code as it is (the hooks inherited from Let's Go Recomp stay off).
enum { GAME_BUILD_OTHER };
int game_build(const char *exefs);
// Its version for the game (am GetDisplayVersion): the DisplayVersion of a control.nacp found with the dump (copied
// next to exefs/ by the setup), else "1.0.0".
const char *game_version(const char *exefs);
// Starts a setup from the picked folder (NULL: only rebuild the game code from files set up already).
void setup_start(const char *folder);
int setup_state(void);         // SETUP_*
float setup_progress(void);    // 0..1 while running
bool setup_running(void);

#ifdef __cplusplus
}
#endif
