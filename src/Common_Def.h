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

#include <cstddef>
#include <cstdint>
#include <cinttypes>
#include <cstdlib>

#include <math.h>
#include <cmath>

#include <cstring>

#include <cstdio>
#include <iostream>

#include <chrono>
#include <ctime>

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <algorithm>

#include <cstdarg>
#include <stdexcept>
#include <climits>

/* CPU Information */

#include "CPU_Information.h"

// Returns the list of Available x86 CPU Instruction Sets. Automatically Initializes
const Supported_CPU_Instruction& get_Available_CPU_Instruction();

/* Typedefs */

typedef float fp32;
typedef double fp64;

// No specifier needed
#define PRIfp32 ""
#define PRIfp64 "l"

typedef int int_enum;
typedef int64_t nano64_t;
typedef int32_t dim32_t;

/* Version */

//#define BUILD_DEBUG
#define BUILD_RELEASE

#ifdef _WIN32
	#define PLATFORM_WINDOWS
#else
	#define PLATFORM_LINUX
#endif

/* Constants */

#define PI 		3.1415926535897932384626433832795
#define TAU 	6.2831853071795864769252867665590
#define EULER 	2.7182818284590452353602874713527

/* Macros */

	// Macro to String
	#define STR_M(x) #x
	// Macro to Number
	#define STR_N(x) STR_M(x)

	#define ARRAY_LENGTH(x)  (sizeof(x) / sizeof(x[0]))
	#define TEXT_LENGTH(x) ( ((sizeof(x) / sizeof(x[0])) != 0) ? ((sizeof(x) / sizeof(x[0])) - 1) : 0 )
	#define FREE(x) do { free(x); x = nullptr; } while(0)

/* Functions */
	// Left Circular Shift
	#define ROL(n,b) (((n) << (b)) | ((n) >> ((sizeof(n) * CHAR_BIT) - (b))))
	// Right Circular Shift
	#define ROR(n,b) (((n) >> (b)) | ((n) << ((sizeof(n) * CHAR_BIT) - (b))))

	constexpr inline const char* bool_Text(const bool& b) { return b ? "true" : "false"; }
	constexpr inline const char* Bool_Text(const bool& b) { return b ? "True" : "False"; }
	constexpr inline const char* BOOL_Text(const bool& b) { return b ? "TRUE" : "FALSE"; }

	// Replace with valueClamp<cast, minimum, maximum>(value)

	#define valueClamp(value,minimum,maximum)\
	(\
		((value) < (minimum)) ?\
		((value) = (minimum)) :\
		(\
			((value) > (maximum)) ?\
			((value) = (maximum)) :\
			((value) = (value))\
		)\
	)
	
	#define valueMinimumClamp(value,minimum) (\
		((value) < (minimum)) ?\
		((value) = (minimum)) :\
		((value) = (value))\
	)
	
	#define valueMaximumClamp(value,maximum) (\
		((value) > (maximum)) ?\
		((value) = (maximum)) :\
		((value) = (value))\
	)

	#define valueRestore(value,restore,minimum,maximum) (\
		((value) >= (minimum) && (value) <= (maximum)) ?\
		((value) = (value)) :\
		((value) = (restore))\
	)
	
	void* patternMemcpy(void* __restrict__ buf, size_t bufSize, const void* __restrict__ PatternData, size_t PatternSize);

	void* inPlacePatternMemcpy(void* __restrict__ buf, size_t bufSize, size_t PatternSize);

	fp64 calcMinMaxRatio(fp64 val, fp64 min, fp64 max, fp64 ratio);
	fp32 calcMinMaxRatio(fp32 val, fp32 min, fp32 max, fp32 ratio);
	int32_t calcMinMaxRatio(int32_t val, int32_t min, int32_t max, fp64 ratio);
	uint32_t calcMinMaxRatio(uint32_t val, uint32_t min, uint32_t max, fp64 ratio);
	
	fp64 calcMinRatioMax(fp64 val, fp64 min, fp64 ratio, fp64 max);
	fp32 calcMinRatioMax(fp32 val, fp32 min, fp32 ratio, fp32 max);
	int32_t calcMinRatioMax(int32_t val, int32_t min, fp64 ratio, int32_t max);
	uint32_t calcMinRatioMax(uint32_t val, uint32_t min, fp64 ratio, uint32_t max);

	template<typename T> T linearInterpolation(const T& x, const T& x0, const T& x1, const T& y0, const T& y1) {
		return y0 + ( ((y1 - y0) * (x - x0)) / (x1 - x0) );
	}

	template<typename T> T linearInterpolationClamp(const T&  x, const T& x0, const T& x1, const T& y0, const T& y1) {
		return (
			(x <= x0) ? y0 : (
				(x >= x1) ? y1 : linearInterpolation(x,x0,x1,y0,y1)
			)
		);
	}

	constexpr inline fp64 RADIANS_TO_DEGREES(fp64 radians) { return radians * (360.0 / TAU); }
	constexpr inline fp64 DEGREES_TO_RADIANS(fp64 degrees) { return degrees * (TAU / 360.0); }

