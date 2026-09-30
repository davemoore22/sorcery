#pragma once

namespace Sorcery {

// Call immediately after a menu Selectable. Returns whether this row should
// become the selection, respecting ImGui's current mouse/keyboard input mode.
[[nodiscard]] auto update_menu_navigation(bool selected, bool disabled) -> bool;

}
