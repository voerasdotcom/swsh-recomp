"""Per-instruction replacements applied by gen.py (guest address -> function(gen, addr) returning C code).

Derived from LASRecomp (MIT, (c) 2026 Peiikai) by way of Let's Go Recomp. Both projects' patches were removed: their
addresses belong to those games' executables. Pokemon Sword / Shield have none yet.

To add some: write them with the addresses of one build's main (PATCHES_BUILD, the first 16 hex digits of its build
id, as gen.py prints it) and, for the other builds, where the same instructions are (BUILD_ADDRS). Any other build is
generated without them.

    def _example(gen, a):
        return '{ ...C code standing in for the instruction at a... }'
    PATCHES[0x7000123456] = _example
"""

PATCHES = {}

PATCHES_BUILD = None  # e.g. '0123456789abcdef': the build the addresses above are from
BUILD_ADDRS = {}      # other build id -> {address in PATCHES_BUILD: address in that build}