/* Time */

	// Returns the time in nanoseconds
	inline nano64_t getNanoTime() { 
		using nanoseconds = std::chrono::duration<nano64_t, std::nano>;
		auto now = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<nanoseconds>(now.time_since_epoch()).count();
	}

	constexpr inline fp64 NANO_TO_SECONDS(nano64_t t) { return (fp64)t / 1.0e9; }
	constexpr inline fp64 NANO_TO_FRAMERATE(nano64_t t) { return 1.0e9 / (fp64)t; }
	constexpr inline nano64_t SECONDS_TO_NANO(fp64 s) { return (nano64_t)(s * 1.0e9); }
	constexpr inline fp64 SECONDS_TO_FRAMERATE(fp64 s) { return 1.0 / s; }
	constexpr inline nano64_t FRAMERATE_TO_NANO(fp64 f) { return (nano64_t)(1.0e9 / f); }
	constexpr inline fp64 FRAMERATE_TO_SECONDS(fp64 f) { return 1.0 / f; }
	
	// Returns the time in seconds
	inline fp64 getDecimalTime() {
		return NANO_TO_SECONDS(getNanoTime());
	}
	
	// Sleeps for a given duration. Returns the difference between the duration and the actual time slept (negative being late)
	inline nano64_t accurateSleep(nano64_t duration) {
		constexpr nano64_t getNanoTimePrecision = 100;
		duration = (duration / getNanoTimePrecision) * getNanoTimePrecision;
		
		constexpr nano64_t sleep_threshold    = SECONDS_TO_NANO(1.0e0 );
		constexpr nano64_t yield_threshold    = SECONDS_TO_NANO(1.0e-3);
		constexpr nano64_t spinlock_threshold = getNanoTimePrecision;
		
		const nano64_t sleep_End = getNanoTime() + duration;
		nano64_t time_dif = sleep_End - getNanoTime();
		
		// Sleep
		while (time_dif > sleep_threshold) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			time_dif = sleep_End - getNanoTime();
		}
		// Yield
		while (time_dif > yield_threshold) {
			std::this_thread::yield();
			time_dif = sleep_End - getNanoTime();
		}
		// Spin-lock
		while (time_dif > spinlock_threshold) {
			time_dif = sleep_End - getNanoTime();
		}
		return time_dif;
	}
	
	// Sleeps for a given duration. Returns the difference between the duration and the actual time slept (negative being late)
	inline fp64 accurateSleep(fp64 duration) {
		return NANO_TO_SECONDS(accurateSleep(SECONDS_TO_NANO(duration)));
	}

/* String Functions */

