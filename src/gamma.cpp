/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "gamma.h"

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