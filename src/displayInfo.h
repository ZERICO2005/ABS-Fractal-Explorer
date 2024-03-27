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

#include "user_data.h"

class DisplayInfo {
	public:
		DisplayInfo();
		~DisplayInfo();
		DisplayInfo(
			int32_t index,
			int32_t resX, int32_t resY,
			int32_t posX, int32_t posY,
			fp64 refreshRate,
			uint8_t bitsPerPixel,
			std::string name
		);
		// Doesn't do anything
		void updateDisplayInfo(
			int32_t index,
			int32_t resX, int32_t resY,
			int32_t posX, int32_t posY,
			fp64 refreshRate,
			uint8_t bitsPerPixel,
			std::string name
		);
		// Writes to any non nullptr parameters
		void retriveDisplayInfo(
			int32_t* index,
			int32_t* resX, int32_t* resY,
			int32_t* posX, int32_t* posY,
			fp64* refreshRate = nullptr,
			uint8_t* bitsPerPixel = nullptr,
			std::string* name = nullptr
		) const;
		int32_t getIndex() const;
		void getPosition(int32_t& x, int32_t& y) const;
		int64_t getSquaredDistanceFromPoint(int32_t x, int32_t y) const;
		void getResolution(int32_t& x, int32_t& y) const;
		size_t getPixelCount() const;
		fp64 getHorizontalAspectRatio() const;
		fp64 getVerticalAspectRatio() const;
		fp64 getRefreshRate() const;
		uint8_t getBitsPerPixel() const;
		std::string getName() const;

		void printInfo() const;
	
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
		void clearDisplayInfo();
		int32_t Index;
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
	static const char* Display_Bootup_Text[] = {
		"Automatic","Cursor Position","First Monitor","Last Monitor","Specific Monitor",
		"Left","Right","Center","Top","Bottom","Top-Left","Top-Right","Bottom-Left","Bottom-Right",
		"Highest Resolution","Lowest Resolution","Highest Framerate","Lowest Framerate","Widest Aspect Ratio","Tallest Aspect Ratio"
	};
};

namespace Display_Fullscreen {
	enum Display_Fullscreen_Enum {
		Windowed, Windowed_Fullscreen, Borderless_Fullscreen, Fullscreen,
		Length
	};
	static const char* const Display_Fullscreen_Text[] {
		"Windowed", "Windowed-Fullscreen", "Borderless-Fullscreen", "Fullscreen"
	};
};

namespace Display_RefreshRate {
	enum Display_RefreshRate_Enum {
		Automatic,CurrentMonitor,HighestRefreshRate,LowestRefreshRate,ConstantValue,
		Length
	};
	const char* const Display_RefreshRate_Text[] = {
		"Automatic","Current Monitor","Highest Refresh-Rate","Lowest Refresh-Rate","Constant Value"
	};
};

const std::vector<DisplayInfo>& getDisplayList();

int32_t getDisplayCount();

// Returns the number of displays (0 on failure)
int32_t reloadDisplays();

void getDisplayAreaBounds(
	int32_t& leftBound, int32_t& rightBound,
	int32_t& topBound, int32_t& bottomBound
);

// index >= 1, returns nullptr on out of bounds
const DisplayInfo* getDisplayFromIndex(int32_t index);

// Returns nullptr if no matches were found
const DisplayInfo* getDisplayFromPosition(int32_t posX, int32_t posY);

struct SDL_Window;
const DisplayInfo* getDisplayFromWindowPosition(SDL_Window* window);

const DisplayInfo* getDisplayFromCursorPosition();

const DisplayInfo* matchDisplayAttribute(
	Display_Bootup::Display_Bootup_Enum type,
	const User_Display_Preferences& Display_Config,
	int32_t minResX = 0, int32_t minResY = 0,
	int32_t cursorPosX = INT32_MIN, int32_t cursorPosY = INT32_MIN
);

// Returns 0 if no matches were found
const DisplayInfo* getBootupDisplay(
	const User_Display_Preferences& Display_Config,
	int32_t minResX = 0, int32_t minResY = 0,
	int32_t cursorPosX = INT32_MIN, int32_t cursorPosY = INT32_MIN
);

// Used to determine if the display configuration has changed. Returns 0 if parameters are invalid
uint64_t getDisplayConfigHash();

#endif /* DISPLAY_INFO_H */