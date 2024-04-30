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

void setDefaultParameters(ABS_Mandelbrot* frac, enum FractalTypeEnum type) {
	if (frac == nullptr) {
		printError("Fractal_Data* frac is NULL in setDefaultParameters()");
		return;
	}
	ABS_Mandelbrot& FRAC = *frac;
	switch(type) {
		case Fractal_ABS_Mandelbrot: {
			FRAC.juliaSet = false;
			FRAC.startingZ = true;
			FRAC.screenSplit = 0;
			FRAC.swapJuliaSplit = false;
			FRAC.cursorZValue = false;
			FRAC.relativeZValue = false;
			FRAC.showFloatingJulia = false;
			FRAC.adjustZoomToPower = false;
			FRAC.polarMandelbrot = false;
			FRAC.lockToCardioid = false;
			FRAC.flipCardioidSide = false;
			/* Coordinates */
			FRAC.r = 0.0;
			FRAC.i = 0.0;
			FRAC.zr = 0.0;
			FRAC.zi = 0.0;
			FRAC.zoom = -log10(getABSFractalMaxRadius(2.0));
			/* Transformation */
			FRAC.rot = 0.0;
			FRAC.sX = 1.0;
			FRAC.sY = 1.0;
			FRAC.stretch = 0.0;
			/* Parameters */
			FRAC.maxItr = 192;
			FRAC.maxItr_Log2 = log2(192.0);
			FRAC.power = 2;
			FRAC.polarPower = 2.0;
			FRAC.integerPolarPower = false;
			FRAC.formula = 0;
			FRAC.breakoutValue = 4096.0;
			/* Coloring */

			FRAC.smoothColoring = true;
			FRAC.exterior_Alpha = 1.0;
				FRAC.exterior_R_Amp   = 0.9 ;
				FRAC.exterior_R_Freq  = 0.45;
				FRAC.exterior_R_Phase = 0.5 ;
				FRAC.exterior_G_Amp   = 1.0 ;
				FRAC.exterior_G_Freq  = 0.45;
				FRAC.exterior_G_Phase = 0.9 ;
				FRAC.exterior_B_Amp   = 1.0 ;
				FRAC.exterior_B_Freq  = 0.45;
				FRAC.exterior_B_Phase = 0.1 ;
			FRAC.interior_Alpha = 1.0;
				FRAC.interior_R_Amp   = 0.0;
				FRAC.interior_R_Freq  = 0.5;
				FRAC.interior_R_Phase = 0.0;
				FRAC.interior_G_Amp   = 0.0;
				FRAC.interior_G_Freq  = 0.5;
				FRAC.interior_G_Phase = 0.0;
				FRAC.interior_B_Amp   = 1.0;
				FRAC.interior_B_Freq  = 0.5;
				FRAC.interior_B_Phase = 0.0;
		} break;
		case Fractal_Polar_Mandelbrot: {
			
		}
			FRAC.juliaSet = false;
			FRAC.startingZ = true;
			FRAC.screenSplit = 0;
			FRAC.swapJuliaSplit = false;
			FRAC.cursorZValue = false;
			FRAC.relativeZValue = false;
			FRAC.showFloatingJulia = false;
			FRAC.adjustZoomToPower = false;
			FRAC.polarMandelbrot = true;
			FRAC.integerPolarPower = false;
			FRAC.lockToCardioid = false;
			FRAC.flipCardioidSide = false;
			/* Coordinates */
			FRAC.r = 0.0;
			FRAC.i = 0.0;
			FRAC.zr = 0.0;
			FRAC.zi = 0.0;
			FRAC.zoom = -log10(getABSFractalMaxRadius(3.0));
			/* Transformation */
			FRAC.rot = 0.0;
			FRAC.sX = 1.0;
			FRAC.sY = 1.0;
			FRAC.stretch = 0.0;
			/* Parameters */
			FRAC.maxItr = 192;
			FRAC.maxItr_Log2 = log2(192.0);
			FRAC.power = 3;
			FRAC.polarPower = 3.0;
			FRAC.formula = 0;
			FRAC.breakoutValue = 4096.0;
			/* Coloring */
			FRAC.smoothColoring = true;
			FRAC.exterior_Alpha = 1.0;
				FRAC.exterior_R_Amp   = 0.875  ;
				FRAC.exterior_R_Freq  = 0.59375;
				FRAC.exterior_R_Phase = 0.3125 ;
				FRAC.exterior_G_Amp   = 0.9375 ;
				FRAC.exterior_G_Freq  = 0.59375;
				FRAC.exterior_G_Phase = 0.9375 ;
				FRAC.exterior_B_Amp   = 1.0    ;
				FRAC.exterior_B_Freq  = 0.59375;
				FRAC.exterior_B_Phase = 0.6875 ;
			FRAC.interior_Alpha = 1.0;
				FRAC.interior_R_Amp   = 0.0;
				FRAC.interior_R_Freq  = 0.5;
				FRAC.interior_R_Phase = 0.0;
				FRAC.interior_G_Amp   = 0.0;
				FRAC.interior_G_Freq  = 0.5;
				FRAC.interior_G_Phase = 0.0;
				FRAC.interior_B_Amp   = 1.0;
				FRAC.interior_B_Freq  = 0.5;
				FRAC.interior_B_Phase = 0.0;
		break;
		default:
			printError("Unknown Fractal Type: %d",type);
			return;
	}
}

Mandelbrot_Type get_Mandelbrot_Type(const ABS_Mandelbrot& param) {
	if (param.polarMandelbrot == true) {
		return Mandelbrot_Polar;
	}
	switch(param.power) {
		case 2:
			return Mandelbrot_Quadratic;
		case 3:
			return Mandelbrot_Cubic;
		case 4:
			return Mandelbrot_Quartic;
		case 5:
			return Mandelbrot_Quintic;
		case 6:
			return Mandelbrot_Sextic;
		default:
			return Mandelbrot_Unknown;
	}
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
		frac->r = (fpCord)getABSFractalMinRadius(frac->polarPower);
		frac->r *= (frac->flipCardioidSide == true) ? (fpCord)-1.0 : (fpCord)1.0;
	}
	valueClamp(frac->r,(fpCord)-10.0,(fpCord)10.0);
	valueClamp(frac->i,(fpCord)-10.0,(fpCord)10.0);
	valueClamp(frac->zoom,-5.0,40.0);
	valueClamp(frac->zr,(fpCord)-4.0,(fpCord)4.0);
	valueClamp(frac->zi,(fpCord)-4.0,(fpCord)4.0);
	valueClamp(frac->maxItr,16,16777216);
	valueClamp(frac->maxItr_Log2,log2(16.0),log2(16777216.0));
	frac->rot = (frac->rot >= 0.0) ? fmod(frac->rot,TAU) : fmod(frac->rot + TAU,TAU);
	valueClamp(frac->stretch,-100.0,100.0);
	setStretchValue(frac);
	valueClamp(frac->breakoutValue,0.25,4294967296.0);
	
	valueClamp(frac->exterior_Alpha, 0.0, 1.0);
		valueClamp(frac->exterior_R_Amp, -1.0, 1.0);
		valueClamp(frac->exterior_G_Amp, -1.0, 1.0);
		valueClamp(frac->exterior_B_Amp, -1.0, 1.0);
	valueClamp(frac->interior_Alpha, 0.0, 1.0);
		valueClamp(frac->interior_R_Amp, -1.0, 1.0);
		valueClamp(frac->interior_G_Amp, -1.0, 1.0);
		valueClamp(frac->interior_B_Amp, -1.0, 1.0);
}