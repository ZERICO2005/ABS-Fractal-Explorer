/*
**	Author: zerico2005 (2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef DISPLAY_INFO_H
#define DISPLAY_INFO_H

#include "Common_Def.h"

class DisplayInfo {
	public:
		DisplayInfo(
			int32_t resX, int32_t resY,
			int32_t posX, int32_t posY,
			fp64 refreshRate, uint8_t bitsPerPixel,
			std::string name
		);
		void updateDisplayInfo(
			int32_t resX, int32_t resY,
			int32_t posX, int32_t posY,
			fp64 refreshRate, uint8_t bitsPerPixel,
			std::string name
		);
		void getPosition(int32_t& x, int32_t& y) const;
		int64_t getSquaredDistanceFromPoint(int32_t x, int32_t y) const;
		void getResolution(int32_t& x, int32_t& y) const;
		size_t getPixelCount() const;
		fp64 getHorizontalAspectRatio() const;
		fp64 getVerticalAspectRatio() const;
		fp64 getRefreshRate() const;
		std::string getName() const;
	
		int32_t getLeftPosX() const;
		int32_t getRightPosX() const;
		int32_t getTopPosY() const;
		int32_t getBottomPosY() const;
		
		void getCenterPosition(int32_t& x, int32_t& y) const;
		int32_t getCenterPosX() const;
		int32_t getCenterPosY() const;
		
		void getCornerNW(int32_t& x, int32_t& y) const; // Exact same as getPosition(x,y)
		void getCornerNE(int32_t& x, int32_t& y) const;
		void getCornerSW(int32_t& x, int32_t& y) const;
		void getCornerSE(int32_t& x, int32_t& y) const;

		bool pointInBounds(int32_t x, int32_t y) const;

	private:
		int32_t ResX;
		int32_t ResY;
		int32_t PosX; // Top left corner
		int32_t PosY; // Top left corner
		fp64 RefreshRate;
		uint8_t BitsPerPixel;
		std::string Name;
};

namespace Display_Bootup {
	enum Display_Bootup_Enum {
		Automatic,CursorPosition,First,Last,Specific,
		Left,Right,Center,Top,Bottom,TopLeft,TopRight,BottomLeft,BottomRight,
		HighResolution,LowResolution,HighFrameRate,LowFrameRate,WideAspectRatio,TallAspectRatio,
		Length
	};
};

void getDisplayAreaBounds(
	const DisplayInfo*& displayArray, size_t displayCount,
	int32_t& leftBound, int32_t& rightBound,
	int32_t& topBound, int32_t& bottomBound
);

// Returns 0 if no matches were found
int32_t getDisplayFromPosition(
	const DisplayInfo*& displayArray, size_t displayCount,
	int32_t posX, int32_t posY
);

// Returns 0 if no matches were found
int32_t matchDisplay(
	const DisplayInfo*& displayArray, size_t displayCount,
	Display_Bootup::Display_Bootup_Enum bootupType,
	int32_t specificDisplay,
	int32_t cursorPosX, int32_t cursorPosY
	int32_t minResX, int32_t minResY
);

#endif /* DISPLAY_INFO_H */