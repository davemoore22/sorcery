# Sorcery

An open source remake/remaster of the classic CRPG **Wizardry, Proving Grounds of the Mad Overlord**. Currently under heavy development with an initial release around the end of 2026. Developed using ImGui/SDL2/OpenGL/C++. Will be available on both Linux and Windows.

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

## Progress Update (2nd September 2026)

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
* ~~Automap~~ (DONE)
* ~~Character Editing/Deleting/Legating~~ (DONE)
* ~~Inventory and Item Manipulation~~ (DONE)
* ~~Victory/End Game Handling~~ (DONE)
* ~~Traps and Chests~~ (DONE)
* ~~Quicksave/Quickload~~ (DONE)
* **Special Item Powers and Handling (DOING)**
* **Boltac's Equipment Shop (DOING)**
* Context-Sensitive-Help (TO DO)
* Creatures and NPCs (TO DO)
* Encounter Tables (TO DO)
* Magic and Combat (TO DO)
* Strict (Legacy) Mode (TO DO)
* Windows version (TO DO)

## Wishlist

* Achievements
* Radial Menu Control mode
* Enhanced Quality-of-Life features
* Sequels Content (II, III, V)
* Procedurally-Generated Infinite Content
* Monster Pits (practise against Encounters without risk of death or reward)
* Migration to SDL3 from SDL2
* Optional Textured/Rendered 3D View
* Wizardry VI implemented in the classic Wireframe Engine.

## What next? (as of September 2026)

Currently adding Item Usage/Invocation, as well as buying, selling, uncursing, and identifying at Boltac's. Normal inventory management has been mosty completed! Initially I am planning to remake Wizardry 1, then adding the content from Wizardrys 2, 3, and 5; and then subsequently providing infinite procedurally-generated content like that of many roguelikes.

In the meantime, check out Snarfu's bugfix and recompilation of the original Wizardry for Apple II at <https://github.com/snafaru> or Sshoecraft's recreation in the terminal of the original trilogy, again for Apple II at <https://github.com/sshoecraft/wizardry>>.

## Compilation

I'm currently developing solely on Linux (Ubuntu 26) so compilation instructions are currently Linux-specific. Windows instructions and releases will of course be supported at a later date. Full compliation instructions for Linux are in the doc/COMPILE.md file, but in brief, CMake is used to test for needed system libraries, fetch needed third-party headers, and to produce an appropriate makefile. Please note that C++26 is required, so GCC 16.1 is the minimum recommended compiler.

All code is released under the GPLv2 - see the doc/LICENSE.md file for full details.

## Contact and Help

All comments and criticism and help welcome. You can mainly find me as *Typhon* on the C++ Help and DungeonCrawlers.org Discord Servers.

## Credits

Graphics used (all free/open source) are by:

* Torio - <https://www.pixiv.net/en/users/5887541>
* Various - <https://game-icons.net/>

Music used (Creative Commonds) is by:

* Kevin MacLeod - <https://incompetech.com/music/royalty-free/music.html>
