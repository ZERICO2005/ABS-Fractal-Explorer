/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef GAMMA_H
#define GAMMA_H

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <time.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float fp32;
typedef double fp64;

typedef int int_enum;

typedef int64_t nano64_t;

#include "RGB888.h"

#define PI 		3.1415926535897932384626433832795
#define TAU 	6.2831853071795864769252867665590
#define EULER 	2.7182818284590452353602874713527

struct fractalData {
	uint8_t type;
	uint8_t julia;
	uint64_t formula;
	fp64 r;
	fp64 i;
	fp64 zoom;
	uint32_t itr;
	fp64 itrVal;
	fp64 zr;
	fp64 zi;
	uint32_t resX;
	uint32_t resY;
	uint32_t samples;
};
typedef struct fractalData frac;

#define shallowCopy(dst,src) memcpy(dst, src, sizeof (*dst))
#define fpInc(x) nextafter(x,x-copysign(1.0,x))
#define fpDec(x) nextafter(x,x+copysign(1.0,x))

/* Functions */
	// Fills a buffer with a repeating pattern of N bytes
	void* patternMemcpy(void* __restrict__ buf, size_t bufSize, const void* __restrict__ PatternData, size_t PatternSize);

	// Assumes the pattern is set in the first N bytes in buf
	void* inPlacePatternMemcpy(void* __restrict__ buf, size_t bufSize, size_t PatternSize);


#endif /* GAMMA_H */