#ifdef PLATFORM_WINDOWS
	inline uint8_t  stringTo_Uint8 (const char* nPtr, char** endPtr, int base = 10) { return (nPtr != nullptr) ? (uint8_t) strtoul (nPtr,endPtr,base) : 0; }
	inline uint16_t stringTo_Uint16(const char* nPtr, char** endPtr, int base = 10) { return (nPtr != nullptr) ? (uint16_t)strtoul (nPtr,endPtr,base) : 0; }
	inline uint32_t stringTo_Uint32(const char* nPtr, char** endPtr, int base = 10) { return (nPtr != nullptr) ? (uint32_t)strtoul (nPtr,endPtr,base) : 0; }
	inline uint64_t stringTo_Uint64(const char* nPtr, char** endPtr, int base = 10) { return (nPtr != nullptr) ? (uint64_t)strtoull(nPtr,endPtr,base) : 0; }
	inline int8_t   stringTo_Int8  (const char* nPtr, char** endPtr, int base = 10) { return (nPtr != nullptr) ? (int8_t)  strtol  (nPtr,endPtr,base) : 0; }
	inline int16_t  stringTo_Int16 (const char* nPtr, char** endPtr, int base = 10) { return (nPtr != nullptr) ? (int16_t) strtol  (nPtr,endPtr,base) : 0; }
	inline int32_t  stringTo_Int32 (const char* nPtr, char** endPtr, int base = 10) { return (nPtr != nullptr) ? (int32_t) strtol  (nPtr,endPtr,base) : 0; }
	inline int64_t  stringTo_Int64 (const char* nPtr, char** endPtr, int base = 10) { return (nPtr != nullptr) ? (int64_t) strtoll (nPtr,endPtr,base) : 0; }
	
	inline fp32 stringTo_Float32(const char* nPtr, char** endPtr = nullptr) { return (nPtr != nullptr) ? strtof(nPtr,endPtr) : 0.0f; }
	inline fp64 stringTo_Float64(const char* nPtr, char** endPtr = nullptr) { return (nPtr != nullptr) ? strtod(nPtr,endPtr) : 0.0 ; }

	inline uint8_t  stringTo_Uint8 (const char* nPtr, int base = 10) {  return (nPtr != nullptr) ? (uint8_t) strtoul (nPtr, nullptr, base) : 0; }
	inline uint16_t stringTo_Uint16(const char* nPtr, int base = 10) {  return (nPtr != nullptr) ? (uint16_t)strtoul (nPtr, nullptr, base) : 0; }
	inline uint32_t stringTo_Uint32(const char* nPtr, int base = 10) {  return (nPtr != nullptr) ? (uint32_t)strtoul (nPtr, nullptr, base) : 0; }
	inline uint64_t stringTo_Uint64(const char* nPtr, int base = 10) {  return (nPtr != nullptr) ? (uint64_t)strtoull(nPtr, nullptr, base) : 0; }
	inline int8_t   stringTo_Int8  (const char* nPtr, int base = 10) {  return (nPtr != nullptr) ? (int8_t)  strtol  (nPtr, nullptr, base) : 0; }
	inline int16_t  stringTo_Int16 (const char* nPtr, int base = 10) {  return (nPtr != nullptr) ? (int16_t) strtol  (nPtr, nullptr, base) : 0; }
	inline int32_t  stringTo_Int32 (const char* nPtr, int base = 10) {  return (nPtr != nullptr) ? (int32_t) strtol  (nPtr, nullptr, base) : 0; }
	inline int64_t  stringTo_Int64 (const char* nPtr, int base = 10) {  return (nPtr != nullptr) ? (int64_t) strtoll (nPtr, nullptr, base) : 0; }
#else
	inline uint8_t  stringTo_Uint8 (const char* nPtr, char** endPtr, int base = 10) { return (nPtr != nullptr) ? (uint8_t) strtoul(nPtr,endPtr,base) : 0; }
	inline uint16_t stringTo_Uint16(const char* nPtr, char** endPtr, int base = 10) { return (nPtr != nullptr) ? (uint16_t)strtoul(nPtr,endPtr,base) : 0; }
	inline uint32_t stringTo_Uint32(const char* nPtr, char** endPtr, int base = 10) { return (nPtr != nullptr) ? (uint32_t)strtoul(nPtr,endPtr,base) : 0; }
	inline uint64_t stringTo_Uint64(const char* nPtr, char** endPtr, int base = 10) { return (nPtr != nullptr) ? (uint64_t)strtoul(nPtr,endPtr,base) : 0; }
	inline int8_t   stringTo_Int8  (const char* nPtr, char** endPtr, int base = 10) { return (nPtr != nullptr) ? (int8_t)  strtol (nPtr,endPtr,base) : 0; }
	inline int16_t  stringTo_Int16 (const char* nPtr, char** endPtr, int base = 10) { return (nPtr != nullptr) ? (int16_t) strtol (nPtr,endPtr,base) : 0; }
	inline int32_t  stringTo_Int32 (const char* nPtr, char** endPtr, int base = 10) { return (nPtr != nullptr) ? (int32_t) strtol (nPtr,endPtr,base) : 0; }
	inline int64_t  stringTo_Int64 (const char* nPtr, char** endPtr, int base = 10) { return (nPtr != nullptr) ? (int64_t) strtol (nPtr,endPtr,base) : 0; }
	
	inline fp32 stringTo_Float32(const char* nPtr, char** endPtr = nullptr) { return (nPtr != nullptr) ? strtof32(nPtr,endPtr) : 0.0f; }
	inline fp64 stringTo_Float64(const char* nPtr, char** endPtr = nullptr) { return (nPtr != nullptr) ? strtof64(nPtr,endPtr) : 0.0 ; }

	inline uint8_t  stringTo_Uint8 (const char* nPtr, int base = 10) { return (nPtr != nullptr) ? (uint8_t) strtoul(nPtr,nullptr,base) : 0; }
	inline uint16_t stringTo_Uint16(const char* nPtr, int base = 10) { return (nPtr != nullptr) ? (uint16_t)strtoul(nPtr,nullptr,base) : 0; }
	inline uint32_t stringTo_Uint32(const char* nPtr, int base = 10) { return (nPtr != nullptr) ? (uint32_t)strtoul(nPtr,nullptr,base) : 0; }
	inline uint64_t stringTo_Uint64(const char* nPtr, int base = 10) { return (nPtr != nullptr) ? (uint64_t)strtoul(nPtr,nullptr,base) : 0; }
	inline int8_t   stringTo_Int8  (const char* nPtr, int base = 10) { return (nPtr != nullptr) ? (int8_t)  strtol (nPtr,nullptr,base) : 0; }
	inline int16_t  stringTo_Int16 (const char* nPtr, int base = 10) { return (nPtr != nullptr) ? (int16_t) strtol (nPtr,nullptr,base) : 0; }
	inline int32_t  stringTo_Int32 (const char* nPtr, int base = 10) { return (nPtr != nullptr) ? (int32_t) strtol (nPtr,nullptr,base) : 0; }
	inline int64_t  stringTo_Int64 (const char* nPtr, int base = 10) { return (nPtr != nullptr) ? (int64_t) strtol (nPtr,nullptr,base) : 0; }
