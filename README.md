# Sorcery

Sorcery is an open-source remake/remaster of the classic CRPG **Wizardry: Proving Grounds of the Mad Overlord**.

The project is under active development and is built using **C++26, Dear ImGui, SDL2 and OpenGL**. Sorcery targets **Debian/Ubuntu Linux** and **64-bit Windows**.

**Alpha #1 is available now from the Releases section. Alpha #2 is coming soon.**

## Screenshots

<table>
  <tr>
    <td><img src="promo/screen1.png" width="150"></td>
    <td><img src="promo/screen2.png" width="150"></td>
    <td><img src="promo/screen3.png" width="150"></td>
    <td><img src="promo/screen4.png" width="150"></td>
    <td><img src="promo/screen5.png" width="150"></td>
  </tr>
  <tr>
    <td><img src="promo/screen6.png" width="150"></td>
    <td><img src="promo/screen7.png" width="150"></td>
    <td><img src="promo/screen8.png" width="150"></td>
    <td><img src="promo/screen9.png" width="150"></td>
    <td><img src="promo/screen10.png" width="150"></td>
  </tr>
  <tr>
    <td><img src="promo/screen11.png" width="150"></td>
    <td><img src="promo/screen12.png" width="150"></td>
    <td><img src="promo/screen13.png" width="150"></td>
    <td><img src="promo/screen14.png" width="150"></td>
    <td><img src="promo/screen15.png" width="150"></td>
  </tr>
  <tr>
    <td><img src="promo/screen16.png" width="150"></td>
    <td><img src="promo/screen17.png" width="150"></td>
    <td><img src="promo/screen18.png" width="150"></td>
    <td><img src="promo/screen19.png" width="150"></td>
    <td><img src="promo/screen20.png" width="150"></td>
  </tr>
  <tr>
    <td><img src="promo/screen21.png" width="150"></td>
    <td><img src="promo/screen22.png" width="150"></td>
    <td><img src="promo/screen23.png" width="150"></td>
    <td><img src="promo/screen24.png" width="150"></td>
    <td><img src="promo/screen25.png" width="150"></td>
  </tr>
</table>

Brief gameplay videos are also available on [YouTube](https://www.youtube.com/channel/UCEkmGGFyxu5vH6xAHej3Dag).

## Current Status

### Alpha #1 — Released 19 September 2026

Alpha #1 is the first public release of Sorcery and contains the majority of the core game framework and non-combat systems.

Implemented functionality includes:

- **Core game systems**
  - New Game, Save and Load
  - Quicksave and Quickload
  - Keyboard, mouse and gamepad controls
  - Linux/Windows support
- **Characters and party management**
  - Character creation
  - Character editing, deleting and legating
  - Party and expedition management
  - Levelling up and changing class
  - Healing and resurrection
- **Town and services**
  - Town and Castle menus
  - Gilgamesh's Tavern
  - Adventurer's Inn
  - Temple
  - Boltac's Equipment Shop
- **Dungeon exploration**
  - Maze generation and navigation
  - Dungeon features and events
  - Automapping
  - Traps and chests
  - Party wipe handling
  - Victory and end-game handling
- **Items and presentation**
  - Inventory and item manipulation
  - Special item powers and handling
  - Compendium
  - Music

### Alpha #2 — Coming Soon

Current Alpha #2 changes (as of 25th September) include:

- Reduced required graphics memory to approximately **400 MB**.
- Corrected fullscreen rendering.
- Added a **music volume control**.
- Added **context-sensitive help** using **F1**.
- Expanded the **Cheat Menu** available through **F2**.
- Implemented out-of-combat spellcasting for:
  - **Divine spells**
    - DIOS, DIAL, DIALMA, LATUMOFIS, MADI
    - MILWA, LOMILWA, LATUMAPIC
    - MAPORFIC, KANDI
    - DI, KADORTO
  - **Arcane spells**
    - DUMAPIC, MALOR
- The buff bar now displays the status of **MILWA/LOMILWA, LATUMAPIC and MAPORFIC**.
- Darkness squares now extinguish any active light spell.

The broader Alpha #2 development cycle is focused on implementing **Magic and Combat**, together with **Creatures, Combat and NPCs**. I will release this as soon as feasible

### Alpha #3 — Planned

Current Alpha #3 targets include:

- Encounter tables
- Fixed combats
- Treasure tables
- Sound effects
- Strict (Legacy) Mode

## Development Roadmap

With most of the original Wizardry I game framework now implemented, the main remaining areas of work are **spellcasting, combat, creatures and NPCs**. Obviously the roadmap will continue to evolve as the Alpha releases progress.

## Wishlist

Longer-term ideas include:

- Achievements and quests
- Radial-menu control mode
- Additional quality-of-life features
- Content from later Wizardry games — II, III, V, and possibly IV
- Procedurally generated infinite content
- Monster Pits for practising against encounters without risk or reward
- Migration from SDL2 to SDL3
- Optional textured/rendered OpenGL 3D dungeon view
- Wizardry VI implemented using the classic wireframe engine

## Related Projects

If you are interested in the original Wizardry games and related preservation/reimplementation work, see:

- [Snarfu's Wizardry work](https://github.com/snafaru), including bug fixes and recompilation work for the original Apple II Wizardry.
- [Sshoecraft's Wizardry recreation](https://github.com/sshoecraft/wizardry), a terminal-based recreation of the original Apple II trilogy.

## System Requirements

Sorcery requires:

- **OpenGL 3.3**
- **2 GB system memory**
- Approximately **400 MB graphics memory** for Alpha #2
- A 64-bit Windows or Debian/Ubuntu Linux environment

## Compilation

Sorcery uses a **single source tree** for both Linux and Windows.

The primary development platform is Ubuntu/Debian Linux. Windows builds are supported using **MSYS2 UCRT64**, GCC, CMake and Ninja. Other Windows toolchains are not currently tested or officially supported, although contributions documenting additional working configurations are welcome.

The project targets **C++26** and uses C++ reflection, so **GCC 16.1 or later is required**. The current development toolchain uses **GCC 16.2**.

Platform-specific differences are handled by CMake and the source code; no separate Windows or Linux source trees are required.

See [`doc/COMPILE.md`](doc/COMPILE.md) for full build instructions.

## License

Sorcery source code is released under the **GNU GPLv2**. See [`doc/LICENSE.md`](doc/LICENSE.md) for full details.

Graphics and music are used under their respective licenses, as listed below. Note that all artwork/music is authored by humans.

## Contact and Help

Comments, criticism and contributions are welcome.

You can usually find me as **Typhon** on the **C++ Help** and **DungeonCrawlers.org** Discord servers, or contact me by email at:

`davemoore22 AT protonmail.com`

## Credits

### Graphics

Creative Commons graphics are used from:

- [Toriotto](https://www.pixiv.net/en/users/5887541)
- [game-icons.net](https://game-icons.net/)
- [Xelu](https://thoseawesomeguys.com/prompts/)

### Music

Music is used under the Soundimage International Public License and is by:

- [Eric Matyas](https://www.soundimage.org)
