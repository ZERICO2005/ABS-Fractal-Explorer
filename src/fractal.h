/*
**	Author: zerico2005 (2023-2024)
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
	Fractal_ABS_Mandelbrot, Fractal_Polar_Mandelbrot,
};

#ifndef BUILD_RELEASE
	const char* const FractalTypeText[] = {"ABS Mandelbrot", "Polar Mandelbrot"};
	const char* const FractalTypeFileText[] = {"ABS-Mandelbrot", "Polar-Mandelbrot"};
#else
	const char* const FractalTypeText[] = {"ABS Mandelbrot", "Polar Mandelbrot"};
	const char* const FractalTypeFileText[] = {"ABS-Mandelbrot", "Polar-Mandelbrot"};
#endif

const char* const PowerText[] = {
	"Constant", "Linear"  , "Quadratic", "Cubic"    , "Quartic"    , "Quintic"   , "Sextic"   , "Septic"    , "Octic"    , "Nonic"    ,
	"Decic"   , "Undecic" , "Dodecic"  , "Tridecic" , "Tetradecic" , "Pentadecic", "Hexadecic", "Heptadecic", "Octadecic", "Nonadecic",
	"Icosic"  , "Unicosic", "Duocosic" , "Triacosic", "Tetraicosic", "Pentacosic", "Hexacosic", "Heptacosic", "Octacosic", "Nonacosic"
};
 /* Safe Method of accessing PowerText */
const char* getPowerText(uint32_t p);
const char* getPowerText(fp64 p);

inline fp64 getABSFractalMinRadius(fp64 power) {
	return (power - 1.0) / (pow(power, power / (power - 1.0)));
}
inline fp64 getABSFractalMinRadius(uint32_t power) {
	return getABSFractalMinRadius((fp64)power);
}

inline fp64 getABSFractalMaxRadius(fp64 power) {
	return pow(2.0, 1.0 / (power - 1.0));
}
inline fp64 getABSFractalMaxRadius(uint32_t power) {
	return getABSFractalMaxRadius((fp64)power);
}

constexpr inline fp64 clampRotation(fp64 rotation) {
	return (rotation >= 0.0) ? fmod(rotation, TAU) : fmod(rotation + TAU, TAU);
}

uint64_t limitFormulaID(uint32_t power, uint64_t formula);
uint64_t getABSValue(uint32_t power);

inline fp64 getStretchValue(fp64 stretch) {
	return pow(2.0, -abs(stretch));
}

inline fp64 zoomDefault(fp64 power) {
	return -log10(getABSFractalMaxRadius(power) - 0.01);
}
inline fp64 zoomDefault(uint32_t power) {
	return -log10(getABSFractalMaxRadius((fp64)power) - 0.01);
}

enum Mandelbrot_Type {
	Mandelbrot_Unknown   = 0,
	Mandelbrot_Polar     = 1,
	Mandelbrot_Quadratic = 2,
	Mandelbrot_Cubic     = 3,
	Mandelbrot_Quartic   = 4,
	Mandelbrot_Quintic   = 5,
	Mandelbrot_Sextic    = 6
};

struct ABS_Mandelbrot {
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
	fpCord r;
	fpCord i;
	fpCord zr;
	fpCord zi;
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
	fp64 exterior_Alpha;
		fp64 exterior_R_Amp;
		fp64 exterior_R_Freq;
		fp64 exterior_R_Phase;
		fp64 exterior_G_Amp;
		fp64 exterior_G_Freq;
		fp64 exterior_G_Phase;
		fp64 exterior_B_Amp;
		fp64 exterior_B_Freq;
		fp64 exterior_B_Phase;
	fp64 interior_Alpha;
		fp64 interior_R_Amp;
		fp64 interior_R_Freq;
		fp64 interior_R_Phase;
		fp64 interior_G_Amp;
		fp64 interior_G_Freq;
		fp64 interior_G_Phase;
		fp64 interior_B_Amp;
		fp64 interior_B_Freq;
		fp64 interior_B_Phase;
};

void setDefaultParameters(ABS_Mandelbrot* frac, enum FractalTypeEnum type);

Mandelbrot_Type get_Mandelbrot_Type(const ABS_Mandelbrot& param);

/* Cordinates */

template <typename fpX, typename outputType>
void coordinate_to_pixel(fpX xI, fpX yI, outputType& xO, outputType& yO, const ABS_Mandelbrot& param, dim32_t resX, dim32_t resY) {
	/* Reverses Transformations */
	xI -= (fpX)param.r;
	yI -= (fpX)param.i;
	fpX xC = xI * cos((fpX)param.rot) + yI * sin((fpX)param.rot);
	fpX yC = yI * cos((fpX)param.rot) - xI * sin((fpX)param.rot);
	xC /= (fpX)param.sX;
	yC /= (fpX)param.sY;
	/* Normalizes Coordinates */
	dim32_t dimX = resX - 1;
	dim32_t dimY = resY - 1;
	dim32_t dimZ = (dimX >= dimY) ? dimY : dimX;
	xO = (outputType)( ( xC * pow((fpX)10.0, (fpX)param.zoom) / (fpX)2.0 * (((fpX)dimZ))) + (((fpX)dimX) / (fpX)2.0) );
	yO = (outputType)( (-yC * pow((fpX)10.0, (fpX)param.zoom) / (fpX)2.0 * (((fpX)dimZ))) + (((fpX)dimY) / (fpX)2.0) );
}

