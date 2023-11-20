/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef PROGRAM_DEF_H
#define PROGRAM_DEF_H

#include "Common_Def.h"

/*
**	Program_Def.h contains commonly used functions, includes, defines, macros and constants used specifically for this program
*/

/* Version */

#define PROGRAM_NAME "ABS-Fractal-Explorer"
#define PROGRAM_DATE "2023/11/19" /* YYYY/MM/DD */
#define PROGRAM_V_MAJOR 1
#define PROGRAM_V_MINOR 0
#define PROGRAM_V_PATCH 29
#define PROGRAM_V_TAG "debug alpha mostly-stable"
#define PROGRAM_VERSION STR_N(PROGRAM_V_MAJOR) "." STR_N(PROGRAM_V_MINOR) "." STR_N(PROGRAM_V_PATCH) " " PROGRAM_V_TAG

/* float80 and float128 */

#define enableFP80andFP128

#ifdef enableFP80andFP128
	#include <quadmath.h>
	typedef __float128 fp128;

	#ifdef __GNUC__
		typedef __int128_t int128_t;
		typedef __uint128_t uint128_t;
		typedef __int128_t i128;
		typedef __uint128_t u128;
		typedef __float80 fp80;
	#else
		typedef int64_t int128_t;
		typedef uint64_t uint128_t;
		typedef int64_t i128;
		typedef uint64_t u128;
		typedef __float128 fp80;
	#endif

	fp128 sin(fp128 x);
	fp128 cos(fp128 x);
	fp128 tan(fp128 x);
	fp128 asin(fp128 x);
	fp128 acos(fp128 x);
	fp128 atan(fp128 x);
	fp128 atan2(fp128 y, fp128 x);
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
#else
	#define quadmath_snprintf snprintf
	#define strtoflt128 strtod
	typedef fp64 fp128;
	typedef fp64 fp80;
	typedef int64_t int128_t;
	typedef uint64_t uint128_t;
	typedef int64_t i128;
	typedef uint64_t u128;
#endif

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

// Disable this for release builds
//#define displayTerribleProgrammingJokes

#endif /* PROGRAM_DEF_H */