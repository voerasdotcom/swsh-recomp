# Changelog

## 0.1.0 (unreleased)

First version, adapted from Let's Go Recomp 0.7.6 (see [docs/PORTING-NOTES.md](docs/PORTING-NOTES.md)). Not yet run
against the game.

- Pokémon Sword and Pokémon Shield detected by program id; any other game is refused
- The game's code modules are read from the dump (main, sdk, any subsdk0..9) instead of Let's Go's fixed four
- The game's version comes from the dump's `control.nacp` when there is one
- Shield saves to `save-shield/`; mods are looked up under the game's title id
- Let's Go-only options (60 / 120 fps, wild battles, controller skip) hidden; their hooks and patches removed
- `make dist` refuses to package any game file, key or game code
- Fast forward: hold Tab (option on the Game page); the X button's default key moves from Tab to E
- Frame generation removed: it paired draws by Let's Go's object hooks, and on Sword objects flashed in cutscenes
- Offline friend service (Sword crashed at start-up without it); characters' textures no longer show up seconds late
