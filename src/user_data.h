/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#ifndef USER_DATA_H
#define USER_DATA_H

#include "Common_Def.h"

const char* const Sensitivity_Labels[] {
	"Global Sensitivity","Coordinate","Zoom","Iteration","Julia Coordinate","Rotation","Stretch","Polar Power","Breakout Value"
};

struct User_Parameter_Sensitivity {
	fp64 global; // Applies to moveDelta
	fp64 coordinate;
	fp64 zoom;
	bool invert_zoom;
	fp64 maxIter;
	fp64 julia;
	fp64 rotation;
	fp64 stretch;
	fp64 polar_power;
	fp64 breakout_value;
}; typedef User_Parameter_Sensitivity User_Parameter_Sensitivity;

bool set_default_sensitivity(User_Parameter_Sensitivity* sen);
bool init_default_sensitivity(User_Parameter_Sensitivity* sen);

#endif /* USER_DATA_H */