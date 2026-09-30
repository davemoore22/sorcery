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

Fresh Windows and Ubuntu interactive checks remain pending: Windows was locked
at the RustDesk login screen, and Ubuntu rejected the saved RustDesk password.
SSH builds and CTest ran successfully on both. Prior fullscreen limitations in
`ALPHA2_INTEGRATION.md` are not superseded by these automated results. This is
not a complete gameplay, combat, spell, or release-acceptance test pass.

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

The branch has not been pushed or submitted for review as part of this audit
fix pass; remaining remote interactive checks should be recorded first.
