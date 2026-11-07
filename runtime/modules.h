// The game's code modules and where each is loaded (guest addresses). One rule for everything that needs it: the
// loader (runtime/loader.c), the start screen's build of the game code (runtime/gamebuild.c) and `make gen` (the
// Makefile repeats the same arithmetic). The generated code holds absolute addresses, so a library built with another
// layout can't run: change it only together with GAMELIB_ABI (runtime/gamelib.h).
//
// The modules are the NSOs of exefs/ in load order: main, sdk, then subsdk0..subsdk9 - whichever the game has
// (Let's Go had subsdk0 and subsdk1; Pokémon Sword / Shield's set is read from the dump, not assumed). Each has a slot
// of its own:
//   main          0x70_0000_0000, 512 MB
//   sdk           0x70_2000_0000, 64 MB
//   subsdk<i>     0x70_2400_0000 + i * 64 MB    (subsdk9 ends at 0x70_4C00_0000, below HLE_BASE)
#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

#define MAIN_BASE      0x7000000000ull
#define MAIN_SLOT      0x0020000000ull
#define SDK_BASE       0x7020000000ull
#define MODULE_SLOT    0x0004000000ull
#define SUBSDK_BASE(i) (0x7024000000ull + (uint64_t)(i) * MODULE_SLOT)
#define RT_MAX_SUBSDK  10
#define RT_MAX_MODULES (2 + RT_MAX_SUBSDK)

typedef struct {
    char name[16];  // the NSO's file name in exefs/
    uint64_t base;  // where it is loaded
    uint64_t slot;  // how much room it has there
} RtModuleSlot;

// The modules in exefs/ (main and sdk always listed; the subsdk ones that exist), in load order. Returns how many.
static inline int rt_module_layout(const char *exefs, RtModuleSlot out[RT_MAX_MODULES]) {
    int n = 0;
    snprintf(out[n].name, sizeof out[n].name, "main"), out[n].base = MAIN_BASE, out[n].slot = MAIN_SLOT, n++;
    snprintf(out[n].name, sizeof out[n].name, "sdk"), out[n].base = SDK_BASE, out[n].slot = MODULE_SLOT, n++;
    for (int i = 0; i < RT_MAX_SUBSDK; i++) {
        char p[1100];
        struct stat st;
        snprintf(p, sizeof p, "%s/subsdk%d", exefs, i);
        if (stat(p, &st) != 0 || !S_ISREG(st.st_mode)) continue;
        snprintf(out[n].name, sizeof out[n].name, "subsdk%d", i);
        out[n].base = SUBSDK_BASE(i), out[n].slot = MODULE_SLOT, n++;
    }
    return n;
}
