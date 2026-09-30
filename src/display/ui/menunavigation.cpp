#include "display/ui/menunavigation.hpp"
#include "imgui.h"

auto Sorcery::update_menu_navigation(const bool selected, const bool disabled) -> bool {

	if (disabled)
		return false;

	if (selected && ImGui::IsWindowAppearing()) {
		ImGui::SetItemDefaultFocus();
		ImGui::SetKeyboardFocusHere(-1);
		// Focus alone leaves navigation inactive until an arrow key is pressed.
		ImGui::SetNavCursorVisible(true);
	}

	// IsItemHovered already follows keyboard navigation while it is active.
	// IsItemFocused alone can remain true after the user switches to the mouse.
	return ImGui::IsItemHovered();
}