#endif

/* Print Functions */
	#define printFlush(...) do { printf(__VA_ARGS__); fflush(stdout); } while(0)
	#define printFatalError(...) do { printf("\n============\nFATAL ERROR: "); printf(__VA_ARGS__); printf("\n============\n"); fflush(stdout); } while(0)
	#define printCriticalError(...) do { printf("\nCRITICAL ERROR: "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
	#define printError(...) do { printf("\nError: "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
	#define printWarning(...) do { printf("\nWarning: "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)

	// Print change in value, also calls fflush(stdout);
	#define printfChange(type, value, ...) do { \
		static type Detect_Change = (value); \
		if (Detect_Change != (value)) { \
			printf(__VA_ARGS__); \
			fflush(stdout); \
			Detect_Change = (value); \
		} \
	} while(0)

	// Print up to every (freq) seconds, also calls fflush(stdout);
	#define printfInterval(freq, ...) do { \
		static nano64_t ResetTime_PrintfInterval = getNanoTime(); \
		if (getNanoTime() - ResetTime_PrintfInterval > SECONDS_TO_NANO(freq)) { \
			ResetTime_PrintfInterval = getNanoTime(); \
			printf(__VA_ARGS__); \
			fflush(stdout); \
		} \
	} while(0)

	// Print change in value up to every (freq) seconds, also calls fflush(stdout);
	#define printfChangeInterval(type, value, freq, ...) do { \
		static nano64_t ResetTime_PrintfInterval = getNanoTime(); \
		static type Detect_Change = (value); \
		if (getNanoTime() - ResetTime_PrintfInterval > SECONDS_TO_NANO(freq)) { \
			if (Detect_Change != (value)) { \
				ResetTime_PrintfInterval = getNanoTime(); \
				printf(__VA_ARGS__); \
				fflush(stdout); \
				Detect_Change = (value); \
			} \
		} \
	} while(0)
	
	// (Debugging Tool) Waits for a duration in seconds
	inline void BurnTime(fp64 s) {
		nano64_t BURN_TIME_NANOSECONDS = getNanoTime();
		while (getNanoTime() - BURN_TIME_NANOSECONDS < SECONDS_TO_NANO(s)) {};
	}

/* Color */

	// (&R,&G,&B) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
	template <typename T, typename fpX>
	void getRGBfromHSV(T* r, T* g, T* b, fpX hue, fpX sat, fpX val) {
		uint8_t hueIndex = (uint8_t)((fpX)hue / (fpX)60.0) % 6;
		fpX f = ((fpX)hue / (fpX)60.0) - (fpX)floor((fpX)hue / (fpX)60.0);
		val *= (fpX)255.0;
		T vR = (T)val;
		T pR = (T)(val * ((fpX)1.0 - sat));
		T qR = (T)(val * ((fpX)1.0 - f * sat));
		T tR = (T)(val * ((fpX)1.0 - ((fpX)1.0 - f) * sat));
		T R = 0; T G = 0; T B = 0;
		switch(hueIndex) {
			case 0:
			R = vR; G = tR; B = pR;
			break;
			case 1:
			R = qR; G = vR; B = pR;
			break;
			case 2:
			R = pR; G = vR; B = tR;
			break;
			case 3:
			R = pR; G = qR; B = vR;
			break;
			case 4:
			R = tR; G = pR; B = vR;
			break;
			case 5:
			R = vR; G = pR; B = qR;
			break;
		}
		if (r != nullptr) { *r = R; }
		if (g != nullptr) { *g = G; }
		if (b != nullptr) { *b = B; }
	}

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
