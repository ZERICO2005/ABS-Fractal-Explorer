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

#include <cstdarg>
#include <stdarg.h>
#include <stdexcept>

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

/* Version */

#define PROGRAM_NAME "ABS-Fractal-Explorer"
#define PROGRAM_DATE "2023/10/27" /* YYYY/MM/DD */
#define PROGRAM_V_MAJOR 1
#define PROGRAM_V_MINOR 0
#define PROGRAM_V_PATCH 10
#define PROGRAM_V_TAG "debug pre-alpha"

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
#define printError(...) printf("\nError: "); printf(__VA_ARGS__); printf("\n"); fflush(stdout)

#define PROGRAM_VERSION STR_N(PROGRAM_V_MAJOR) "." STR_N(PROGRAM_V_MINOR) "." STR_N(PROGRAM_V_PATCH) " " PROGRAM_V_TAG

#define valueLimit(value,minimum,maximum) { if (value < minimum) { value = minimum; } else if (value > maximum) { value = maximum; } }
#define valueMinimum(value,minimum) { if (value < minimum) { value = minimum; } }
#define valueMaximum(value,maximum) { if (value > maximum) { value = maximum; } }

/* Time */

uint64_t getNanoTime(); // Returns the time in nanoseconds
fp64 getDecimalTime(); // Returns the time in seconds

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

#endif /* COMMON_DEF_H */