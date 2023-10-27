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


fp64 getABSFractalMinRadius(fp64 power) {
	return (power - 1.0) / (pow(power, power / (power - 1.0)));
}
fp64 getABSFractalMinRadius(uint32_t power) { return getABSFractalMinRadius((fp64)power); }

fp64 getABSFractalMaxRadius(fp64 power) {
	return pow(2.0,1.0 / (power - 1.0));
}
fp64 getABSFractalMaxRadius(uint32_t power) { return getABSFractalMaxRadius((fp64)power); }