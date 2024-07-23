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
// Moved to CMakeLists.txt
/*
**	// long double
**	#define Enable_Float80
**
**	// quadmath.h
**	#define Enable_Float128
**
**	// GPU Rendering
**	// #define Enable_OpenCL
**
**	// Uses the OpenCV scaler over the SDL2 one
**	// #define Enable_OpenCV_Scaler
**
**	// Remove Color
**	// #define MONOCHROME_MODE
*/

/* Version */

#define PROGRAM_NAME "ABS-Fractal-Explorer"
#define PROGRAM_DATE "2024/07/23" /* YYYY/MM/DD */
#define PROGRAM_V_MAJOR 1
#define PROGRAM_V_MINOR 2
#define PROGRAM_V_PATCH 1
#define PROGRAM_V_TAG "(Revision-6)"
#define PROGRAM_VERSION STR_N(PROGRAM_V_MAJOR) "." STR_N(PROGRAM_V_MINOR) "." STR_N(PROGRAM_V_PATCH) " " PROGRAM_V_TAG

/* Float80 and Float128 */

	#ifdef Enable_Float80
		#include "floats/Float80.hpp"
	#endif

	#ifdef Enable_Float128
		#include "floats/Float128.hpp"
	#endif

	#if defined(Enable_Float128)
		/* Float128 is the highest precision */
		typedef fp128 fpCord;
		#define FloatCoordinate_snprintf quadmath_snprintf
		#define PRIfpCord PRIfp128
		inline fpCord stringTo_FloatCoordinate(const char* nPtr, char** endPtr) {
			return stringTo_Float128(nPtr,endPtr);
		}
	#elif defined(Enable_Float80)
		/* Float80 is the highest precision */
		typedef fp80 fpCord;
		#define FloatCoordinate_snprintf snprintf
		#define PRIfpCord PRIfp80
		inline fpCord stringTo_FloatCoordinate(const char* nPtr, char** endPtr) {
			return stringTo_Float80(nPtr, endPtr);
		}
	#else
		/* Float64 is the highest precision */
		typedef fp64 fpCord;
		#define FloatCoordinate_snprintf snprintf
		#define PRIfpCord PRIfp64
		inline fpCord stringTo_FloatCoordinate(const char* nPtr, char** endPtr) {
			return stringTo_Float64(nPtr, endPtr);
		}
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