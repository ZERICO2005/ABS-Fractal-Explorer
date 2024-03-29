/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include <string.h>
#include <stdlib.h>
/* Functions */
	// Fills a buffer with a repeating pattern of N bytes
	void* patternMemcpy(void* __restrict__ buf, size_t bufSize, const void* __restrict__ PatternData, size_t PatternSize) {
		if (buf == nullptr || PatternData == nullptr || PatternSize == 0) { return nullptr; }
		if (bufSize == 0) { return buf; } // 0 Bytes to copy
		if (PatternSize == 1) {
			memset(buf,((uint8_t*)PatternData)[0],bufSize);
			return buf;
		}
		if (bufSize <= PatternSize) {
			memcpy(buf,PatternData,bufSize);
			return buf;
		}
		memcpy(buf,PatternData,PatternSize); // Initial Copy
		size_t len = PatternSize;
		size_t pos = PatternSize;
		
		while (pos + len <= bufSize) {
			memcpy((uint8_t*)buf + pos,buf,len); 
			pos += len;
			len *= 2; // Doubles copy size each iteration
		}
		memcpy((uint8_t*)buf + pos,buf,bufSize - len); // Copies the remaining portion
		return buf;
	}

	// Assumes the pattern is set in the first N bytes in buf
	void* inPlacePatternMemcpy(void* __restrict__ buf, size_t bufSize, size_t PatternSize) {
		if (buf == nullptr || PatternSize == 0) { return nullptr; }
		if (bufSize <= PatternSize) { return buf; }
		if (PatternSize == 1) {
			memset(buf,((uint8_t*)buf)[0],bufSize);
			return buf;
		}
		size_t len = PatternSize;
		size_t pos = PatternSize;
		while (pos + len <= bufSize) {
			memcpy((uint8_t*)buf + pos,buf,len); 
			pos += len;
			len *= 2; // Doubles copy size each iteration
		}
		memcpy((uint8_t*)buf + pos,buf,bufSize - len); // Copies the remaining portion
		return 0;
	}

// NOT A CRYPTOGRAPHIC HASH FUNCTION (https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function)
uint64_t fnv1a_hash(const uint8_t* buf, size_t len) {
	if (buf == nullptr) { return 0; }
	constexpr uint64_t fnv1a_Prime = 0x100000001B3; // FNV prime (64bit)
	uint64_t fnv1a_Hash = 0xCBF29CE484222325; // FNV offset basis (64bit)
	for (size_t i = 0; i < len; i++) {
		fnv1a_Hash ^= buf[i];
		fnv1a_Hash *= fnv1a_Prime;
	}
	return fnv1a_Hash;
}

// NOT A CRYPTOGRAPHIC HASH FUNCTION (https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function)
// Set hash to 0x0 to start. Allows multiple arrays to be used in the hash
void fnv1a_hash_continous(uint64_t& hash, const uint8_t* buf, size_t len) {
	if (buf == nullptr) { return; }
	constexpr uint64_t fnv1a_Prime = 0x100000001B3; // FNV prime (64bit)
	hash = (hash == 0x0) ? 0xCBF29CE484222325 : hash; // FNV offset basis (64bit)
	for (size_t i = 0; i < len; i++) {
		hash ^= buf[i];
		hash *= fnv1a_Prime;
	}
}

fp64 calcMinMaxRatio(fp64 val, fp64 min, fp64 max, fp64 ratio) {
	if (val < min) {
		val = min;
		return val;
	}
	if (val <= max) {
		return val;
	}
	val *= ratio;
	if (val < max) {
		val = max;
	}
	return val;
}

fp32 calcMinMaxRatio(fp32 val, fp32 min, fp32 max, fp32 ratio) {
	if (val < min) {
		val = min;
		return val;
	}
	if (val <= max) {
		return val;
	}
	val *= ratio;
	if (val < max) {
		val = max;
	}
	return val;
}

int32_t calcMinMaxRatio(int32_t val, int32_t min, int32_t max, fp64 ratio) {
	if (val < min) {
		val = min;
		return val;
	}
	if (val <= max) {
		return val;
	}
	val = (uint32_t)((fp64)val * ratio);
	if (val < max) {
		val = max;
	}
	return val;
}

uint32_t calcMinMaxRatio(uint32_t val, uint32_t min, uint32_t max, fp64 ratio) {
	if (val < min) {
		val = min;
		return val;
	}
	if (val <= max) {
		return val;
	}
	val = (uint32_t)((fp64)val * ratio);
	if (val < max) {
		val = max;
	}
	return val;
}

fp64 calcMinRatioMax(fp64 val, fp64 min, fp64 ratio, fp64 max) {
	val *= ratio;
	if (val < min) {
		return min;
	}
	if (val > max) {
		return max;
	}
	return val;
}

fp32 calcMinRatioMax(fp32 val, fp32 min, fp32 ratio, fp32 max) {
	val *= ratio;
	if (val < min) {
		return min;
	}
	if (val > max) {
		return max;
	}
	return val;
}

int32_t calcMinRatioMax(int32_t val, int32_t min, fp64 ratio, int32_t max) {
	val = (int32_t)((fp64)val * ratio);
	if (val < min) {
		return min;
	}
	if (val > max) {
		return max;
	}
	return val;
}

