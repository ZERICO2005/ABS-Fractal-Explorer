/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef DISPLAY_GUI_INTERNAL_H
#define DISPLAY_GUI_INTERNAL_H

#include "../Common_Def.h"
#include "../Program_Def.h"

#include "imgui.h"

constexpr inline const char* Enable_Text(const bool& b) { return b ? "Enabled" : "Disabled"; }
constexpr inline const char* Available_Text(const bool& b) { return b ? "Available" : "Unavailable"; }

extern ImGuiWindowFlags ImGui_WINDOW_FLAGS;

void horizontal_buttons_IMGUI(ImGuiWindowFlags window_flags);

/* Utils */
	struct User_Configuration_Data;
	struct User_GUI_Settings;

	void ImGui_DefaultWindowSize(
		const User_GUI_Settings& GUI_Settings,
		int32_t valX, int32_t bufX, int32_t minX, int32_t maxX,
		int32_t valY, int32_t bufY, int32_t minY, int32_t maxY
	);

	void ImGui_BoundWindowPosition(const User_GUI_Settings& GUI_Settings);

	void Item_Tooltip(const char* fmt, ...);

	ImVec4 get_Theme_Highlight_Color();

	void refresh_IMGUI(const User_Configuration_Data& config);

/* SubMenus */
	
	void SubMenu_GPU_Information();

#endif /* DISPLAY_GUI_INTERNAL_H */