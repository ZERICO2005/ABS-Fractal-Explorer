/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRACTAL_H
#define FRACTAL_H
#include "Common_Def.h"
enum FractalTypeEnum {
	Fractal_ABS_Mandelbrot,Fractal_Polar_Mandelbrot,Fractal_Sierpinski_Carpet
};
const char* const FractalTypeText[] = {"ABS Mandelbrot","Polar Mandelbrot","Sierpinski Carpet"};
const char* const PowerText[] = {"Constant","Linear","Quadratic","Cubic","Quartic","Quintic","Sextic","Septic","Octic","Nonic","Decic"};
 /* Safe Method of accessing PowerText */
const char* getPowerText(int32_t p);
const char* getPowerText(fp64 p);

fp64 getABSFractalMinRadius(fp64 power);
fp64 getABSFractalMinRadius(uint32_t power);

fp64 getABSFractalMaxRadius(fp64 power);
fp64 getABSFractalMaxRadius(uint32_t power);

struct _ABS_Mandelbrot {
	/* Parameters */
	bool juliaSet;
	bool startingZ;
	uint32_t screenSplit;
	bool swapJuliaSplit;
	bool cursorZValue;
	bool relativeZValue;
	bool showFloatingJulia;
	bool adjustZoomToPower;
	bool polarMandelbrot;
	bool lockToCardioid;
	bool flipCardioidSide;
	/* Coordinates */
	fp64 r;
	fp64 i;
	fp64 zr;
	fp64 zi;
	fp64 zoom;
	/* Transformation */
	fp64 rot;
	fp64 sX;
	fp64 sY;
	fp64 stretch;
	/* Parameters */
	uint32_t maxItr;
	uint32_t power;
	fp64 polarPower;
	uint64_t formula;
	fp64 breakoutValue;
	/* Coloring */
	bool smoothColoring;
	fp64 rA;
	fp64 rF;
	fp64 rP;
	fp64 gA;
	fp64 gF;
	fp64 gP;
	fp64 bA;
	fp64 bF;
	fp64 bP;
	fp64 iA;
	fp64 iF;
	fp64 iP;
}; typedef struct _ABS_Mandelbrot ABS_Mandelbrot;

struct _Sierpinski_Carpet {
	/* Parameters */
	bool wallisSieve;
	bool renderOutOfBounds;
	bool fixateOnCorner;
	/* Coordinates */
	fp64 x;
	fp64 y;
	fp64 zoom;
	fp64 squareSize;
	uint32_t maxItr;
	/* Color */
	uint32_t baseColor;
	uint32_t lowColor;
	uint32_t highColor;
}; typedef struct _Sierpinski_Carpet Sierpinski_Carpet;


union _Fractal_Type {
	ABS_Mandelbrot abs_mandelbrot;
	ABS_Mandelbrot polar_mandelbrot;
	Sierpinski_Carpet sierpinski_carpet;
}; typedef union _Fractal_Type Fractal_Type;

/* Holds the fractal and rendering parameters */
struct _Fractal_Data {
	const char* type_name;
	uint32_t type_value;
	Fractal_Type type;
}; typedef struct _Fractal_Data Fractal_Data;

void setDefaultParameters(Fractal_Data* frac, enum FractalTypeEnum type);

#endif /* FRACTAL_H */