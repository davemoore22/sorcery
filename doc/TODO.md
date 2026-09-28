
# TECHNICAL TODOs (20/11/2026)

Map / tile handling optimisation

- revisit how levels/maps are stored and loaded
- clean up the Grid Cartographer JSON peculiarities
- then optimise tile/event/lookups and rendering-side usage

UI

- Resize all modal frames dynamically and properly (e.g. cast spell)

Dungeon View

- Reduce the number of drawing calls in rendering optimisation
- Make Secret Doors flash occasionally when not using LOMILWA

Automap

- Add a "Make all floor visible" button to the Cheat Menu
- Add click and move as well as mouse highlight to the Automap

Help

- Finish Context Sensitive Help for all Modules
- Split Engine Help across two columns

Debug

- Morrowind style console window
- --debug and --profile command line options (not using CMake Defines?)
