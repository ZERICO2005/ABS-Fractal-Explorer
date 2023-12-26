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
#include "Program_Def.h"

#include "render.h"

enum FractalTypeEnum {
	Fractal_ABS_Mandelbrot,Fractal_Polar_Mandelbrot,Fractal_Sierpinski_Carpet
};

#ifndef BUILD_RELEASE
	const char* const FractalTypeText[] = {"ABS Mandelbrot","Polar Mandelbrot","Sierpinski Carpet"};
	const char* const FractalTypeFileText[] = {"ABS-Mandelbrot","Polar-Mandelbrot","Sierpinski-Carpet"};
#else
	const char* const FractalTypeText[] = {"ABS Mandelbrot","Polar Mandelbrot"};
	const char* const FractalTypeFileText[] = {"ABS-Mandelbrot","Polar-Mandelbrot"};
#endif

const char* const PowerText[] = {
	"Constant","Linear","Quadratic","Cubic","Quartic","Quintic","Sextic","Septic","Octic","Nonic",
	"Decic","Undecic","Dodecic","Tridecic","Tetradecic","Pentadecic","Hexadecic","Heptadecic","Octadecic","Nonadecic",
    "Icosic", "Unicosic", "Duocosic", "Triacosic", "Tetraicosic","Pentacosic", "Hexacosic", "Heptacosic", "Octacosic", "Nonacosic"
};
 /* Safe Method of accessing PowerText */
const char* getPowerText(int32_t p);
const char* getPowerText(fp64 p);

fp64 getABSFractalMinRadius(fp64 power);
fp64 getABSFractalMinRadius(uint32_t power);

fp64 getABSFractalMaxRadius(fp64 power);
fp64 getABSFractalMaxRadius(uint32_t power);

uint64_t limitFormulaID(uint32_t power, uint64_t formula);
uint64_t getABSValue(uint32_t power);
fp64 getStretchValue(fp64 s);
fp64 getZoomDefault(fp64 p);

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
	bool integerPolarPower;
	bool lockToCardioid;
	bool flipCardioidSide;
	/* Coordinates */
	fp128 r;
	fp128 i;
	fp128 zr;
	fp128 zi;
	fp64 zoom;
	/* Transformation */
	fp64 rot;
	fp64 sX;
	fp64 sY;
	fp64 stretch;
	/* Parameters */
	uint32_t maxItr;
	fp64 maxItr_Log2;
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

/* Cordinates */

void coordinate_to_pixel(fp32 xI, fp32 yI, int32_t* xO, int32_t* yO, ABS_Mandelbrot* param, Render_Data* ren);
void coordinate_to_pixel(fp64 xI, fp64 yI, int32_t* xO, int32_t* yO, ABS_Mandelbrot* param, Render_Data* ren);
#ifdef enableFP80andFP128
	void coordinate_to_pixel(fp80 xI, fp80 yI, int32_t* xO, int32_t* yO, ABS_Mandelbrot* param, Render_Data* ren);
	void coordinate_to_pixel(fp128 xI, fp128 yI, int32_t* xO, int32_t* yO, ABS_Mandelbrot* param, Render_Data* ren);
#endif

void pixel_to_coordinate(int32_t xI, int32_t yI, fp64* xO, fp64* yO, ABS_Mandelbrot* param, Render_Data* ren);
void pixel_to_coordinate(int32_t xI, int32_t yI, fp32* xO, fp32* yO, ABS_Mandelbrot* param, Render_Data* ren);
#ifdef enableFP80andFP128
	void pixel_to_coordinate(int32_t xI, int32_t yI, fp80* xO, fp80* yO, ABS_Mandelbrot* param, Render_Data* ren);
	void pixel_to_coordinate(int32_t xI, int32_t yI, fp128* xO, fp128* yO, ABS_Mandelbrot* param, Render_Data* ren);
#endif

void cpu_pixel_to_coordinate(int32_t xI, int32_t yI, fp32* xO, fp32* yO, ABS_Mandelbrot* param, uint32_t ResX, uint32_t ResY, uint32_t subSample);
void cpu_pixel_to_coordinate(int32_t xI, int32_t yI, fp64* xO, fp64* yO, ABS_Mandelbrot* param, uint32_t ResX, uint32_t ResY, uint32_t subSample);
#ifdef enableFP80andFP128
	void cpu_pixel_to_coordinate(int32_t xI, int32_t yI, fp80* xO, fp80* yO, ABS_Mandelbrot* param, uint32_t ResX, uint32_t ResY, uint32_t subSample);
	void cpu_pixel_to_coordinate(int32_t xI, int32_t yI, fp128* xO, fp128* yO, ABS_Mandelbrot* param, uint32_t ResX, uint32_t ResY, uint32_t subSample);
#endif

#define POLAR_POWER_MINIMUM 1.0100
#define POLAR_POWER_MAXIMUM 20.0
#define MANDELBROT_POWER_MAXIMUM 6

void setMaxItr(ABS_Mandelbrot* frac, fp64 val);
void setMaxItr(ABS_Mandelbrot* frac, uint32_t val);
void setStretchValue(ABS_Mandelbrot* frac);

void correctFracParameters(ABS_Mandelbrot* frac);

#endif /* FRACTAL_H */