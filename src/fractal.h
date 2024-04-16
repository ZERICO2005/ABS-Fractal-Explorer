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
	return pow(2.0,1.0 / (power - 1.0));
}
inline fp64 getABSFractalMaxRadius(uint32_t power) {
	return getABSFractalMaxRadius((fp64)power);
}

uint64_t limitFormulaID(uint32_t power, uint64_t formula);
uint64_t getABSValue(uint32_t power);

inline fp64 getStretchValue(fp64 stretch) {
	return pow(2.0,-abs(stretch));
}

inline fp64 zoomDefault(fp64 power) {
	return -log10(getABSFractalMaxRadius(power) - 0.01);
}
inline fp64 zoomDefault(uint32_t power) {
	return -log10(getABSFractalMaxRadius((fp64)power) - 0.01);
}


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

/* Cordinates */

template <typename fpX>
void coordinate_to_pixel(fpX xI, fpX yI, int32_t* xO, int32_t* yO, const ABS_Mandelbrot* param, const Render_Data* ren) {
	/* Reverses Transformations */
	fpX xC = xI * cos(-(fpX)param->rot) - yI * sin(-(fpX)param->rot) - param->r;
	fpX yC = yI * cos(-(fpX)param->rot) + xI * sin(-(fpX)param->rot) - param->i;
	xC /= (fpX)param->sX;
	yC /= (fpX)param->sY;
	/* Normalizes Coordinates */
	dim32_t resX = ren->resX - 1;
	dim32_t resY = ren->resY - 1;
	dim32_t resZ = (resX >= resY) ? resY : resX;
	*xO = (int32_t)( (xC * pow((fpX)10.0, (fpX)param->zoom) / (fpX)2.0 * (((fpX)resZ))) + (((fpX)resX) / (fpX)2.0) );
	*yO = (int32_t)( (-yC * pow((fpX)10.0, (fpX)param->zoom) / (fpX)2.0 * (((fpX)resZ))) + (((fpX)resY) / (fpX)2.0) );
}

template <typename fpX>
void coordinate_to_image_cordinate(fpX xI, fpX yI, fp32* xO, fp32* yO, const ABS_Mandelbrot* param, const Render_Data* ren) {
	/* Reverses Transformations */
	fpX xC = xI * cos(-(fpX)param->rot) - yI * sin(-(fpX)param->rot);
	fpX yC = yI * cos(-(fpX)param->rot) + xI * sin(-(fpX)param->rot);
	xC /= (fpX)param->sX;
	yC /= (fpX)param->sY;
	/* Normalizes Coordinates */
	dim32_t resX = ren->resX - 1;
	dim32_t resY = ren->resY - 1;
	dim32_t resZ = (resX >= resY) ? resY : resX;
	*xO = (fp32)( (xC * pow((fpX)10.0, (fpX)param->zoom) / (fpX)2.0 * (((fpX)resZ))) + (((fpX)resX) / (fpX)2.0) );
	*yO = (fp32)( (-yC * pow((fpX)10.0, (fpX)param->zoom) / (fpX)2.0 * (((fpX)resZ))) + (((fpX)resY) / (fpX)2.0) );
}

template <typename fpX>
void pixel_to_coordinate(int32_t xI, int32_t yI, fpX* xO, fpX* yO, const ABS_Mandelbrot* param, const Render_Data* ren) {
	/* Normalizes Coordinates */
	dim32_t resX = ren->resX - 1;
	dim32_t resY = ren->resY - 1;
	fpX numX = ((fpX)resX / 2.0);
	fpX numY = ((fpX)resY / 2.0);
	fpX numZ = (resX >= resY) ? numY * pow((fpX)10.0, (fpX)param->zoom) : numX * pow((fpX)10.0, (fpX)param->zoom);
	/* Applies Transformations */
	fpX xC = ((xI - numX) / numZ) * (fpX)param->sX;
	fpX yC = -((yI - numY) / numZ) * (fpX)param->sY;
	*xO = (xC * cos((fpX)param->rot) - yC * sin((fpX)param->rot)) + param->r;
	*yO = (yC * cos((fpX)param->rot) + xC * sin((fpX)param->rot)) + param->i;
}

template <typename fpX>
void cpu_pixel_to_coordinate(int32_t xI, int32_t yI, fpX* xO, fpX* yO, fpX zoomVal, fpX rotSin, fpX rotCos, const ABS_Mandelbrot* param, dim32_t ResX, dim32_t ResY, int32_t subSample) { \
	/* Normalizes Coordinates */
	dim32_t resX = ResX - 1;
	dim32_t resY = ResY - 1;
	fpX numX = ((fpX)resX / (fpX)2.0);
	fpX numY = ((fpX)resY / (fpX)2.0);
	fpX numZ = (resX >= resY) ? numY * zoomVal : numX * zoomVal;
	/* Applies Transformations */
	fpX xC = (((fpX)xI - numX) / numZ) * (fpX)param->sX;
	fpX yC = -(((fpX)yI - numY) / numZ) * (fpX)param->sY;
	*xO = (xC * rotCos - yC * rotSin) + (fpX)param->r;
	*yO = (yC * rotCos + xC * rotSin) + (fpX)param->i;
}

constexpr fp64 POLAR_POWER_MINIMUM = 1.0100;
constexpr fp64 POLAR_POWER_MAXIMUM = 20.0;
constexpr int32_t MANDELBROT_POWER_MAXIMUM = 6;

void setMaxItr(ABS_Mandelbrot* frac, fp64 val);
void setMaxItr(ABS_Mandelbrot* frac, uint32_t val);
void setStretchValue(ABS_Mandelbrot* frac);

void correctFracParameters(ABS_Mandelbrot* frac);

#endif /* FRACTAL_H */