uint32_t calcMinRatioMax(uint32_t val, uint32_t min, fp64 ratio, uint32_t max) {
	val = (uint32_t)((fp64)val * ratio);
	if (val < min) {
		return min;
	}
	if (val > max) {
		return max;
	}
	return val;
}

/* Time */

	nano64_t getNanoTime() { // Returns the time in nanoseconds
		using nanoseconds = std::chrono::duration<nano64_t, std::nano>;
		auto now = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<nanoseconds>(now.time_since_epoch()).count();
	}

	fp64 getDecimalTime() { // Returns the time in seconds
		return NANO_TO_SECONDS(getNanoTime());
	}

// const char* getDateAndTimeUTC(
// 	char DateDelimiter, char DateSeparator, char TimeDelimiter
// ) {
// 	static char timeFormat[sizeof("%Y-%m-%d_%H-%M-%S")];
// 	memset(timeFormat,'\0',sizeof(timeFormat));

// 	char* timeText = nullptr;
// 	size_t timeTextLen = sizeof("YYYY-mm-dd_HH-MM-SS");
// 	timeText = (char*)calloc(timeTextLen,sizeof(char));
// 	if (timeText == nullptr) {
// 		return nullptr;
// 	}
	
// 	snprintf(timeFormat,sizeof(timeFormat),"%%Y%c%%m%c%%d%c%%H%c%%M%c%%S",
// 		DateDelimiter,DateDelimiter,DateSeparator,TimeDelimiter,TimeDelimiter
// 	);
// 	time_t currentTime;
// 	struct tm *localTime;
// 	time(&currentTime);
// 	localTime = localtime(&currentTime);
// 	strftime(timeText, timeTextLen, timeFormat, localTime);
// 	return timeText;
// }

/* Color 24bit */

// (&R,&G,&B) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
void getRGBfromHSV(uint8_t* r, uint8_t* g, uint8_t* b, fp64 hue, fp64 sat, fp64 val) { 
	uint8_t hi = (uint8_t)(hue / 60.0) % 6;
	fp64 f = (hue / 60.0) - floor(hue / 60.0);
	val *= 255.0;
	uint8_t vR = (uint8_t)val;
	uint8_t pR = (uint8_t)(val * (1.0 - sat));
	uint8_t qR = (uint8_t)(val * (1.0 - f * sat));
	uint8_t tR = (uint8_t)(val * (1.0 - (1.0 - f) * sat));
	uint8_t R = 0; uint8_t G = 0; uint8_t B = 0;
	switch(hi) {
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

// (&R,&G,&B) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
void getRGBfromHSV(uint8_t* r, uint8_t* g, uint8_t* b, fp32 hue, fp32 sat, fp32 val) {
	uint8_t hi = (uint8_t)(hue / 60.0) % 6;
	fp32 f = (hue / 60.0) - floor(hue / 60.0);
	val *= 255.0;
	uint8_t vR = (uint8_t)val;
	uint8_t pR = (uint8_t)(val * (1.0 - sat));
	uint8_t qR = (uint8_t)(val * (1.0 - f * sat));
	uint8_t tR = (uint8_t)(val * (1.0 - (1.0 - f) * sat));
	uint8_t R = 0; uint8_t G = 0; uint8_t B = 0;
	switch(hi) {
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
uint32_t getRGBfromHSV(fp64 hue, fp64 sat, fp64 val) {
	uint8_t r,g,b;
	getRGBfromHSV(&r,&g,&b,hue,sat,val);
	return (r << 16) + (g << 8) + b;
}

// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0
uint32_t getRGBfromHSV(fp32 hue, fp32 sat, fp32 val) {
	uint8_t r,g,b;
	getRGBfromHSV(&r,&g,&b,hue,sat,val);
	return (r << 16) + (g << 8) + b;
}

/* Color 32bit */

// (&R,&G,&B,&A) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0, A 0.0-1.0
void getRGBAfromHSVA(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a, fp64 hue, fp64 sat, fp64 val, fp64 alpha) {
	if (a != nullptr) {
		*a = (uint8_t)(valueLimit(alpha,0.0,1.0) * 255.0);
	}
	getRGBfromHSV(r,g,b,hue,sat,val);
}

// (&R,&G,&B,&A) H 0.0-360.0, S 0.0-1.0, V 0.0-1.0, A 0.0-1.0
void getRGBAfromHSVA(uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a, fp32 hue, fp32 sat, fp32 val, fp32 alpha) {
	if (a != nullptr) {
		*a = (uint8_t)(valueLimit(alpha,0.0,1.0) * 255.0);
	}
	getRGBfromHSV(r,g,b,hue,sat,val);
}

// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0, A 0.0-1.0
uint32_t getRGBAfromHSVA(fp64 hue, fp64 sat, fp64 val, fp64 alpha) {
	uint8_t r,g,b,a;
	getRGBAfromHSVA(&r,&g,&b,&a,hue,sat,val,alpha);
	return (r << 16) + (g << 8) + b;
}

// H 0.0-360.0, S 0.0-1.0, V 0.0-1.0, A 0.0-1.0
uint32_t getRGBAfromHSVA(fp32 hue, fp32 sat, fp32 val, fp32 alpha) {
	uint8_t r,g,b,a;
	getRGBAfromHSVA(&r,&g,&b,&a,hue,sat,val,alpha);
	return (r << 16) + (g << 8) + b;
}