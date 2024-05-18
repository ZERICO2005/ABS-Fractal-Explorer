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

/* Toggable Features */

	// long double
	#define Enable_Float80

	// quadmath.h
	#define Enable_Float128

	// GPU Rendering
	#define Enable_OpenCL

	// Uses the OpenCV scaler over the SDL2 one
	// #define Enable_OpenCV_Scaler

	// Remove Color
	// #define MONOCHROME_MODE

/* Version */

#define PROGRAM_NAME "ABS-Fractal-Explorer"
#define PROGRAM_DATE "2024/05/18" /* YYYY/MM/DD */
#define PROGRAM_V_MAJOR 1
#define PROGRAM_V_MINOR 2
#define PROGRAM_V_PATCH 0
#define PROGRAM_V_TAG "debug developer rev-5.1"
#define PROGRAM_VERSION STR_N(PROGRAM_V_MAJOR) "." STR_N(PROGRAM_V_MINOR) "." STR_N(PROGRAM_V_PATCH) " " PROGRAM_V_TAG

/* Float80 and Float128 */

	#ifdef Enable_Float80
		typedef long double fp80;
		#define PRIfp80 "Lf"
	#endif

	#ifdef Enable_Float128
		#include <quadmath.h>
		typedef __float128 fp128;
		#define PRIfp128 "Qf"
		/* Math overloads */
			/* Trigonometry */
			inline fp128  sin (fp128 x) { return  sinq (x); }
			inline fp128  cos (fp128 x) { return  cosq (x); }
			inline fp128  tan (fp128 x) { return  tanq (x); }
			inline fp128 asin (fp128 x) { return asinq (x); }
			inline fp128 acos (fp128 x) { return acosq (x); }
			inline fp128 atan (fp128 x) { return atanq (x); }
			inline fp128  sinh(fp128 x) { return  sinhq(x); }
			inline fp128  cosh(fp128 x) { return  coshq(x); }
			inline fp128  tanh(fp128 x) { return  tanhq(x); }
			inline fp128 asinh(fp128 x) { return asinhq(x); }
			inline fp128 acosh(fp128 x) { return acoshq(x); }
			inline fp128 atanh(fp128 x) { return atanhq(x); }
			inline fp128 atan2(fp128 y, fp128 x) { return atan2q(y, x); }
			inline fp128 hypot(fp128 x, fp128 y) { return hypotq(x, y); }
			/* Logarithms and Exponents */
			inline fp128 log  (fp128 x) { return logq  (x); }
			inline fp128 log1p(fp128 x) { return log1pq(x); }
			inline fp128 log2 (fp128 x) { return log2q (x); }
			inline fp128 log10(fp128 x) { return log10q(x); }
			inline fp128 exp  (fp128 x) { return expq  (x); }
			inline fp128 expm1(fp128 x) { return expm1q(x); }
			inline fp128 exp2 (fp128 x) { return exp2q (x); }
			inline fp128 pow(fp128 x, fp128 y) { return powq(x, y); }
			/* Rounding */
			inline fp128 trunc(fp128 x) { return truncq(x); }
			inline fp128 floor(fp128 x) { return floorq(x); }
			inline fp128 ceil (fp128 x) { return ceilq (x); }
			inline fp128 round(fp128 x) { return roundq(x); }
			/* Others */
			inline fp128 fmax(fp128 x, fp128 y) { return fmaxq(x, y); }
			inline fp128 fmin(fp128 x, fp128 y) { return fminq(x, y); }
			inline fp128 fmod(fp128 x, fp128 y) { return fmodq(x, y); }
			inline fp128 fabs(fp128 x) { return fabsq(x); }
			inline fp128 copysign(fp128 x, fp128 y) { return copysignq(x, y); }
			inline bool signbit(fp128 x) { return (signbitq(x) != 0) ? true : false; }
			inline fp128 sqrt(fp128 x) { return sqrtq(x); }
			inline fp128 cbrt(fp128 x) { return cbrtq(x); }
	#endif

	#ifdef Enable_Float128
		/* Float128 is the highest precision */
		typedef fp128 fpCord;
		#define FloatCoordinate_snprintf quadmath_snprintf
		#define PRIfpCord PRIfp128
		/* Platform Specific */
			#ifdef PLATFORM_WINDOWS
				inline fpCord stringTo_FloatCoordinate(const char* nPtr, char** endPtr) { return strtoflt128(nPtr,endPtr); }
			#else
				inline fpCord stringTo_FloatCoordinate(const char* nPtr, char** endPtr) { return strtof128  (nPtr, endPtr); }
			#endif
	#else
		#ifdef Enable_Float80
			/* Float80 is the highest precision */
			typedef fp80 fpCord;
			#define FloatCoordinate_snprintf snprintf
			#define PRIfpCord PRIfp80
			inline fpCord stringTo_FloatCoordinate(const char* nPtr, char** endPtr) { return (fpCord)strtold(nPtr, endPtr); }
		#else
			/* Float64 is the highest precision */
			typedef fp64 fpCord;
			#define FloatCoordinate_snprintf snprintf
			#define PRIfpCord PRIfp64
			inline fpCord stringTo_FloatCoordinate(const char* nPtr, char** endPtr) { return (fpCord)strtold(nPtr, endPtr); }
		#endif
	#endif

	#ifdef Enable_Float128
		#ifdef Enable_Float80
			/* Both Float80 and Float128 */
			
		#else
			/* Only Float128 */
		#endif
	#else
		#ifdef Enable_Float80
			/* Only Float80 */
		#else
			/* Neither Float80 or Float128 */
		#endif
	#endif

/* Image Buffers and Framerate */

	constexpr size_t IMAGE_BUFFER_CHANNELS = 4;

	constexpr fp64 FRAMERATE_MINIMUM = 12.0;
	constexpr fp64 FRAMERATE_MAXIMUM = 1200.0;
	constexpr nano64_t FRAMETIME_MINIMUM = FRAMERATE_TO_NANO(FRAMERATE_MAXIMUM);
	constexpr nano64_t FRAMETIME_MAXIMUM = FRAMERATE_TO_NANO(FRAMERATE_MINIMUM);

	constexpr inline fp64 CALC_FRAMERATE_OFFSET(fp64 frameRate) {
		frameRate += 0.01;
		valueClamp(frameRate, FRAMERATE_MINIMUM, FRAMERATE_MAXIMUM);
		return frameRate;
	}

/* Relative File Path */
	std::string get_RelativeFilePath();
	void set_RelativeFilePath(const std::string& path);

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
		nano64_t getResetTimeNano() const;
		fp64 getResetTime() const;
	/* deltaTime */
		nano64_t getDeltaTimeNano() const;
		fp64 getDeltaTime() const;
	/* freqTime */
		void setFreq(nano64_t freq);
		void setFreq(fp64 freq);
		nano64_t getResetFreqNano() const;
		fp64 getResetFreq() const;
	/* timeElapsed */
		nano64_t timeElapsedNano() const;
		fp64 timeElapsed() const;
		nano64_t timeToTimerReadyNano() const;
		fp64 timeToTimerReady() const;
	/* timerReset | Resets the Timer */
		bool timerReset();
		bool timerReset(nano64_t freq);
		bool timerReset(fp64 freq);
	/* timerReady | Does not reset the Timer */
		bool timerReady() const;
		bool timerReady(nano64_t freq) const;
		bool timerReady(fp64 freq) const;
	private:
		nano64_t resetTime;
		nano64_t freqTime;
		nano64_t deltaTime;
};

// Disable this for release builds
//#define displayTerribleProgrammingJokes

#endif /* PROGRAM_DEF_H */