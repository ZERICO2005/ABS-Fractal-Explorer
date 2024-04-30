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
#include <cmath>
#include <cstring>

#include <cstdio>
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
#include <stdexcept>
#include <climits>

/* CPU Information */

#include "CPU_Information.h"

// Returns the list of Available x86 CPU Instruction Sets. Automatically Initializes
const Supported_CPU_Instruction& get_Available_CPU_Instruction();

/* Typedefs */

typedef float fp32;
typedef double fp64;

#define PRIfp32 "f"
#define PRIfp64 "lf"

typedef int int_enum;
typedef int64_t nano64_t;
typedef int32_t dim32_t;

/* Version */

//#define BUILD_DEBUG
#define BUILD_RELEASE

#define PLATFORM_WINDOWS
// #define PLATFORM_LINUX

/* Constants */

#define PI 		3.1415926535897932384626433832795
#define TAU 	6.2831853071795864769252867665590
#define EULER 	2.7182818284590452353602874713527

/* Macros */

	// Macro to String
	#define STR_M(x) #x
	// Macro to Number
	#define STR_N(x) STR_M(x)

	#define ARRAY_LENGTH(x) ( (sizeof(x[0]) != 0) ? (sizeof(x) / sizeof(x[0])) : 0 )
	#define TEXT_LENGTH(x) ( (sizeof(x[0]) != 0) ? ( ((sizeof(x) / sizeof(x[0])) != 0) ? ((sizeof(x) / sizeof(x[0])) - 1) : 0 ) : 0 )
	#define FREE(x) free(x); x = nullptr

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

	// NOT A CRYPTOGRAPHIC HASH FUNCTION (https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function)
	uint64_t fnv1a_hash(const uint8_t* buf, size_t len);
	// NOT A CRYPTOGRAPHIC HASH FUNCTION (https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function)
	// Set hash to 0x0 to start. Allows multiple arrays to be used in the hash
	void fnv1a_hash_continous(uint64_t& hash, const uint8_t* buf, size_t len);

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

/* Time */
	// Returns the time in nanoseconds
	nano64_t getNanoTime();
	// Returns the time in seconds 
	fp64 getDecimalTime();

	constexpr inline fp64 NANO_TO_SECONDS(nano64_t t) { return (fp64)t / 1.0e9; }
	constexpr inline fp64 NANO_TO_FRAMERATE(nano64_t t) { return (1.0e9) / (fp64)t; }
	constexpr inline nano64_t SECONDS_TO_NANO(fp64 s) { return (nano64_t)(s * 1.0e9); }
	constexpr inline fp64 SECONDS_TO_FRAMERATE(fp64 s) { return 1.0 / s; }
	constexpr inline nano64_t FRAMERATE_TO_NANO(fp64 f) { return (nano64_t)(1.0e9 / f); }
	constexpr inline fp64 FRAMERATE_TO_SECONDS(fp64 f) { return 1.0 / f; }
	
/* String Functions */

#ifdef PLATFORM_WINDOWS
	inline uint8_t  stringTo_Uint8 (const char* nPtr, char** endPtr, int base) { return (nPtr != nullptr) ? (uint8_t) strtoul (nPtr,endPtr,base) : 0; }
	inline uint16_t stringTo_Uint16(const char* nPtr, char** endPtr, int base) { return (nPtr != nullptr) ? (uint16_t)strtoul (nPtr,endPtr,base) : 0; }
	inline uint32_t stringTo_Uint32(const char* nPtr, char** endPtr, int base) { return (nPtr != nullptr) ? (uint32_t)strtoul (nPtr,endPtr,base) : 0; }
	inline uint64_t stringTo_Uint64(const char* nPtr, char** endPtr, int base) { return (nPtr != nullptr) ? (uint64_t)strtoull(nPtr,endPtr,base) : 0; }
	inline int8_t   stringTo_Int8  (const char* nPtr, char** endPtr, int base) { return (nPtr != nullptr) ? (int8_t)  strtol  (nPtr,endPtr,base) : 0; }
	inline int16_t  stringTo_Int16 (const char* nPtr, char** endPtr, int base) { return (nPtr != nullptr) ? (int16_t) strtol  (nPtr,endPtr,base) : 0; }
	inline int32_t  stringTo_Int32 (const char* nPtr, char** endPtr, int base) { return (nPtr != nullptr) ? (int32_t) strtol  (nPtr,endPtr,base) : 0; }
	inline int64_t  stringTo_Int64 (const char* nPtr, char** endPtr, int base) { return (nPtr != nullptr) ? (int64_t) strtoll (nPtr,endPtr,base) : 0; }
	inline fp32 stringTo_Float32(const char* nPtr, char** endPtr) { return strtof(nPtr,endPtr); }
	inline fp64 stringTo_Float64(const char* nPtr, char** endPtr) { return strtod(nPtr,endPtr); }

	inline uint8_t  stringTo_Uint8 (const char* nPtr) { return (nPtr != nullptr) ? (uint8_t) atoi (nPtr) : 0; }
	inline uint16_t stringTo_Uint16(const char* nPtr) { return (nPtr != nullptr) ? (uint16_t)atoi (nPtr) : 0; }
	inline uint32_t stringTo_Uint32(const char* nPtr) { return (nPtr != nullptr) ? (uint32_t)atoll(nPtr) : 0; }
	inline uint64_t stringTo_Uint64(const char* nPtr) { return (nPtr != nullptr) ? (uint64_t)atoll(nPtr) : 0; }
	inline int8_t   stringTo_Int8  (const char* nPtr) { return (nPtr != nullptr) ? (int8_t)  atoi (nPtr) : 0; }
	inline int16_t  stringTo_Int16 (const char* nPtr) { return (nPtr != nullptr) ? (int16_t) atoi (nPtr) : 0; }
	inline int32_t  stringTo_Int32 (const char* nPtr) { return (nPtr != nullptr) ? (int32_t) atoi (nPtr) : 0; }
	inline int64_t  stringTo_Int64 (const char* nPtr) { return (nPtr != nullptr) ? (int64_t) atoll(nPtr) : 0; }
	inline fp32 stringTo_Float32   (const char* nPtr) { return (nPtr != nullptr) ? (fp32)atof(nPtr) : 0.0f; }
	inline fp64 stringTo_Float64   (const char* nPtr) { return (nPtr != nullptr) ? (fp64)atof(nPtr) : 0.0 ; }
