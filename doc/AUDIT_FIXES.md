# Branch audit fixes and validation — 30 September 2026

Tested code: `cc40f6d9ddf525656a6d62cb2e2689a20615c584` on
`m-series_mac_support`. The branch includes upstream master `873276d` through
merge commit `3b096cf`; the merge preserved the existing debug-only metrics
window guard. Earlier validation is recorded in `ALPHA2_INTEGRATION.md`.

## Changes

- Menu initialization now enables the ImGui navigation cursor as well as
  focusing the remembered selection. Enter therefore activates the selection
  without first requiring an arrow key, including after reopening camp.
- Menu selection follows ImGui's active input mode. A stale keyboard-focused
  row no longer overrides a different mouse-hovered row. The small navigation
  helper is shared by production menus and headless ImGui regression tests.
- A fixed compatibility fixture checks the names and numeric IDs of all 522
  enumerators across 25 enum types, in both reflection implementations. The
  expected IDs are literals, independent of the current enum declarations.
- Successful game deserialization now restores runtime contexts and spell
  catalogs at the archive boundary. Callers no longer have to remember a
  separate restoration call. Serialized fields and their order are unchanged.
- A real-resource regression test covers binary game restoration, XML character
  loading, party/light state, runtime contexts, learned/forgotten spells,
  remaining spell points, repeat restoration, and unchanged serialized bytes
  after a round trip. All save writes use a unique temporary directory.

Windows validation also caught and resolved three test portability issues:
qualification of `Sorcery::GUID`, the SDL entry-point signature/header, and an
explicit success return from `SDL_main`.

## Native builds and automated checks

| Platform | Toolchain | Enum implementation | Build | CTest |
| --- | --- | --- | --- | --- |
| macOS 27.2, arm64 | Apple Clang 21.0.0 | Portable | Pass | 4/4 |
| Windows, x64 | MSYS2 UCRT64 GCC 16.2.0 | C++ reflection | Pass | 4/4 |
| Ubuntu 26.04.1, x64 | GCC 16.0.1 experimental | C++ reflection | Pass | 4/4 |

The checks are `enum_resource_compatibility`, `preserve_saves_on_rebuild`,
`menu_keyboard_navigation`, and `saved_game_runtime_restoration`.
The menu test was also run against deliberately restored versions of both
navigation defects; each defective version failed its regression check.

Windows and Ubuntu used fresh build directories, verified copies of the
tracked source and runtime assets, and the previously downloaded pinned
dependency sources. Final SHA-256 source manifests matched both remote copies.
Promotional artwork was omitted from the transfer; runtime artwork was included.
These are development-machine checks, not clean-machine distribution tests.

## Interactive checks and remaining limits

On Mac, the running game passed first-Enter main-menu activation, arrow/Enter
dialog cancellation, keyboard navigation through Castle and Edge of Town into
the maze, first-Enter Inspect Party activation, and the same activation after
reopening camp. Fullscreen rendered and the remembered camp Options selection
activated with Enter after reopening. The game returned to windowed mode.
This used an isolated copy of the runtime configuration and saves.

Windows passed first-Enter main-menu activation, keyboard dialog cancellation,
loading the saved party, Castle/Edge of Town navigation into the maze, and
first-Enter Inspect Party activation both initially and after reopening camp.
In fullscreen, Escape reopened camp and the first Enter activated its remembered
Options selection. Switching back to windowed mode also succeeded.

Ubuntu passed saved-party loading, arrow/Enter navigation through Castle and
Edge of Town, keyboard cancellation of the stairs dialog, and first-Enter
Inspect Party activation initially and after reopening camp. Fullscreen rendered;
reconnecting RustDesk revealed subsequent camp and Options screens. Gregory
also checked the physical Ubuntu monitor and reported that the animations were
moving, the controls he tried worked, and he walked around the dungeon.

RustDesk intermittently retained stale frames after fullscreen transitions on
both remote machines. Refreshing the Windows stream restored observation;
Ubuntu required reconnection and still had intermittent stale frames. Therefore
Ubuntu's fullscreen responsiveness is supported by the user's physical-monitor
check; an exact first-Enter fullscreen sequence was not independently verified
through RustDesk. Its initial main-menu first-Enter check was also inconclusive
after a mouse click changed navigation focus. The automated navigation tests
passed on all three platforms. No application freeze was established.

Both remote runs used isolated copies of the existing test configuration and
saves. These results supplement the historical limitations in
`ALPHA2_INTEGRATION.md`; they are focused regression and smoke checks, not a
complete gameplay, combat, spell, or release-acceptance test pass.

## Evidence

Local build/test logs and source manifests are retained under the ignored
`build/audit-fixes-evidence/` directory, with separate `windows/` and `ubuntu/`
subdirectories. Remote validation trees are under
`ApexTesting/Sorcery-audit-a6c53a7/source/` in each user's home directory. Their
historical directory name is unchanged; `SOURCE_COMMIT` records the tested code.

Executable SHA-256:

- Mac: `8610a38cd1fc85e36f33b7738c6b4d4b40d3928a6151349eaa4ef36779b0b922`
- Windows: `f4a09d13da49b497ad237ea0ff37b13642830fa20f622219b1470e258ce95526`
- Ubuntu: `101993b054136fa79bc88fa95b3ac5dbe14ee877154c7576381cb6fa47ff45da`

The audit fixes and focused three-platform validation are ready to share on
`m-series_mac_support`. No pull request has been created as part of this pass.
