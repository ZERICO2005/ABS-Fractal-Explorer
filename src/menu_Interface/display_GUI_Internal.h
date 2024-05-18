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

constexpr inline const char* Enable_Text(const bool& b) { return b ? "Enabled" : "Disabled"; }
constexpr inline const char* Available_Text(const bool& b) { return b ? "Available" : "Unavailable"; }

#endif /* DISPLAY_GUI_INTERNAL_H */