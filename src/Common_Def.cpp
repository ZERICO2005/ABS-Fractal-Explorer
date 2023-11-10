/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"

/* Functions */

fp64 calcMinMaxRatio(fp64 val, fp64 min, fp64 max, fp64 ratio) {
    if (val < min) {
        val = min;
    }
    if (val <= max) {
        return val;
    }
    val *= ratio;
    if (val < max) {
        val = max;
    }
    return val;
}

fp32 calcMinMaxRatio(fp32 val, fp32 min, fp32 max, fp32 ratio) {
    if (val < min) {
        val = min;
    }
    if (val <= max) {
        return val;
    }
    val *= ratio;
    if (val < max) {
        val = max;
    }
    return val;
}

uint32_t calcMinMaxRatio(uint32_t val, uint32_t min, uint32_t max, fp64 ratio) {
    if (val < min) {
        val = min;
    }
    if (val <= max) {
        return val;
    }
    val = (uint32_t)((fp64)val * ratio);
    if (val < max) {
        val = max;
    }
    return val;
}

/* Time */

uint64_t getNanoTime() { // Returns the time in nanoseconds
	using nanoseconds = std::chrono::duration<std::uint64_t, std::nano>;
	auto now = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<nanoseconds>(now.time_since_epoch()).count();
}

fp64 getDecimalTime() { // Returns the time in seconds
	return (fp64)getNanoTime() / 1.0e9;
}

/* Color */

// (&R,&G,&B) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
void getRGBfromHSV(uint8_t* r, uint8_t* g, uint8_t* b, fp64 hue, fp64 sat, fp64 val) { 
	if (r == NULL || g == NULL || b == NULL) { return; }
	uint8_t hi = (uint8_t)(hue / 60.0) % 6;
	fp64 f = (hue / 60.0) - floor(hue / 60.0);
	val *= 255.0;
	uint8_t vR = (uint8_t)val;
	uint8_t pR = (uint8_t)(val * (1.0 - sat));
	uint8_t qR = (uint8_t)(val * (1.0 - f * sat));
	uint8_t tR = (uint8_t)(val * (1.0 - (1.0 - f) * sat));
	switch(hi) {
		case 0:
		*r = vR; *g = tR; *b = pR;
		return;
		case 1:
		*r = qR; *g = vR; *b = pR;
		return;
		case 2:
		*r = pR; *g = vR; *b = tR;
		return;
		case 3:
		*r = pR; *g = qR; *b = vR;
		return;
		case 4:
		*r = tR; *g = pR; *b = vR;
		return;
		case 5:
		*r = vR; *g = pR; *b = qR;
		return;
	}
}

// (&R,&G,&B) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
void getRGBfromHSV(uint8_t* r, uint8_t* g, uint8_t* b, fp32 hue, fp32 sat, fp32 val) {
	if (r == NULL || g == NULL || b == NULL) { return; }
	uint8_t hi = (uint8_t)(hue / 60.0) % 6;
	fp32 f = (hue / 60.0) - floor(hue / 60.0);
	val *= 255.0;
	uint8_t vR = (uint8_t)val;
	uint8_t pR = (uint8_t)(val * (1.0 - sat));
	uint8_t qR = (uint8_t)(val * (1.0 - f * sat));
	uint8_t tR = (uint8_t)(val * (1.0 - (1.0 - f) * sat));
	switch(hi) {
		case 0:
		*r = vR; *g = tR; *b = pR;
		return;
		case 1:
		*r = qR; *g = vR; *b = pR;
		return;
		case 2:
		*r = pR; *g = vR; *b = tR;
		return;
		case 3:
		*r = pR; *g = qR; *b = vR;
		return;
		case 4:
		*r = tR; *g = pR; *b = vR;
		return;
		case 5:
		*r = vR; *g = pR; *b = qR;
		return;
	}
}

// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
uint32_t getRGBfromHSV(fp64 hue, fp64 sat, fp64 val) {
	uint8_t r,g,b;
	getRGBfromHSV(&r,&g,&b,hue,sat,val);
	return (r << 16) + (g << 8) + b;
}

// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
uint32_t getRGBfromHSV(fp32 hue, fp32 sat, fp32 val) {
	uint8_t r,g,b;
	getRGBfromHSV(&r,&g,&b,hue,sat,val);
	return (r << 16) + (g << 8) + b;
}
