/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef COMMON_DEF_H
#define COMMON_DEF_H

/*
**	Common_Def.h is ideally constant and unchanging. Containing commonly used functions, includes, defines, macros and constants needed in all files
**	Common_Def.h should be used for standalone code/classes
*/

/* Includes */

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <stdio.h>
#include <iostream>

#include <chrono>
#include <time.h>

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <algorithm>

#include <cstdarg>
#include <stdarg.h>
#include <stdexcept>
#include <limits.h>

/* Typedefs */

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

typedef int64_t nano64_t;

typedef int int_enum;

/* Version */

//#define BUILD_DEBUG
#define BUILD_RELEASE

/* Constants */

#define PI 		3.1415926535897932384626433832795
#define TAU 	6.2831853071795864769252867665590
#define EULER 	2.7182818284590452353602874713527

/* Macros */

#define STR_M(x) #x
#define STR_N(x) STR_M(x)

#define ARRAY_LENGTH(x) ( (sizeof(x[0]) != 0) ? (sizeof(x) / sizeof(x[0])) : 0 )
#define TEXT_LENGTH(x) ( (sizeof(x[0]) != 0) ? ( ((sizeof(x) / sizeof(x[0])) != 0) ? ((sizeof(x) / sizeof(x[0])) - 1) : 0 ) : 0 )
#define FREE(x) free(x); x = nullptr

/* Functions */
	// Left Circular Shift
	#define ROL(n,b) (((n) << (b)) | ((n) >> ((sizeof(n) * CHAR_BIT) - (b))))
	// Right Circular Shift
	#define ROR(n,b) (((n) >> (b)) | ((n) << ((sizeof(n) * CHAR_BIT) - (b))))

	#define boolText(bool) ((bool) ? "true" : "false")
	#define BoolText(bool) ((bool) ? "True" : "False")

	#define valueLimit(value,minimum,maximum) ( ((value) < (minimum)) ? ((value) = (minimum)) : ( ((value) > (maximum)) ? ((value) = (maximum)) : ((value) = (value)) ) )
	#define valueMinimum(value,minimum) ( ((value) < (minimum)) ? ((value) = (minimum)) : ((value) = (value)) )
	#define valueMaximum(value,maximum) ( ((value) > (maximum)) ? ((value) = (maximum)) : ((value) = (value)) )
	#define valueRestore(value,restore,minimum,maximum) ( ((value) >= (minimum) && (value) <= (maximum)) ? ((value) = (value)) : ((value) = (restore)) )

	int patternMemcpy(uint8_t* buf, size_t bufSize, const uint8_t* PatternData, size_t PatternSize);

	int inPlacePatternMemcpy(uint8_t* buf, size_t bufSize, size_t PatternSize);

	fp64 calcMinMaxRatio(fp64 val, fp64 min, fp64 max, fp64 ratio);
	fp32 calcMinMaxRatio(fp32 val, fp32 min, fp32 max, fp32 ratio);
	uint32_t calcMinMaxRatio(uint32_t val, uint32_t min, uint32_t max, fp64 ratio);
	fp64 calcMinRatioMax(fp64 val, fp64 min, fp64 ratio, fp64 max);
	fp32 calcMinRatioMax(fp32 val, fp32 min, fp32 ratio, fp32 max);
	uint32_t calcMinRatioMax(uint32_t val, uint32_t min, fp64 ratio, uint32_t max);

	#define linearInterpolation(x,x0,x1,y0,y1) ( (y0) + ( (((y1) - (y0)) * ((x) - (x0))) / ((x1) - (x0)) ) )
	#define linearInterpolationClamp(x,x0,x1,y0,y1) ( ((x) <= (x0)) ? (y0) : ( ((x) >= (x1)) ? (y1) : linearInterpolation((x),(x0),(x1),(y0),(y1)) ) )

