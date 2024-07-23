/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef UPDATE_PARAMETERS_H
#define UPDATE_PARAMETERS_H

#include "Common_Def.h"

struct ABS_Mandelbrot;

int_enum updateFractalParameters(
    ABS_Mandelbrot& FRAC,
    bool& force_quit_flag
);

#endif /* UPDATE_PARAMETERS_H */