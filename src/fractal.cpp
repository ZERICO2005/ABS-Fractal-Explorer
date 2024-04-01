/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"

#include "fractal.h"

/* Safe Method of accessing PowerText */
const char* getPowerText(uint32_t p) {
	if (p < (uint32_t)ARRAY_LENGTH(PowerText)) {
		return PowerText[p];
	}
	return NULL;
}
const char* getPowerText(fp64 p) {
	return getPowerText((uint32_t)p);
}

uint64_t limitFormulaID(uint32_t power, uint64_t formula) {
	switch(power) {
		case 2:
			formula %= (1 << 8); break;
		case 3:
			formula %= (1 << 14); break;
		case 4:
			formula %= (1 << 17); break;
		case 5:
			formula %= (1 << 20); break;
		case 6:
			formula %= (1 << 23); break;
	};
	return formula;
}

uint64_t getABSValue(uint32_t power) {
	switch(power) {
		case 2:
			return (1 << 3); // Bit 3
		case 3:
			return (1 << 6); // Bit 6
		case 4:
			return (1 << 7); // Bit 7
		case 5:
			return (1 << 8); // Bit 8
		case 6:
			return (1 << 9); // Bit 9
		default:
			return (1 << 0);
	};
	return 1; // Unknown
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
		FRAC.adjustZoomToPower = false;
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

void setMaxItr(ABS_Mandelbrot* frac, fp64 val) {
	if (frac == nullptr) { return; }
	valueClamp(val,log2(16.0),log2(16777216.0));
	frac->maxItr = (uint32_t)pow(2.0,val);
	frac->maxItr_Log2 = val;
}
void setMaxItr(ABS_Mandelbrot* frac, uint32_t val) {
	if (frac == nullptr) { return; }
	valueClamp(val,16,16777216);
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
	valueClamp(frac->power,2,MANDELBROT_POWER_MAXIMUM);
	valueClamp(frac->polarPower,POLAR_POWER_MINIMUM,POLAR_POWER_MAXIMUM);
	if (frac->polarMandelbrot == true) {
		if (frac->integerPolarPower == true) {
			frac->polarPower = round(frac->polarPower);
			valueClamp(frac->polarPower,ceil(POLAR_POWER_MINIMUM),floor(POLAR_POWER_MAXIMUM));
		} else {
			valueClamp(frac->polarPower,POLAR_POWER_MINIMUM,POLAR_POWER_MAXIMUM);
		}
	}
	frac->formula = limitFormulaID(frac->power,frac->formula);
	if (frac->lockToCardioid == true) {
		frac->r = (fp128)getABSFractalMinRadius(frac->polarPower);
		frac->r *= (frac->flipCardioidSide == true) ? (fp128)-1.0 : (fp128)1.0;
	}
	valueClamp(frac->r,(fp128)-10.0,(fp128)10.0);
	valueClamp(frac->i,(fp128)-10.0,(fp128)10.0);
	valueClamp(frac->zoom,-5.0,40.0);
	valueClamp(frac->zr,(fp128)-4.0,(fp128)4.0);
	valueClamp(frac->zi,(fp128)-4.0,(fp128)4.0);
	valueClamp(frac->maxItr,16,16777216);
	valueClamp(frac->maxItr_Log2,log2(16.0),log2(16777216.0));
	frac->rot = (frac->rot >= 0.0) ? fmod(frac->rot,TAU) : fmod(frac->rot + TAU,TAU);
	valueClamp(frac->stretch,-100.0,100.0);
	setStretchValue(frac);
	valueClamp(frac->breakoutValue,0.25,4294967296.0);
	valueClamp(frac->rA,-1.0,1.0);
	valueClamp(frac->gA,-1.0,1.0);
	valueClamp(frac->bA,-1.0,1.0);
	valueClamp(frac->iA,-1.0,1.0);
}