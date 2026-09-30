# Alpha 2 integration validation — 28 September 2026

Latest source tested on all three platforms: `f5aa2a0` on
`m-series_mac_support`. Mac and Windows additionally tested the keyboard
changes through `43c221a`, as detailed below.
The initial integration commit `45ff45c1201bf4818756079eb0dc3a521d420686`
merges the previously tested Mac support with
`Alpha_2_Spellcasting` at `ffd88d4f6592769b11af02b1e4411becf590e30d`.

## Integration

The merge retains Dear ImGui `v1.92.8-docking`, Dave's viewport/coordinate
changes, the new game/magic libraries, and the Alpha 2 gameplay changes.
The two textual conflicts were resolved in the README and `src/types/meta.cpp`.
The latter now includes the relocated spell and input-mode declarations while
retaining the Apple C++23 enum adapter and GCC C++26 reflection implementation.
Enum tests were extended for spell identifiers, the negative no-spell value,
field/combat/trap cast contexts, and the engine input mode.

## Native build results

Fresh, separate build directories were used, preserving the earlier builds
and their saves. All three machines built the initial integration and then
rebuilt successfully at `f5aa2a0`; the results below apply to that source:

| Platform | Compiler | Enum implementation | Debug build | CTest |
| --- | --- | --- | --- | --- |
| Apple M2 Pro, macOS 27.2 | Apple Clang 21.0.0 | Portable C++23 adapter | Passed | 2/2 passed |
| Windows 11 x64, MSYS2 UCRT64 | GCC 16.2.0 | C++26 reflection | Passed | 2/2 passed |
| Ubuntu 26.04.1 x86_64 | GCC 16.0.1 experimental, 20260322 | C++26 reflection | Passed | 2/2 passed |

The checks are `enum_resource_compatibility` and `preserve_saves_on_rebuild`.
They do not establish correctness of every new spell or UI feature. Ubuntu's
compiler is a distribution snapshot, not a released GCC 16.1/16.2 toolchain.

Build commands follow [MACOS.md](MACOS.md) and
[CROSS_PLATFORM_VALIDATION.md](CROSS_PLATFORM_VALIDATION.md), with the Mac
build directory changed to `build/macos-alpha2`. Windows and Linux explicitly
used `SORCERY_PORTABLE_ENUMS=OFF`; all enabled `SORCERY_BUILD_TESTS`.

## Runtime observations and remaining checks

The following regression checks were observed on `f5aa2a0` using the existing
isolated test parties. Remote UI checks used RustDesk; builds and debugger
collection used SSH.

| Runtime check | Mac | Windows | Ubuntu |
| --- | --- | --- | --- |
| Launch, castle, enter dungeon, rotate | Passed | Passed | Passed |
| F9, rotate, F10 restores saved orientation | Passed | Passed | Passed |
| Inspect priest and open spell menu after quickload | Passed | Passed | Passed |
| Normal LOMILWA cast consumes one level-3 spell point | Passed (9 to 8) | Passed (9 to 8) | Passed (9 to 8) |
| LOMILWA changes torch icon and visible distance | Passed | Passed | Not separately captured |
| DIOS target selection and Return cancellation preserves points | Not repeated | Passed | Not repeated |
| Fullscreen entry, input, return to windowed mode | Passed | Unresolved | Unresolved |

Mac runtime checks used a complete isolated copy of the latest runtime at
`/tmp/sorcery-alpha2-f5aa2a0/Sorcery Runtime Test.app`. The development wrapper
on the external volume was updated to the same executable but stalled while
opening a resource. A process sample stopped in `StringStore::_load` through
`ifstream`/`fopen`/`__open_nocancel`; the cause is not established. The complete
local copy launched and passed the checks above. This does not validate the
external-volume wrapper's launch path or a redistribution bundle.

On Windows and Ubuntu, switching to fullscreen left the remote view showing
the options screen without visible response to further input. Ubuntu's desktop
updated again after the game exited. A subsequent fullscreen launch showed a
black view; GDB sampled the main thread in Mesa's buffer-swap path called by
`Display::present`, reached from `MainMenu::start`. This sample does not prove
a deadlock or distinguish driver/compositor behavior from remote capture.
Both test configurations were returned to windowed mode. Fullscreen needs a
local-display check before being marked passed; no speculative graphics fix
was made from this evidence.

The Ubuntu owner reports a failing hard drive scheduled for replacement.
Existing kernel-log entries checked during this pass showed `sr0` read errors,
not hard-drive errors; this limited check does not rule out the reported drive
fault. No disk stress test was run. Repeat Ubuntu testing after replacement.

Earlier integration checks also exercised F1 help, F2 cheat tools, and the
corrected full-window backgrounds. On Mac a metrics window was detached and
rendered on another display. Windows cheat controls enabled LOMILWA,
LATUMAPIC and MAPORFIC icons and changed displayed AC from 10 to 8. These
observations are separate from normal spellcasting verification.

Remaining coverage includes the other implemented field spells and their
effects, resurrection/failure cases, teleport destination/cancellation,
darkness extinguishing light, detached-window lifecycle, and sustained play.
Audible music quality, sustained texture unloading and the claimed GPU-memory
reduction are unverified. Some text/menu rows were clipped in the Mac UI;
layout coverage is incomplete. The two automated tests do not exercise these
runtime paths. Retest changed paths if further Alpha 2 work is incorporated.

