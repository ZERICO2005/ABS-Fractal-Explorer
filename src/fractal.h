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

const char* const FractalTypeText[] = {"ABS Mandelbrot","Polar Mandelbrot","Sierpinski Carpet",};
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
	bool lockToCardioid;
	bool flipCardioidSide;
	fp64 input_breakoutValue;
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
	/* Coloring */
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
	Sierpinski_Carpet sierpinski_carpet;
}; typedef union _Fractal_Type Fractal_Type;

/* Holds resolution, sampling, precision and rendering method */
struct _Render_Data {
	/* Dimensions */
	uint32_t resX;
	uint32_t resY;
	uint32_t offsetX;
	uint32_t offsetY;
	uint32_t subResX;
	uint32_t subResY;
	/* Format */
	uint32_t padding;
	uint8_t bpp;
	uint8_t channels;
	uint8_t flip;
	bool subPixelRendering;
	/* Rendering Parameters */
	uint32_t sample;
	uint32_t subSample;
	fp64 area;
	/* Rendering Method */
	uint32_t rendering_method;
	uint32_t CPU_Precision;
	uint32_t CPU_Threads;
	uint32_t GPU_Precision;
}; typedef struct _Render_Data Render_Data;

/* Holds the fractal and rendering parameters */
struct _Fractal_Data {
	char* type_name;
	uint32_t type_value;
	Render_Data render_data;
	Fractal_Type type;
}; typedef struct _Fractal_Data Fractal_Data;

#endif /* FRACTAL_H */