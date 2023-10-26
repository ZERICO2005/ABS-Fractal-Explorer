/*
**	Author: zerico2005 (2023)
**	Project: Easy-GUI
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRACTAL_H
#define FRACTAL_H
#include "Common_Def.h"

const char* const FractalTypeText[] = {"ABS Mandelbrot","Polar Mandelbrot","Sierpinski Carpet",};
const char* const PowerText[] = {"Constant","Linear","Quadratic","Cubic","Quartic","Quintic","Sextic","Septic","Octic","Nonic","Decic"};
 /* Safe Method of accessing PowerText */
const char* getPowerText(int32_t p);

struct _FractalType {
	char* name;
}; typedef struct _FractalType FractalType;

fp64 getABSFractalMinRadius(fp64 power);
fp64 getABSFractalMinRadius(uint32_t power);

fp64 getABSFractalMaxRadius(fp64 power);
fp64 getABSFractalMaxRadius(uint32_t power);

#endif /* FRACTAL_H */