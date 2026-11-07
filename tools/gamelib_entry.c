// Entry of the game code library (runtime/gamelib.h), compiled into it with the generated code: the function
// tables, found by name when the app loads the library. Everything else in it stays hidden.
//
// The modules depend on the game's exefs/ (runtime/modules.h): gamelib_modules.h, written next to the generated code
// by whoever generated it (runtime/gamebuild.c, Makefile `gen`), lists them in load order:
//   #define GAMELIB_MODULES(X) X(main) X(sdk) X(subsdk0) ...
#include "gamelib.h"
#include "gamelib_modules.h"

#define MOD(n) extern const uint64_t n##_fn_addrs[], n##_fn_count; extern const GuestFn n##_fn_ptrs[];
GAMELIB_MODULES(MOD)
#undef MOD
#define TAB(n) {#n, n##_fn_addrs, n##_fn_ptrs, &n##_fn_count},
#define ONE(n) +1

GAMELIB_EXPORT const GameLibTables *lasgame_tables(void) {
    static const GameLibTables t = {GAMELIB_ABI, GAMELIB_ID, 0 GAMELIB_MODULES(ONE), {GAMELIB_MODULES(TAB)}};
    return &t;
}
