/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "fractal.h"

/* Safe Method of accessing PowerText */
const char* getPowerText(int32_t p) {
    if (p >= 0 && p <= 10) {
        return PowerText[p];
    }
    return NULL;
}
const char* getPowerText(fp64 p) {
    return getPowerText((int32_t)p);
}

fp64 getABSFractalMinRadius(fp64 power) {
	return (power - 1.0) / (pow(power, power / (power - 1.0)));
}
fp64 getABSFractalMinRadius(uint32_t power) { return getABSFractalMinRadius((fp64)power); }

fp64 getABSFractalMaxRadius(fp64 power) {
	return pow(2.0,1.0 / (power - 1.0));
}
fp64 getABSFractalMaxRadius(uint32_t power) { return getABSFractalMaxRadius((fp64)power); }

void setDefaultParameters(Fractal_Data* frac, enum FractalTypeEnum type) {
	if (frac == NULL) {
		printError("Fractal_Data* frac is NULL in setDefaultParameters()");
		return;
	}
	if (type == Fractal_ABS_Mandelbrot) {
		#define FRAC frac->type.abs_mandelbrot
		FRAC.juliaSet = false;
		FRAC.startingZ = true;
		FRAC.screenSplit = 0;
		FRAC.swapJuliaSplit = false;
		FRAC.cursorZValue = false;
		FRAC.showFloatingJulia = false;
		FRAC.adjustZoomToPower = true;
		FRAC.polarMandelbrot = false;
		FRAC.lockToCardioid = false;
		FRAC.flipCardioidSide = false;
		FRAC.r = 0.0;
		FRAC.i = 0.0;
		FRAC.zr = 0.0;
		FRAC.zi = 0.0;
		FRAC.zoom = -log10(getABSFractalMaxRadius(2.0));
		FRAC.rot = 0.0;
		FRAC.sX = 1.0;
		FRAC.sY = 1.0;
		FRAC.stretch = 0.0;
		FRAC.maxItr = 192;
		FRAC.power = 2;
		FRAC.polarPower = 2.0;
		FRAC.formula = 0;
		FRAC.breakoutValue = 16777216.0;
		FRAC.smoothColoring = true;
		FRAC.rA = 0.9;
		FRAC.rF = 0.45;
		FRAC.rP = 0.5;
		FRAC.gA = 1.0;
		FRAC.gF = 0.45;
		FRAC.gP = 0.9;
		FRAC.bA = 1.0;
		FRAC.bF = 0.45;
		FRAC.bP = 0.1;
		FRAC.iA = 1.0;
		FRAC.iF = 0.5;
		FRAC.iP = 0.0;
		#undef FRAC
	} else if (type == Fractal_Polar_Mandelbrot) {
		#define FRAC frac->type.abs_mandelbrot
		FRAC.juliaSet = false;
		FRAC.startingZ = true;
		FRAC.screenSplit = 0;
		FRAC.swapJuliaSplit = false;
		FRAC.cursorZValue = false;
		FRAC.showFloatingJulia = false;
		FRAC.adjustZoomToPower = true;
		FRAC.polarMandelbrot = true;
		FRAC.lockToCardioid = false;
		FRAC.flipCardioidSide = false;
		FRAC.r = 0.0;
		FRAC.i = 0.0;
		FRAC.zr = 0.0;
		FRAC.zi = 0.0;
		FRAC.zoom = -log10(getABSFractalMaxRadius(3.0));
		FRAC.rot = 0.0;
		FRAC.sX = 1.0;
		FRAC.sY = 1.0;
		FRAC.stretch = 0.0;
		FRAC.maxItr = 192;
		FRAC.power = 3;
		FRAC.polarPower = 3.0;
		FRAC.formula = 0;
		FRAC.breakoutValue = 16777216.0;
		FRAC.smoothColoring = true;
		FRAC.rA = 0.875;
		FRAC.rF = 0.59375;
		FRAC.rP = 0.3125;
		FRAC.gA = 0.9375;
		FRAC.gF = 0.59375;
		FRAC.gP = 0.9375;
		FRAC.bA = 1.0;
		FRAC.bF = 0.59375;
		FRAC.bP = 0.6875;
		FRAC.iA = 1.0;
		FRAC.iF = 0.5;
		FRAC.iP = 0.0;
		#undef FRAC
	} else if (type == Fractal_Sierpinski_Carpet) {
		#define FRAC frac->type.sierpinski_carpet
		FRAC.wallisSieve = false;
		FRAC.renderOutOfBounds = false;
		FRAC.fixateOnCorner = false;
		FRAC.x = 0.5;
		FRAC.y = 0.5;
		FRAC.zoom = 0.0;
		FRAC.squareSize = 1.0;
		FRAC.maxItr = 6;
		FRAC.baseColor = 0xFF0000;
		FRAC.lowColor = 0x000000;
		FRAC.lowColor = 0x00FFFF;
		#undef FRAC
	} else {
		printError("Unknown Fractal Type: %d",type);
		return;
	}
	frac->type_value = type;
	frac->type_name = FractalTypeText[type];
}