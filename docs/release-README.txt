SwSh Recomp
===========

An experimental native Windows port of Pokemon Sword and Pokemon Shield (Nintendo Switch) by static recompilation.
Based on Let's Go Recomp (https://github.com/raulzrrs/letsgo-recomp) and LAS Recomp by Peiikai.

NO GAME FILES ARE INCLUDED. You need your own legally obtained copy of Pokemon Sword or Pokemon Shield, extracted
(the exefs and romfs folders). The game code is translated and compiled on your PC from your own copy.

EARLY, UNTESTED PORT: it may not boot yet. See the project page for its status.

Supported: Pokemon Sword (title id 0100ABF008968000) and Pokemon Shield (01008DB008C2C000), any version and region.
To play an updated version, extract the base game with the update applied (merged exefs + romfs). Keep the
control.nacp your extraction tool writes next to exefs, if it writes one: the game's version is read from it.
Packed files (.xci, .nsp, .nsz, .nca) must be extracted first.

Requirements
------------
- Windows 10 or 11, 64-bit
- A CPU with SSE4.2; AVX2 recommended (without it the game code is built for older CPUs, and runs slower)
- A GPU whose driver supports Vulkan 1.3
- RAM: 16 GB recommended
- About 30 GB of free disk space during the one-time setup

How to play
-----------
1. Unzip this folder anywhere you can write to (e.g. D:\Games\SwShRecomp). Not inside Program Files.
2. Open SwShRecomp.exe. It is not code-signed: if Windows says "Windows protected your PC", click "More info"
   and then "Run anyway".
3. Click "Select game folder..." and pick the folder with your extracted game (the one that holds exefs and romfs,
   or any folder above it).
4. Wait while the game files are copied and the game code is recompiled for your PC. This happens only once and
   can take an hour or more.
5. Click Play.

Both games: one folder holds one set-up game, so unzip this twice (e.g. SwShRecomp-Sword and SwShRecomp-Shield).

Everything stays in this folder: settings (larecomp.ini), saves (save\, or save-shield\ for Shield), the game files
(extracted\), mods (mods\) and the log (lasrecomp.log).

Options inherited from Let's Go Recomp that hook that game's code (60/120 fps, wild battles, controller-screen skip)
are hidden for Sword / Shield.

Mods
----
On the start screen's Mods page, click "Install a mod..." and pick the downloaded file (.zip, .rar or .7z). Packs made
for Atmosphere / LayeredFS (romfs replacements) work; mods of the game's code (exefs, IPS patches, cheats) don't.

Controls
--------
Keyboard: WASD walk, Space/Enter = A, Q/Backspace = B, Tab = X, R = Y, Z/C = L/R, = and - = Plus/Minus,
arrows = D-pad, Esc = close, F12 = settings. All keys can be changed on the start screen.
Controllers: any controller SDL recognizes (Xbox, PlayStation, Switch Pro...).

Troubleshooting
---------------
- "That's not Pokemon Sword or Pokemon Shield": pick the folder of your own extracted game.
- "The game code builder is missing": your antivirus may have quarantined lasgen\lasgen.exe (a PyInstaller
  program, sometimes flagged by mistake). Restore it or add this folder as an exception.
- The game closes or doesn't start: see lasrecomp.log in this folder and report it on the project page.

Legal
-----
Fan project, not affiliated with Nintendo, The Pokemon Company, Game Freak or Creatures. MIT licensed (LICENSE.txt),
based on Let's Go Recomp and LAS Recomp by Peiikai (LICENSE-LASRecomp.txt, https://github.com/peiikai/LASRecomp).
Bundled: SDL3 (zlib), FFmpeg (LGPL 2.1; source: https://ffmpeg.org), shaderc (Apache 2.0), libiconv (LGPL),
Zig (MIT), the Baloo 2 font (SIL OFL 1.1, Baloo2-OFL.txt), AMD's FidelityFX SDK runtime for FSR 3.1 (MIT,
amd_fidelityfx_vk.dll, FidelityFX-LICENSE.txt), and the GCC / MinGW-w64 runtime libraries (GPL with the runtime
library exception).
