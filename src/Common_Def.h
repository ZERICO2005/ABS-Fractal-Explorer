/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef COMMON_DEF_H
#define COMMON_DEF_H

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

#include <cstdarg>
#include <stdarg.h>
#include <stdexcept>
#include <limits.h>
#include <quadmath.h>

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

#define enableFP80andFP128

#ifdef enableFP80andFP128
	typedef __float128 fp128;

	#ifdef __GNUC__
		typedef __int128_t i128;
		typedef __uint128_t u128;
		typedef __float80 fp80;
	#else
		typedef int64_t i128;
		typedef uint64_t u128;
		typedef __float128 fp80;
	#endif

	fp128 sin(fp128 x);
	fp128 cos(fp128 x);
	fp128 log(fp128 x);
	fp128 log2(fp128 x);
	fp128 log10(fp128 x);
	fp128 pow(fp128 x, fp128 y);
	fp128 floor(fp128 x);
	fp128 ceil(fp128 x);
	fp128 fmax(fp128 x, fp128 y);
	fp128 fmin(fp128 x, fp128 y);
	fp128 fmod(fp128 x, fp128 y);
	fp128 fabs(fp128 x);
	fp128 copysign(fp128 x, fp128 y);
#endif

/* Version */

#define PROGRAM_NAME "ABS-Fractal-Explorer"
#define PROGRAM_DATE "2023/11/04" /* YYYY/MM/DD */
#define PROGRAM_V_MAJOR 1
#define PROGRAM_V_MINOR 0
#define PROGRAM_V_PATCH 19
#define PROGRAM_V_TAG "debug alpha"

/* Constants */

#define PI 		3.1415926535897932384626433832795
#define TAU 	6.2831853071795864769252867665590
#define EULER 	2.7182818284590452353602874713527

/* Macros */

#define ROL(n,b) ((n << b) | (n >> ((sizeof(n) * 8) - b))) // Left Circular Shift
#define ROR(n,b) ((n >> b) | (n << ((sizeof(n) * 8) - b))) // Right Circular Shift

#define STR_M(x) #x
#define STR_N(x) STR_M(x)

#define ARRAY_LENGTH(x) ( (sizeof(x[0]) != 0) ? (sizeof(x) / sizeof(x[0])) : 0 )
#define TEXT_LENGTH(x) ( (sizeof(x[0]) != 0) ? ( ((sizeof(x) / sizeof(x[0])) != 0) ? ((sizeof(x) / sizeof(x[0])) - 1) : 0 ) : 0 )
#define FREE(x) free(x); x = NULL

#define printFlush(...) printf(__VA_ARGS__); fflush(stdout)
#define printFatalError(...) printf("\n============\nFATAL ERROR: "); printf(__VA_ARGS__); printf("\n============\n"); fflush(stdout)
#define printCriticalError(...) printf("\nCritical Error: "); printf(__VA_ARGS__); printf("\n"); fflush(stdout)
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

#define boolText(bool) ((bool) ? "True" : "False")

#define PROGRAM_VERSION STR_N(PROGRAM_V_MAJOR) "." STR_N(PROGRAM_V_MINOR) "." STR_N(PROGRAM_V_PATCH) " " PROGRAM_V_TAG

#define valueLimit(value,minimum,maximum) { if (value < minimum) { value = minimum; } else if (value > maximum) { value = maximum; } }
#define valueMinimum(value,minimum) { if (value < minimum) { value = minimum; } }
#define valueMaximum(value,maximum) { if (value > maximum) { value = maximum; } }

/* Functions */

fp64 calcMinMaxRatio(fp64 val, fp64 min, fp64 max, fp64 ratio);
uint32_t calcMinMaxRatio(uint32_t val, uint32_t min, uint32_t max, fp64 ratio);

#define linearInterpolation(x,x0,x1,y0,y1) ( (y0) + ( (((y1) - (y0)) * ((x) - (x0))) / ((x1) - (x0)) ) )
#define linearInterpolationLimit(x,x0,x1,y0,y1) ( ((x) <= (x0)) ? (y0) : ( ((x) >= (x1)) ? (y1) : linearInterpolation(x,x0,x1,y0,y1) ) )


/* Time */

uint64_t getNanoTime(); // Returns the time in nanoseconds
fp64 getDecimalTime(); // Returns the time in seconds

// Print up to every (freq) seconds, also calls fflush(stdout);
#define printfInterval(freq,...); \
{ \
	static uint64_t ResetTime_PrintfInterval = getNanoTime(); \
	if (getNanoTime() - ResetTime_PrintfInterval > (uint64_t)((freq) * 1.0e9)) { \
		ResetTime_PrintfInterval = getNanoTime(); \
		printf(__VA_ARGS__); \
		fflush(stdout); \
	} \
}
// Print change in value up to every (freq) seconds, also calls fflush(stdout);
#define printfChangeInterval(type,value,freq,...) \
{ \
	static uint64_t ResetTime_PrintfInterval = getNanoTime(); \
	static type Detect_Change = (value); \
	if (getNanoTime() - ResetTime_PrintfInterval > (uint64_t)((freq) * 1.0e9)) { \
		if (Detect_Change != (value)) { \
			ResetTime_PrintfInterval = getNanoTime(); \
			printf(__VA_ARGS__); \
			fflush(stdout); \
			Detect_Change = (value); \
		} \
	} \
}


#define SECONDS_TO_NANO(t) (uint64_t)((t) * 1.0e9)
#define NANO_TO_SECONDS(t) ((fp64)(t) / 1.0e9)

class TimerBox {
	public:
	/* Constructors */
		TimerBox();
		TimerBox(uint64_t freq);
		TimerBox(fp64 freq);
	/* resetTime */
		void updateTime();
		void setTime(uint64_t time);
		void setTime(fp64 time);
		uint64_t getResetTimeNano();
		fp64 getResetTime();
	/* deltaTime */
		uint64_t getDeltaTimeNano();
		fp64 getDeltaTime();
	/* freqTime */
		void setFreq(uint64_t freq);
		void setFreq(fp64 freq);
		uint64_t getResetFreqNano();
		fp64 getResetFreq();
	/* timeElapsed */
		uint64_t timeElapsedNano();
		fp64 timeElapsed();
		uint64_t timeToTimerReadyNano();
		fp64 timeToTimerReady();
	/* timerReset | Resets the Timer */
		bool timerReset();
		bool timerReset(uint64_t freq);
		bool timerReset(fp64 freq);
	/* timerReady | Does not reset the Timer */
		bool timerReady();
		bool timerReady(uint64_t freq);
		bool timerReady(fp64 freq);
	private:
		uint64_t resetTime;
		uint64_t freqTime;
		uint64_t deltaTime;
};

/* Color */

// (&R,&G,&B) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
void getRGBfromHSV(uint8_t* r, uint8_t* g, uint8_t* b, fp64 hue, fp64 sat, fp64 val);
// (&R,&G,&B) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
void getRGBfromHSV(uint8_t* r, uint8_t* g, uint8_t* b, fp32 hue, fp32 sat, fp32 val);
// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
uint32_t getRGBfromHSV(fp64 hue, fp64 sat, fp64 val);
// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
uint32_t getRGBfromHSV(fp32 hue, fp32 sat, fp32 val);

#endif /* COMMON_DEF_H */