#else
	inline uint8_t  stringTo_Uint8 (const char* nPtr, char** endPtr, int base) { return (nPtr != nullptr) ? (uint8_t) strtoul(nPtr,endPtr,base) : 0; }
	inline uint16_t stringTo_Uint16(const char* nPtr, char** endPtr, int base) { return (nPtr != nullptr) ? (uint16_t)strtoul(nPtr,endPtr,base) : 0; }
	inline uint32_t stringTo_Uint32(const char* nPtr, char** endPtr, int base) { return (nPtr != nullptr) ? (uint32_t)strtoul(nPtr,endPtr,base) : 0; }
	inline uint64_t stringTo_Uint64(const char* nPtr, char** endPtr, int base) { return (nPtr != nullptr) ? (uint64_t)strtoul(nPtr,endPtr,base) : 0; }
	inline int8_t   stringTo_Int8  (const char* nPtr, char** endPtr, int base) { return (nPtr != nullptr) ? (int8_t)  strtol (nPtr,endPtr,base) : 0; }
	inline int16_t  stringTo_Int16 (const char* nPtr, char** endPtr, int base) { return (nPtr != nullptr) ? (int16_t) strtol (nPtr,endPtr,base) : 0; }
	inline int32_t  stringTo_Int32 (const char* nPtr, char** endPtr, int base) { return (nPtr != nullptr) ? (int32_t) strtol (nPtr,endPtr,base) : 0; }
	inline int64_t  stringTo_Int64 (const char* nPtr, char** endPtr, int base) { return (nPtr != nullptr) ? (int64_t) strtol (nPtr,endPtr,base) : 0; }
	inline fp32 stringTo_Float32(const char* nPtr, char** endPtr) { return (nPtr != nullptr) ? (fp32)strtof32(nPtr,endPtr) : 0.0f; }
	inline fp64 stringTo_Float64(const char* nPtr, char** endPtr) { return (nPtr != nullptr) ? (fp64)strtof64(nPtr,endPtr) : 0.0 ; }

	inline uint8_t  stringTo_Uint8 (const char* nPtr) { return (nPtr != nullptr) ? (uint8_t )atoi(nPtr) : 0; }
	inline uint16_t stringTo_Uint16(const char* nPtr) { return (nPtr != nullptr) ? (uint16_t)atoi(nPtr) : 0; }
	inline uint32_t stringTo_Uint32(const char* nPtr) { return (nPtr != nullptr) ? (uint32_t)atol(nPtr) : 0; }
	inline uint64_t stringTo_Uint64(const char* nPtr) { return (nPtr != nullptr) ? (uint64_t)atol(nPtr) : 0; }
	inline int8_t   stringTo_Int8  (const char* nPtr) { return (nPtr != nullptr) ? (int8_t  )atoi(nPtr) : 0; }
	inline int32_t  stringTo_Int32 (const char* nPtr) { return (nPtr != nullptr) ? (int32_t )atoi(nPtr) : 0; }
	inline int16_t  stringTo_Int16 (const char* nPtr) { return (nPtr != nullptr) ? (int16_t )atoi(nPtr) : 0; }
	inline int64_t  stringTo_Int64 (const char* nPtr) { return (nPtr != nullptr) ? (int64_t )atol(nPtr) : 0; }
	inline fp32 stringTo_Float32(const char* nPtr) { return (nPtr != nullptr) ? (fp32)atof(nPtr) : 0.0f; }
	inline fp64 stringTo_Float64(const char* nPtr) { return (nPtr != nullptr) ? (fp64)atof(nPtr) : 0.0 ; }
#endif

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