/* Time */
	// Returns the time in nanoseconds
	nano64_t getNanoTime();
	// Returns the time in seconds 
	fp64 getDecimalTime();

	#define SECONDS_TO_NANO(t) (nano64_t)((t) * 1.0e9)
	#define NANO_TO_SECONDS(t) ((fp64)(t) / 1.0e9)

	// // Callocs the date and time in UTC
	// const char* getDateAndTimeUTC(
	// 	char DateDelimiter = '-', char DateSeparator = '_', char TimeDelimiter = '-'
	// );

/* Print Functions */
	#define printFlush(...) printf(__VA_ARGS__); fflush(stdout)
	#define printFatalError(...) printf("\n============\nFATAL ERROR: "); printf(__VA_ARGS__); printf("\n============\n"); fflush(stdout)
	#define printCriticalError(...) printf("\nCRITICAL ERROR: "); printf(__VA_ARGS__); printf("\n"); fflush(stdout)
	#define printError(...) printf("\nError: "); printf(__VA_ARGS__); printf("\n"); fflush(stdout)
	#define printWarning(...) printf("\nWarning: "); printf(__VA_ARGS__); printf("\n"); fflush(stdout)

	// Print change in value, also calls fflush(stdout);
	#define printfChange(type,value,...) \
	{ \
		static type Detect_Change = (value); \
		if (Detect_Change != (value)) { \
			printf(__VA_ARGS__); \
			fflush(stdout); \
			Detect_Change = (value); \
		} \
	}

	// Print up to every (freq) seconds, also calls fflush(stdout);
	#define printfInterval(freq,...); \
	{ \
		static nano64_t ResetTime_PrintfInterval = getNanoTime(); \
		if (getNanoTime() - ResetTime_PrintfInterval > (nano64_t)((freq) * 1.0e9)) { \
			ResetTime_PrintfInterval = getNanoTime(); \
			printf(__VA_ARGS__); \
			fflush(stdout); \
		} \
	}

	// Print change in value up to every (freq) seconds, also calls fflush(stdout);
	#define printfChangeInterval(type,value,freq,...) \
	{ \
		static nano64_t ResetTime_PrintfInterval = getNanoTime(); \
		static type Detect_Change = (value); \
		if (getNanoTime() - ResetTime_PrintfInterval > (nano64_t)((freq) * 1.0e9)) { \
			if (Detect_Change != (value)) { \
				ResetTime_PrintfInterval = getNanoTime(); \
				printf(__VA_ARGS__); \
				fflush(stdout); \
				Detect_Change = (value); \
			} \
		} \
	}
	
	// Waits for a duration in seconds
	#define BurnTime(s) \
	{\
		nano64_t BURN_TIME_NANOSECONDS = getNanoTime();\
		while (getNanoTime() - BURN_TIME_NANOSECONDS < SECONDS_TO_NANO(s)) {};\
	}

/* Color */
	// (&R,&G,&B) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
	void getRGBfromHSV(uint8_t* r, uint8_t* g, uint8_t* b, fp64 hue, fp64 sat, fp64 val);
	// (&R,&G,&B) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
	void getRGBfromHSV(uint8_t* r, uint8_t* g, uint8_t* b, fp32 hue, fp32 sat, fp32 val);
	// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
	uint32_t getRGBfromHSV(fp64 hue, fp64 sat, fp64 val);
	// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
	uint32_t getRGBfromHSV(fp32 hue, fp32 sat, fp32 val);

	// (&R,&G,&B,&A) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0, A 0.0-1.0
	void getRGBAfromHSVA(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a, fp64 hue, fp64 sat, fp64 val, fp64 alpha);
	// (&R,&G,&B,&A) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0, A 0.0-1.0
	void getRGBAfromHSVA(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a, fp32 hue, fp32 sat, fp32 val, fp32 alpha);
	// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0, A 0.0-1.0
	uint32_t getRGBAfromHSVA(fp64 hue, fp64 sat, fp64 val, fp64 alpha);
	// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0, A 0.0-1.0
	uint32_t getRGBAfromHSVA(fp32 hue, fp32 sat, fp32 val, fp32 alpha);

#endif /* COMMON_DEF_H */