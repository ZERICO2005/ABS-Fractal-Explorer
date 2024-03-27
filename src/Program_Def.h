/*
**	Author: zerico2005 (2023-2024)
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

//#define MONOCHROME_MODE

#define PROGRAM_NAME "ABS-Fractal-Explorer"
#define PROGRAM_DATE "2024/03/27" /* YYYY/MM/DD */
#define PROGRAM_V_MAJOR 1
#define PROGRAM_V_MINOR 1
#define PROGRAM_V_PATCH 8
#define PROGRAM_V_TAG "Rev-2 Developer Alpha"
#define PROGRAM_VERSION STR_N(PROGRAM_V_MAJOR) "." STR_N(PROGRAM_V_MINOR) "." STR_N(PROGRAM_V_PATCH) " " PROGRAM_V_TAG

/* float80 and float128 */

#define enableFP80andFP128

#ifdef enableFP80andFP128
		#include <quadmath.h>
		typedef __float128 fp128;
		typedef __int128_t int128_t;
		typedef __uint128_t uint128_t;
		typedef __int128_t i128;
		typedef __uint128_t u128;
		typedef __float80 fp80;
		inline fp128 sin(fp128 x) { return sinq(x); }
		inline fp128 cos(fp128 x) { return cosq(x); }
		inline fp128 tan(fp128 x) { return tanq(x); }
		inline fp128 asin(fp128 x) { return asinq(x); }
		inline fp128 acos(fp128 x) { return acosq(x); }
		inline fp128 atan(fp128 x) { return atanq(x); }
		inline fp128 atan2(fp128 y, fp128 x) { return atan2q(y,x); }
		inline fp128 log(fp128 x) { return logq(x); }
		inline fp128 log1p(fp128 x) { return log1pq(x); }
		inline fp128 log2(fp128 x) { return log2q(x); }
		inline fp128 log10(fp128 x) { return log10q(x); }
		inline fp128 pow(fp128 x, fp128 y) { return powq(x,y); }
		inline fp128 floor(fp128 x) { return floorq(x); }
		inline fp128 ceil(fp128 x) { return ceilq(x); }
		inline fp128 fmax(fp128 x, fp128 y) { return fmaxq(x,y); }
		inline fp128 fmin(fp128 x, fp128 y) { return fminq(x,y); }
		inline fp128 fmod(fp128 x, fp128 y) { return fmodq(x,y); }
		inline fp128 fabs(fp128 x) { return fabsq(x); }
		inline fp128 copysign(fp128 x, fp128 y) { return copysignq(x,y); }
#else
	#define quadmath_snprintf snprintf
	#define strtoflt128 strtod
	typedef long double fp128;
	typedef long double fp80;
	typedef int64_t int128_t;
	typedef uint64_t uint128_t;
	typedef int64_t i128;
	typedef uint64_t u128;
#endif

constexpr size_t IMAGE_BUFFER_CHANNELS = 4;
/* Use to correct any magic numbers
(?<!\d\.)\b3\b
*/

class TimerBox {
	public:
	/* Constructors */
		TimerBox();
		TimerBox(nano64_t freq);
		TimerBox(fp64 freq);
	/* resetTime */
		void updateTime();
		void setTime(nano64_t time);
		void setTime(fp64 time);
		nano64_t getResetTimeNano();
		fp64 getResetTime();
	/* deltaTime */
		nano64_t getDeltaTimeNano();
		fp64 getDeltaTime();
	/* freqTime */
		void setFreq(nano64_t freq);
		void setFreq(fp64 freq);
		nano64_t getResetFreqNano();
		fp64 getResetFreq();
	/* timeElapsed */
		nano64_t timeElapsedNano();
		fp64 timeElapsed();
		nano64_t timeToTimerReadyNano();
		fp64 timeToTimerReady();
	/* timerReset | Resets the Timer */
		bool timerReset();
		bool timerReset(nano64_t freq);
		bool timerReset(fp64 freq);
	/* timerReady | Does not reset the Timer */
		bool timerReady();
		bool timerReady(nano64_t freq);
		bool timerReady(fp64 freq);
	private:
		nano64_t resetTime;
		nano64_t freqTime;
		nano64_t deltaTime;
};

// Disable this for release builds
//#define displayTerribleProgrammingJokes

#endif /* PROGRAM_DEF_H */