// Ignores real and imag cords, might be useful for determining rendering area.
template <typename fpX, typename outputType>
void coordinate_to_screen_pixel(fpX xI, fpX yI, outputType& xO, outputType& yO, const ABS_Mandelbrot& param, dim32_t resX, dim32_t resY) {
	/* Reverses Transformations */
	fpX xC = xI * cos((fpX)param.rot) + yI * sin((fpX)param.rot);
	fpX yC = yI * cos((fpX)param.rot) - xI * sin((fpX)param.rot);
	xC /= (fpX)param.sX;
	yC /= (fpX)param.sY;
	/* Normalizes Coordinates */
	dim32_t dimX = resX - 1;
	dim32_t dimY = resY - 1;
	dim32_t dimZ = (dimX >= dimY) ? dimY : dimX;
	xO = (outputType)( ( xC * pow((fpX)10.0, (fpX)param.zoom) / (fpX)2.0 * (((fpX)dimZ))) + (((fpX)dimX) / (fpX)2.0) );
	yO = (outputType)( (-yC * pow((fpX)10.0, (fpX)param.zoom) / (fpX)2.0 * (((fpX)dimZ))) + (((fpX)dimY) / (fpX)2.0) );
}

template <typename fpX>
void pixel_to_coordinate(int32_t xI, int32_t yI, fpX& xO, fpX& yO, const ABS_Mandelbrot& param, dim32_t resX, dim32_t resY) {
	/* Normalizes Coordinates */
	dim32_t dimX = resX - 1;
	dim32_t dimY = resY - 1;
	fpX numX = ((fpX)dimX / (fpX)2.0);
	fpX numY = ((fpX)dimY / (fpX)2.0);
	fpX numZ = (dimX >= dimY) ? numY * pow((fpX)10.0, (fpX)param.zoom) : numX * pow((fpX)10.0, (fpX)param.zoom);
	/* Applies Transformations */
	fpX xC = (((fpX)xI - numX) / numZ) * (fpX)param.sX;
	fpX yC = -(((fpX)yI - numY) / numZ) * (fpX)param.sY;
	xO = (xC * cos((fpX)param.rot) - yC * sin((fpX)param.rot)) + param.r;
	yO = (yC * cos((fpX)param.rot) + xC * sin((fpX)param.rot)) + param.i;
}

template <typename fpX>
void cpu_pixel_to_coordinate(int32_t xI, int32_t yI, fpX& xO, fpX& yO, fpX zoomVal, fpX rotSin, fpX rotCos, const ABS_Mandelbrot& param, dim32_t resX, dim32_t resY, int32_t subSample) { \
	/* Normalizes Coordinates */
	dim32_t dimX = resX - 1;
	dim32_t dimY = resY - 1;
	fpX numX = ((fpX)dimX / (fpX)2.0);
	fpX numY = ((fpX)dimY / (fpX)2.0);
	fpX numZ = (dimX >= dimY) ? numY * zoomVal : numX * zoomVal;
	/* Applies Transformations */
	fpX xC = (((fpX)xI - numX) / numZ) * (fpX)param.sX;
	fpX yC = -(((fpX)yI - numY) / numZ) * (fpX)param.sY;
	*xO = (xC * rotCos - yC * rotSin) + (fpX)param.r;
	*yO = (yC * rotCos + xC * rotSin) + (fpX)param.i;
}

constexpr fp64 POLAR_POWER_MINIMUM = 1.0100;
constexpr fp64 POLAR_POWER_MAXIMUM = 20.0;
constexpr int32_t MANDELBROT_POWER_MINIMUM = 2;
constexpr int32_t MANDELBROT_POWER_MAXIMUM = 6;

constexpr fp64 ZOOM_VALUE_MINIMUM = -5.0;
#if defined(Enable_Float128)
	constexpr fp64 ZOOM_VALUE_MAXIMUM = 4966.0; // Float128 limit: log10(2^-16382 * 2^-112) rounded up
#elif defined(Enable_Float80)
	constexpr fp64 ZOOM_VALUE_MAXIMUM = 4951.0; // Float80 limit: log10(2^-16382 * 2^-63) rounded up
#else
	constexpr fp64 ZOOM_VALUE_MAXIMUM = 324.0; // Float64 limit: log10(2^-1022 * 2^-52) rounded up
#endif

constexpr fp64 STRETCH_VALUE_MINIMUM = -100.0;
constexpr fp64 STRETCH_VALUE_MAXIMUM =  100.0;

constexpr uint32_t ITERATION_COUNT_MINIMUM = 16;
constexpr uint32_t ITERATION_COUNT_MAXIMUM = 16777216;

void setMaxItr(ABS_Mandelbrot* frac, fp64 val);
void setMaxItr(ABS_Mandelbrot* frac, uint32_t val);
void setStretchValue(ABS_Mandelbrot* frac);

void correctFracParameters(ABS_Mandelbrot* frac);

#endif /* FRACTAL_H */