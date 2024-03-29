/*
**	Author: zerico2005 (2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef DISPLAY_GUI_H
#define DISPLAY_GUI_H

#include "Common_Def.h"
#include "Program_Def.h"
#include "user_data.h"
#include "imgui.h"

// struct Display_GUI {
// 	ImGuiIO* io_IMGUI;
// 	int32_t ImGui_WINDOW_MARGIN = 8;
// 	ImGuiWindowFlags ImGui_WINDOW_FLAGS = 0;
// }; typedef struct Display_GUI Display_GUI;

int render_IMGUI();
void refresh_IMGUI(User_Configuration_Data& config);

namespace Display_GUI {
	enum IMGUI_Theme {
		IMGUI_Theme_Classic, IMGUI_Theme_Dark, IMGUI_Theme_Light, IMGUI_Theme_Count
	};
}

void set_IMGUI_Theme(Display_GUI::IMGUI_Theme theme);


void horizontal_buttons_IMGUI(ImGuiWindowFlags window_flags);

void Menu_Coordinates();
void Menu_Fractal();
void Menu_Rendering();
void Menu_Settings();
void Menu_Keybinds();

// int updateFractalParameters();

#endif /* DISPLAY_GUI_H */