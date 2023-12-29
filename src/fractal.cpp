/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"

#include "fractal.h"

/* Safe Method of accessing PowerText */
const char* getPowerText(int32_t p) {
    if (p >= 0 && p <= (int32_t)ARRAY_LENGTH(PowerText)) {
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

uint64_t limitFormulaID(uint32_t power, uint64_t formula) {
	switch(power) {
		case 2:
		formula %= 256; break;
		case 3:
		formula %= 16384; break;
		case 4:
		formula %= 131072; break;
		case 5:
		formula %= 1048576; break;
		case 6:
		formula %= 8388608; break;
	};
	return formula;
}

uint64_t getABSValue(uint32_t power) {
	switch(power) {
		case 2:
		return 8; // Bit 3
		case 3:
		return 64; // Bit 6
		case 4:
		return 128; // Bit 7
		case 5:
		return 256; // Bit 8
		case 6:
		return 512; // Bit 9
		default:
		return 1;
	};
	return 1; // Unknown
}

fp64 getStretchValue(fp64 s) {
	return pow(2.0,-abs(s));
}
fp64 getZoomDefault(fp64 p) {
	return (-log10(getABSFractalMaxRadius((fp64)(p))) - 0.01);
}

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
		FRAC.maxItr_Log2 = log2(192.0);
		FRAC.power = 2;
		FRAC.polarPower = 2.0;
		FRAC.integerPolarPower = false;
		FRAC.formula = 0;
		FRAC.breakoutValue = 4096.0;
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
		FRAC.integerPolarPower = false;
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
		FRAC.maxItr_Log2 = log2(192.0);
		FRAC.power = 3;
		FRAC.polarPower = 3.0;
		FRAC.formula = 0;
		FRAC.breakoutValue = 4096.0;
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

/* Coordinates */

#define pixel_to_coordinate_template(fpX); \
void pixel_to_coordinate(int32_t xI, int32_t yI, fpX* xO, fpX* yO, ABS_Mandelbrot* param, Render_Data* ren) { \
	/* Normalizes Coordinates */\
	uint32_t resX = ren->resX - 1;\
	uint32_t resY = ren->resY - 1;\
	fpX numX = ((fpX)resX / 2.0);\
	fpX numY = ((fpX)resY / 2.0);\
	fpX numZ = (resX >= resY) ? numY * pow((fpX)10.0, (fpX)param->zoom) : numX * pow((fpX)10.0, (fpX)param->zoom);\
	/* Applies Transformations */\
	fpX xC = ((xI - numX) / numZ) * (fpX)param->sX;\
	fpX yC = -((yI - numY) / numZ) * (fpX)param->sY;\
	*xO = (xC * cos((fpX)param->rot) - yC * sin((fpX)param->rot)) + param->r;\
	*yO = (yC * cos((fpX)param->rot) + xC * sin((fpX)param->rot)) + param->i;\
}
pixel_to_coordinate_template(fp32);
pixel_to_coordinate_template(fp64);
#ifdef enableFP80andFP128
	pixel_to_coordinate_template(fp80);
	pixel_to_coordinate_template(fp128);
#endif

#define coordinate_to_pixel_template(fpX); \
void coordinate_to_pixel(fpX xI, fpX yI, int32_t* xO, int32_t* yO, ABS_Mandelbrot* param, Render_Data* ren) {\
	/* Reverses Transformations */\
	fpX xC = xI * cos(-(fpX)param->rot) - yI * sin(-(fpX)param->rot);\
	fpX yC = yI * cos(-(fpX)param->rot) + xI * sin(-(fpX)param->rot);\
	xC /= (fpX)param->sX;\
	yC /= (fpX)param->sY;\
	/* Normalizes Coordinates */\
	uint32_t resX = ren->resX - 1;\
	uint32_t resY = ren->resY - 1;\
	uint32_t resZ = (resX >= resY) ? resY : resX;\
	*xO = (int32_t)((xC) * pow((fpX)10.0, (fpX)param->zoom) / (fpX)2.0 * (((fpX)resZ))) + (((fpX)resX) / (fpX)2.0);\
	*yO = (int32_t)(-(yC) * pow((fpX)10.0, (fpX)param->zoom) / (fpX)2.0 * (((fpX)resZ))) + (((fpX)resY) / (fpX)2.0);\
}
coordinate_to_pixel_template(fp32);
coordinate_to_pixel_template(fp64);
#ifdef enableFP80andFP128
	coordinate_to_pixel_template(fp80);
	coordinate_to_pixel_template(fp128);
#endif

#define cpu_pixel_to_coordinate_template(fpX); \
void cpu_pixel_to_coordinate(int32_t xI, int32_t yI, fpX* xO, fpX* yO, ABS_Mandelbrot* param, uint32_t ResX, uint32_t ResY, uint32_t subSample) { \
	/* Normalizes Coordinates */\
	uint32_t resX = ResX - 1;\
	uint32_t resY = ResY - 1;\
	fpX numX = ((fpX)resX / 2.0);\
	fpX numY = ((fpX)resY / 2.0);\
	fpX numZ = (resX >= resY) ? numY * pow((fpX)10.0, (fpX)param->zoom) : numX * pow((fpX)10.0, (fpX)param->zoom);\
	/* Applies Transformations */\
	fpX xC = ((xI - numX) / numZ) * (fpX)param->sX;\
	fpX yC = -((yI - numY) / numZ) * (fpX)param->sY;\
	*xO = (xC * cos((fpX)param->rot) - yC * sin((fpX)param->rot)) + param->r;\
	*yO = (yC * cos((fpX)param->rot) + xC * sin((fpX)param->rot)) + param->i;\
}
cpu_pixel_to_coordinate_template(fp32);
cpu_pixel_to_coordinate_template(fp64);
#ifdef enableFP80andFP128
	cpu_pixel_to_coordinate_template(fp80);
	cpu_pixel_to_coordinate_template(fp128);
#endif

void setMaxItr(ABS_Mandelbrot* frac, fp64 val) {
	if (frac == nullptr) { return; }
	valueLimit(val,log2(16.0),log2(16777216.0));
	frac->maxItr = pow(2.0,val);
	frac->maxItr_Log2 = val;
}
void setMaxItr(ABS_Mandelbrot* frac, uint32_t val) {
	if (frac == nullptr) { return; }
	valueLimit(val,16,16777216);
	frac->maxItr = val;
	frac->maxItr_Log2 = log2((fp64)val);
}
void setStretchValue(ABS_Mandelbrot* frac) {
	if (frac == nullptr) { return; }
	if (frac->stretch >= 0.0) {
		frac->sX = 1.0;
		frac->sY = getStretchValue(frac->stretch);
	} else {
		frac->sX = getStretchValue(frac->stretch);
		frac->sY = 1.0;
	}
}

void correctFracParameters(ABS_Mandelbrot* frac) {
	if (frac == nullptr) { return; }
	valueLimit(frac->power,2,MANDELBROT_POWER_MAXIMUM);
	valueLimit(frac->polarPower,POLAR_POWER_MINIMUM,POLAR_POWER_MAXIMUM);
	if (frac->polarMandelbrot == true) {
		if (frac->integerPolarPower == true) {
			frac->polarPower = round(frac->polarPower);
			valueLimit(frac->polarPower,ceil(POLAR_POWER_MINIMUM),floor(POLAR_POWER_MAXIMUM));
		} else {
			valueLimit(frac->polarPower,POLAR_POWER_MINIMUM,POLAR_POWER_MAXIMUM);
		}
	}
	frac->formula = limitFormulaID(frac->power,frac->formula);
	if (frac->lockToCardioid == true) {
		frac->r = (fp128)getABSFractalMinRadius(frac->polarPower);
		frac->r *= (frac->flipCardioidSide == true) ? (fp128)-1.0 : (fp128)1.0;
	}
	valueLimit(frac->r,(fp128)-10.0,(fp128)10.0);
	valueLimit(frac->i,(fp128)-10.0,(fp128)10.0);
	valueLimit(frac->zoom,-5.0,40.0);
	valueLimit(frac->zr,(fp128)-4.0,(fp128)4.0);
	valueLimit(frac->zi,(fp128)-4.0,(fp128)4.0);
	valueLimit(frac->maxItr,16,16777216);
	valueLimit(frac->maxItr_Log2,log2(16.0),log2(16777216.0));
	frac->rot = (frac->rot >= 0.0) ? fmod(frac->rot,TAU) : fmod(frac->rot + TAU,TAU);
	valueLimit(frac->stretch,-100.0,100.0);
	setStretchValue(frac);
	valueLimit(frac->breakoutValue,0.25,4294967296.0);
	valueLimit(frac->rA,-1.0,1.0);
	valueLimit(frac->gA,-1.0,1.0);
	valueLimit(frac->bA,-1.0,1.0);
	valueLimit(frac->iA,-1.0,1.0);
}