## Runtime fixes found during integration testing

The imported Alpha 2 code exposed these faults during integration testing:

- `1cfc308`: use the viewport's screen position when drawing and clipping the
  tiled background. Previously, a window away from the screen origin could
  have a partially missing background. Verified visually on Ubuntu and Windows.
- `f359467`: restore runtime Context links for the deserialized state,
  characters and creation candidate after quickload. This reuses the existing
  `post_construct` lifecycle and does not change the save format. The crashing
  quickload/inspection path now passes manually on Ubuntu and Windows. The
  automated tests do not cover this runtime regression.
- `f5aa2a0`: rebuild each character's runtime spell catalog in `post_construct`
  and apply its serialized learned-spell flags. Quickload previously restored
  character context but left the non-serialized catalog empty. Database loads
  now use the same lifecycle. This preserves the save format and spell points.
  Quickload followed by normal LOMILWA casting passed on all three platforms.

## Observations to discuss separately

### Keyboard navigation follow-up

The user reported Enter/menu navigation failing on Mac and Windows in both
windowed and fullscreen modes. The shared UI initialization, including the
upstream Alpha 2 version, did not enable ImGui keyboard navigation. The
following commits address the common path without platform-specific input
handling:

- `1a7e36a`: enable `ImGuiConfigFlags_NavEnableKeyboard`.
- `b2a90ff`: keep the selected/highlighted menu row synchronized with keyboard
  focus and request focus for the selected row when a menu appears.
- `43c221a`: show ImGui metrics only with the existing debug-UI flag. The
  automatically opened metrics window had taken focus from the Mac game.

Mac and Windows rebuilt successfully through `43c221a`, and both passed the
two existing CTest checks. These tests do not exercise keyboard UI behavior.
Ubuntu was not rebuilt for this follow-up because of the reported failing
drive; its results above remain at `f5aa2a0`.

Manual observations on the updated builds:

- Mac fullscreen: Down/Enter continued the game, navigated Castle and Edge of
  Town, and entered the maze. Right selected No in the stairs dialog and Enter
  dismissed it without turning the party; a subsequent Right turned the party.
  Escape opened camp, and Down/Enter opened Options. No metrics window opened.
- Mac returned to windowed mode through Options. Camp arrow navigation and
  Enter activation worked, but reopening camp exposed an intermittent focus
  issue: Enter alone on the remembered highlighted row did not activate it
  until the selection was moved away and back with arrows. This remains open.
  Earlier windowed checks at `b2a90ff` also passed main-menu, Castle, Edge of
  Town and stairs-dialog navigation.
- Windows windowed: Down/Enter navigated Continue, Castle and Edge of Town.
  Right/Enter selected No in the stairs dialog; Right then turned the party.
  Camp navigation reached Options, although the initial camp focus needed an
  extra Enter before arrow navigation responded. This needs further focus
  testing alongside the Mac reopening case.
- Windows fullscreen: the remote image again stayed on Options after applying
  fullscreen, including after Escape. This remains unresolved, not a passed
  keyboard test. The test configuration was restored to windowed mode.

The latest executable SHA-256 values are:

- Mac: `36659100bc59546248b833e10203f0c0907b214449f2fc1e93541016d5e743a2`
- Windows: `61bada43c7b825cf3b25a42dfb34ecd3b70b4e5b892c5f9c97eb104fc840840e`

The Mac runtime directory retains `f5aa2a0` in its historical name; its
executable was replaced with the `43c221a` build. Logs are retained as
`macos-keyboard-final-build.log` and `windows-keyboard-final-build.log` under
`build/alpha2-evidence/`, together with the fullscreen dialog/capture images.

### Other observations

The Mac music-volume slider changed from 33% to 54% in the running UI, but
`music_volume` remained 33 in the runtime configuration after Save. The slider
updates the audio player without updating the configuration value. This UI
code is unchanged from Dave's branch; no volume-persistence fix is included.

Existing build warnings remain: GCC-only warning pragmas ignored by Clang,
unused variables in the docking changes, duplicate static libraries, a stale
dependency SDK path, and Windows runtime-dependency-copy warnings. These did
not prevent the builds. A clean-machine redistribution package is unverified.

## Artifact provenance

The transferred archive contained all 357 tracked files, with the 14 Git LFS
artwork files materialized and checked against their pointer hashes. Both
remote machines verified the archive before extraction. Subsequent fixes were
transferred as the changed source files; their SHA-256 values matched
the local checkout on both remote machines before the final build.

SHA-256 values (archive at `45ff45c`; executables at `f5aa2a0`):

- Source archive: `a59f788904c351d3ed59cf38b45494fe8e9010630d5bb05d0c0972bb5f39a121`
- Mac executable: `a5c323b7c39ee5e24416e891db9e0ee43c94c14b9da2bcf31c52137ad75b0a60`
- Windows executable: `829843deef3c24b7483276f4c3d9e1e6de92696a4c1fff90e9c8693c5557b57a`
- Ubuntu executable: `3f3fe6bb4fc40ed69b48662552bba34660206282a84118f3427e7cdccfc741bb`

Local logs, the transfer manifest, and runtime evidence are retained under
`build/alpha2-evidence/` (ignored by Git). The Mac app wrapper is a development
test artifact with links to the build's runtime resources, not a distribution
bundle. The complete local runtime copy used for Mac testing is distinguished above.
