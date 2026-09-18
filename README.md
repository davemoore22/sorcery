# Sorcery

An open source remake/remaster of the classic CRPG **Wizardry, Proving Grounds of the Mad Overlord**. Currently under heavy development with an initial release around the end of 2026. Developed using ImGui/SDL2/OpenGL/C++. Will be available on both Linux and Windows. OpenGL 3.3 and 2 Gb of System Memory, and 1 Gb of Video Memory will be required to run.

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

See also some brief [gameplay videos](https://www.youtube.com/channel/UCEkmGGFyxu5vH6xAHej3Dag) on Youtube.

## Progress Update 16th September 2026)

* ~~Load/Save/New Game~~ (DONE)
* ~~Town/Castle Menus~~ (DONE)
* ~~Character Creation~~ (DONE)
* ~~Party/Expedition Management~~ (DONE)
* ~~Maze Generation and Navigation~~ (DONE)
* ~~Dungeon Features~~ (DONE)
* ~~Automapping~~ (DONE)
* ~~Keyboard/Mouse/Gamepad Control~~ (DONE)
* ~~Gilgamesh's Tavern~~ (DONE)
* ~~Adventurer's Inn~~ (DONE)
* ~~Levelling up/Changing Class~~ (DONE)
* ~~Temple/Healing/Resurrection~~ (DONE)
* ~~Wipe Handling~~ (DONE)
* ~~Dungeon Events~~ (DONE)
* ~~Compendium~~ (DONE)
* ~~Music~~ (DONE)
* ~~Character Editing/Deleting/Legating~~ (DONE)
* ~~Inventory and Item Manipulation~~ (DONE)
* ~~Victory/End Game Handling~~ (DONE)
* ~~Traps and Chests~~ (DONE)
* ~~Quicksave/Quickload~~ (DONE)
* ~~Boltac's Equipment Shop~~ (DONE)
* ~~Windows Port~~ (DONE)
* **Special Item Powers and Handling** (DOING)
* **Magic and Combat** (DOING)
* **Creatures, Combat, and NPCs** (DOING)
* Encounter Tables (TO DO)
* Fixed Combats (TO DO)
* Treasure Tables (TO DO)
* SFX (TO DO)
* Context-Sensitive-Help (TO DO)
* Strict (Legacy) Mode (TO DO)

## Wishlist

* Achievements and Quests
* Radial Menu Control mode
* Enhanced Quality-of-Life features
* Sequels Content (II, III, V, maybe even IV?)
* Procedurally-Generated Infinite Content
* Monster Pits (practise against Encounters without risk of death or reward)
* Migration to SDL3 from SDL2
* Optional OpenGL Textured/Rendered 3D View
* Wizardry VI implemented in the classic Wireframe Engine.

## What next? (as of September 2026)

**Spellcrafting and Combat are the only major things left to implement!** Initially I am only planning to remake Wizardry 1, then adding the content from the second and third games as soon as I can. In the meantime, check out Snarfu's bugfix and recompilation of the original Wizardry for Apple II at <https://github.com/snafaru> or Sshoecraft's recreation in the terminal of the original trilogy, again for Apple II at <https://github.com/sshoecraft/wizardry>>.

## Compilation

Sorcery uses a **single source tree** which builds on both Linux and Windows. The primary development platform is Ubuntu/Debian Linux. Windows builds are supported using **MSYS2 UCRT64**, GCC, CMake and Ninja. Other Windows toolchains are not currently tested or supported by the project, although contributions documenting them are welcome. The project targets **C++26** and uses C++ reflection, so **GCC 16.1 or later is required**. The current development toolchain uses GCC 16.2. No separate Linux or Windows source tree is required. Platform-specific differences are handled by CMake and the source code. Please read the doc/COMPILE.md for full compilation instructions on both platforms, and doc/AI.md for a statement on LLM/AI usage. All code is released under the GPLv2 - see the doc/LICENSE.md file for full details.

## Contact and Help

All comments and criticism and help welcome. You can mainly find me as *Typhon* on the C++ Help and DungeonCrawlers.org Discord Servers or email me at davemoore22 AT protonmail.com

## Credits

Graphics used (Creative Commons) are by:

* Toriotto - <https://www.pixiv.net/en/users/5887541>
* Various - <https://game-icons.net/>
* Xelu - <https://thoseawesomeguys.com/prompts/>

Music used (Soundimage International Public License) is by:

* Eric Matyas - <https://www.soundimage.org>
