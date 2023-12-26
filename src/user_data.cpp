/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "User_Data.h"

int import_config_data(char* path) {
	return 0;
}

int export_config_data(char* path) {
	return 0;
}

bool set_default_sensitivity(User_Parameter_Sensitivity* sen) {
	if (sen == nullptr) { return false; }
	sen->global = 1.0;
	sen->coordinate = 1.0;
	sen->zoom = 1.0;
	sen->maxIter = 1.0;
	sen->julia = 1.0;
	sen->rotation = 1.0;
	sen->stretch = 1.0;
	sen->polar_power = 1.0;
	sen->breakout_value = 1.0;
	return true;
}

bool init_default_sensitivity(User_Parameter_Sensitivity* sen) {
	if (sen == nullptr) { return false; }
	sen->invert_zoom = false;
	return set_default_sensitivity(sen);
}