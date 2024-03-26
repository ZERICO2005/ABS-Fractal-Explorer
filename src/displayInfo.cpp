/*
**	Author: zerico2005 (2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "displayInfo.h"

#include "user_data.h"

#include <SDL2/SDL.h>

/* class DisplayInfo */
	DisplayInfo::DisplayInfo() { clearDisplayInfo(); };
	DisplayInfo::~DisplayInfo() { clearDisplayInfo(); };
	void DisplayInfo::clearDisplayInfo() {
		updateDisplayInfo(
			0,
			0,0,
			0,0,
			0.0,
			0,
			""
		);
	}
	DisplayInfo::DisplayInfo(
		int32_t index,
		int32_t resX, int32_t resY,
		int32_t posX, int32_t posY,
		fp64 refreshRate,
		uint8_t bitsPerPixel,
		std::string name
	) {
		updateDisplayInfo(
			index,
			resX, resY,
			posX, posY,
			refreshRate, bitsPerPixel,
			name
		);
	}
	void DisplayInfo::updateDisplayInfo(
		int32_t index,
		int32_t resX, int32_t resY,
		int32_t posX, int32_t posY,
		fp64 refreshRate,
		uint8_t bitsPerPixel,
		std::string name
	) {
		Index = index;
		ResX = resX; ResY = resY;
		PosX = posX; PosY = posY;
		RefreshRate = refreshRate;
		BitsPerPixel = bitsPerPixel;
		Name = name;
	}
	void DisplayInfo::retriveDisplayInfo(
		int32_t* index,
		int32_t* resX, int32_t* resY,
		int32_t* posX, int32_t* posY,
		fp64* refreshRate,
		uint8_t* bitsPerPixel,
		std::string* name
	) const {
		if (index != nullptr) { *index = Index; }
		if (resX != nullptr) { *resX = ResX; }
		if (resY != nullptr) { *resY = ResY; }
		if (posX != nullptr) { *posX = PosX; }
		if (posY != nullptr) { *posY = PosY; }
		if (refreshRate != nullptr) { *refreshRate = RefreshRate; }
		if (bitsPerPixel != nullptr) { *bitsPerPixel = BitsPerPixel; }
		if (name != nullptr) { *name = Name; }
	}
	int32_t DisplayInfo::getIndex() const { return Index; }
	void DisplayInfo::getPosition(int32_t& x, int32_t& y) const {
		x = PosX;
		y = PosY;
	}
	int64_t DisplayInfo::getSquaredDistanceFromPoint(int32_t x, int32_t y) const {
		return (int64_t)(PosX - x) * (int64_t)(PosX - x) + (int64_t)(PosY - y) * (int64_t)(PosY - y);
	}
	void DisplayInfo::getResolution(int32_t& x, int32_t& y) const {
		x = ResX;
		y = ResY;
	}
	size_t DisplayInfo::getPixelCount() const { return ResX * ResY; }
	fp64 DisplayInfo::getHorizontalAspectRatio() const {
		return (ResY != 0) ? ((fp64)ResX / (fp64)ResY) : 0.0;
	}
	fp64 DisplayInfo::getVerticalAspectRatio() const {
		return (ResX != 0) ? ((fp64)ResY / (fp64)ResX) : 0.0;
	}
	fp64 DisplayInfo::getRefreshRate() const { return RefreshRate; }
	uint8_t DisplayInfo::getBitsPerPixel() const { return BitsPerPixel; }
	std::string DisplayInfo::getName() const { return Name; }

	void DisplayInfo::printInfo() const {

	}

	int32_t DisplayInfo::getLeftPosX() const { return PosX; }
	int32_t DisplayInfo::getRightPosX() const { return PosX + ResX; }
	int32_t DisplayInfo::getTopPosY() const { return PosY; }
	int32_t DisplayInfo::getBottomPosY() const { return PosY + ResY; }
	
	void DisplayInfo::getCenterPosition(int32_t& x, int32_t& y) const {
		x = PosX + (ResX / 2);
		y = PosY + (ResY / 2);
	}
	int32_t DisplayInfo::getCenterPosX() const {
		return PosX + (ResX / 2);
	}
	int32_t DisplayInfo::getCenterPosY() const {
		return PosY + (ResY / 2);
	}
	
	void DisplayInfo::getCornerNW(int32_t& x, int32_t& y) const { // Exact same as getPosition(x,y)
		x = PosX;
		y = PosY;
	}
	void DisplayInfo::getCornerNE(int32_t& x, int32_t& y) const {
		x = PosX + ResX;
		y = PosY;
	}
	void DisplayInfo::getCornerSW(int32_t& x, int32_t& y) const {
		x = PosX;
		y = PosY + ResY;
	}
	void DisplayInfo::getCornerSE(int32_t& x, int32_t& y) const {
		x = PosX + ResX;
		y = PosY + ResY;
	}

	bool DisplayInfo::pointInBounds(int32_t x, int32_t y) const {
		return (
			((x >= PosX) && (x <= PosX + ResX)) &&
			((y >= PosY) && (y <= PosY + ResY))
		) ? true : false;
	}


static std::vector<DisplayInfo> displayList;

const std::vector<DisplayInfo>& getDisplayList() { return displayList; }

int32_t getDisplayCount() { return displayList.size(); }

int32_t reloadDisplays() {
	displayList.clear();
	int32_t displayCount = SDL_GetNumVideoDisplays();
	if (displayCount <= 0) {
		printFlush("\n%s",SDL_GetError());
		return 0;
	}
	displayList.resize(displayCount);
	SDL_DisplayMode mode;
	SDL_Rect rect;
	for (int32_t index = 0; index < displayCount; index++) {
		if (SDL_GetDesktopDisplayMode(index,&mode) != 0) {
			displayList.clear();
			printFlush("\n%s",SDL_GetError());
		}
		SDL_GetDisplayBounds(index, &rect);
		displayList[index] = DisplayInfo(
			index + 1,
			mode.w, mode.h,
			rect.x, rect.y,
			mode.refresh_rate, mode.format,
			SDL_GetDisplayName(index)
		);
	}
	return displayCount;
}

void getDisplayAreaBounds(
	int32_t& leftBound, int32_t& rightBound,
	int32_t& topBound, int32_t& bottomBound
) {
	const std::vector<DisplayInfo>& DisplayList = getDisplayList();
	if (DisplayList.size() == 0) { return; }
	leftBound = DisplayList[0].getLeftPosX();
	rightBound = DisplayList[0].getRightPosX();
	topBound = DisplayList[0].getTopPosY();
	bottomBound = DisplayList[0].getBottomPosY();
	for (size_t i = 1; i < DisplayList.size(); i++) {
		leftBound   = (DisplayList[i].getLeftPosX()   < leftBound  ) ? DisplayList[i].getLeftPosX()   : leftBound  ;
		rightBound  = (DisplayList[i].getRightPosX()  > rightBound ) ? DisplayList[i].getRightPosX()  : rightBound ;
		topBound    = (DisplayList[i].getTopPosY()    > topBound   ) ? DisplayList[i].getTopPosY()    : topBound   ;
		bottomBound = (DisplayList[i].getBottomPosY() < bottomBound) ? DisplayList[i].getBottomPosY() : bottomBound;
	}
}

const DisplayInfo* getDisplayFromIndex(int32_t index) {
	const std::vector<DisplayInfo>& DisplayList = getDisplayList();
	for (size_t i = 0; i < DisplayList.size(); i++) {
		if (DisplayList[i].getIndex() == index) {
			return &DisplayList[i];
		}
	}
	return nullptr;
}

const DisplayInfo* getDisplayFromPosition(
	int32_t posX, int32_t posY
) {
	const std::vector<DisplayInfo>& DisplayList = getDisplayList();
	for (size_t i = 0; i < DisplayList.size(); i++) {
		if (DisplayList[i].pointInBounds(posX,posY) == true) {
			return &DisplayList[i];
		}
	}
	return nullptr;
}

bool displayAboveMinimumResolution(const DisplayInfo& disp, int32_t minResX, int32_t minResY) {
	int32_t resX; int32_t resY;
	disp.getResolution(resX,resY);
	return (resX >= minResX && resY >= minResY) ? true : false;
}

// Finds the display with the highest non-zero resolution
const DisplayInfo* matchDisplayFallback() {
	const std::vector<DisplayInfo>& DisplayList = getDisplayList();
	for (size_t i = 0; i < DisplayList.size(); i++) {\
		int32_t resX, resY;
		DisplayList[i].getResolution(resX,resY);
		if (resX > 0 && resY > 0) {
			return &DisplayList[i];
		}
	}
	return nullptr;
}

const DisplayInfo* matchFirstDisplay(
	int32_t minResX, int32_t minResY
) {
	const std::vector<DisplayInfo>& DisplayList = getDisplayList();
	for (size_t i = 0; i < DisplayList.size(); i++) {
		if (displayAboveMinimumResolution(DisplayList[i],minResX,minResY)) {
			return &DisplayList[i];
		}
	}
	return matchDisplayFallback();
}

const DisplayInfo* matchLastDisplay(
	int32_t minResX, int32_t minResY
) {
	const std::vector<DisplayInfo>& DisplayList = getDisplayList();
	for (size_t i = DisplayList.size(); i-- > 0;) {
		if (displayAboveMinimumResolution(DisplayList[i],minResX,minResY)) {
			return &DisplayList[i];
		}
	}
	return matchDisplayFallback();
}

const DisplayInfo* matchAutomaticDisplay(
	const User_Display_Preferences& Display_Config,
	int32_t minResX, int32_t minResY,
	int32_t cursorPosX, int32_t cursorPosY
) {
	const std::vector<DisplayInfo>& DisplayList = getDisplayList();
	if (DisplayList.size() == 0) { return nullptr; }
	if (
		(Display_Config.Display_Config_Hash == getDisplayConfigHash()) &&
		(Display_Config.Previous_Display_Used > 0)
	) {
		const DisplayInfo* dispPrevious = getDisplayFromIndex(Display_Config.Previous_Display_Used);
		if (dispPrevious != nullptr && displayAboveMinimumResolution(*dispPrevious,minResX,minResY) == true) {
			return dispPrevious;
		}
	}
	const DisplayInfo* disp = getDisplayFromPosition(cursorPosX,cursorPosY);
	return (disp != nullptr) ? disp : matchFirstDisplay(minResX,minResY);
}

const DisplayInfo* getBootupDisplay(
	const User_Display_Preferences& Display_Config,
	int32_t minResX, int32_t minResY,
	int32_t cursorPosX, int32_t cursorPosY
) {
	const std::vector<DisplayInfo>& DisplayList = getDisplayList();
	if (DisplayList.size() == 0) { return nullptr; }
	if (DisplayList.size() == 1) {
		return matchDisplayFallback(); // This will check that the display has a non-zero resolution
	}
	size_t searchStart = 0;
	for (; searchStart < DisplayList.size(); searchStart++) {
		if (displayAboveMinimumResolution(DisplayList[searchStart],minResX,minResY) == true) {
			break;
		}
	}
	int32_t bestDisplay = 0;
	if (searchStart == DisplayList.size()) { // Finding the highest resolution display if none met the minimum screen resolution
		for (size_t i = 1; i < DisplayList.size(); i++) {
			if (DisplayList[i].getPixelCount() > DisplayList[bestDisplay].getPixelCount()) {
				bestDisplay = i;
			}
		}
		return &DisplayList[bestDisplay];
	}

	int32_t leftBound, rightBound, topBound, bottomBound;
	getDisplayAreaBounds(
		leftBound, rightBound,
		topBound, bottomBound
	);
	int32_t centerX = (leftBound + rightBound) / 2;
	int32_t centerY = (topBound + bottomBound) / 2;
	switch (Display_Config.Display_Bootup_Type) {
		case Display_Bootup::Automatic:
			return matchAutomaticDisplay(
				Display_Config,
				minResX, minResY,
				cursorPosX, cursorPosY
			);
		case Display_Bootup::CursorPosition:
			{
				const DisplayInfo* disp = getDisplayFromPosition(cursorPosX,cursorPosY);
				if (disp != nullptr) {
					return disp;
				}
			}
		case Display_Bootup::Specific:
			{
				const DisplayInfo* disp = getDisplayFromIndex(Display_Config.Specific_Bootup_Display);
				if (disp != nullptr) {
					return disp;
				}
			}
		case Display_Bootup::First:
			return matchFirstDisplay(minResX,minResY);
		case Display_Bootup::Last:
			return matchLastDisplay(minResX,minResY);
		case Display_Bootup::Left:
			for (size_t i = searchStart; i < DisplayList.size(); i++) {
				if (DisplayList[i].getCenterPosX() < DisplayList[bestDisplay].getCenterPosX() &&
				displayAboveMinimumResolution(DisplayList[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::Right:
			for (size_t i = searchStart; i < DisplayList.size(); i++) {
				if (DisplayList[i].getCenterPosX() > DisplayList[bestDisplay].getCenterPosX() &&
				displayAboveMinimumResolution(DisplayList[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::Top:
			for (size_t i = searchStart; i < DisplayList.size(); i++) {
				if (DisplayList[i].getCenterPosY() > DisplayList[bestDisplay].getCenterPosY() &&
				displayAboveMinimumResolution(DisplayList[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::Bottom:
			for (size_t i = searchStart; i < DisplayList.size(); i++) {
				if (DisplayList[i].getCenterPosY() < DisplayList[bestDisplay].getCenterPosY() &&
				displayAboveMinimumResolution(DisplayList[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::HighResolution:
			for (size_t i = searchStart; i < DisplayList.size(); i++) {
				if (DisplayList[i].getPixelCount() > DisplayList[bestDisplay].getPixelCount() &&
				displayAboveMinimumResolution(DisplayList[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::LowResolution:
			for (size_t i = searchStart; i < DisplayList.size(); i++) {
				if (DisplayList[i].getPixelCount() < DisplayList[bestDisplay].getPixelCount() &&
				displayAboveMinimumResolution(DisplayList[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::HighFrameRate:
			for (size_t i = searchStart; i < DisplayList.size(); i++) {
				if (DisplayList[i].getRefreshRate() > DisplayList[bestDisplay].getRefreshRate() &&
				displayAboveMinimumResolution(DisplayList[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::LowFrameRate:
			for (size_t i = searchStart; i < DisplayList.size(); i++) {
				if (DisplayList[i].getRefreshRate() < DisplayList[bestDisplay].getRefreshRate() &&
				displayAboveMinimumResolution(DisplayList[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::WideAspectRatio:
			for (size_t i = searchStart; i < DisplayList.size(); i++) {
				if (DisplayList[i].getHorizontalAspectRatio() > DisplayList[bestDisplay].getHorizontalAspectRatio() &&
				displayAboveMinimumResolution(DisplayList[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::TallAspectRatio:
			for (size_t i = searchStart; i < DisplayList.size(); i++) {
				if (DisplayList[i].getVerticalAspectRatio() > DisplayList[bestDisplay].getVerticalAspectRatio() &&
				displayAboveMinimumResolution(DisplayList[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::Center:
			for (size_t i = searchStart; i < DisplayList.size(); i++) {
				if (DisplayList[i].getSquaredDistanceFromPoint(centerX,centerY) < DisplayList[bestDisplay].getSquaredDistanceFromPoint(centerX,centerY) &&
				displayAboveMinimumResolution(DisplayList[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::TopLeft:
			for (size_t i = searchStart; i < DisplayList.size(); i++) {
				if (DisplayList[i].getSquaredDistanceFromPoint(leftBound,topBound) < DisplayList[bestDisplay].getSquaredDistanceFromPoint(leftBound,topBound) &&
				displayAboveMinimumResolution(DisplayList[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::TopRight:
			for (size_t i = searchStart; i < DisplayList.size(); i++) {
				if (DisplayList[i].getSquaredDistanceFromPoint(rightBound,topBound) < DisplayList[bestDisplay].getSquaredDistanceFromPoint(rightBound,topBound) &&
				displayAboveMinimumResolution(DisplayList[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::BottomLeft:
			for (size_t i = searchStart; i < DisplayList.size(); i++) {
				if (DisplayList[i].getSquaredDistanceFromPoint(leftBound,bottomBound) < DisplayList[bestDisplay].getSquaredDistanceFromPoint(leftBound,bottomBound) &&
				displayAboveMinimumResolution(DisplayList[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::BottomRight:
			for (size_t i = searchStart; i < DisplayList.size(); i++) {
				if (DisplayList[i].getSquaredDistanceFromPoint(rightBound,bottomBound) < DisplayList[bestDisplay].getSquaredDistanceFromPoint(rightBound,bottomBound) &&
				displayAboveMinimumResolution(DisplayList[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		default:
		return nullptr;
	};
	return &DisplayList[bestDisplay];
}

const DisplayInfo* matchDisplayAttribute(
	Display_Bootup::Display_Bootup_Enum type,
	const User_Display_Preferences& Display_Config,
	int32_t minResX, int32_t minResY,
	int32_t cursorPosX, int32_t cursorPosY
) {
	User_Display_Preferences Display_Config_Copy = Display_Config;
	Display_Config_Copy.Display_Bootup_Type = type;
	return getBootupDisplay(Display_Config_Copy,minResX,minResY,cursorPosX,cursorPosY);
}

uint64_t getDisplayConfigHash() {
	const std::vector<DisplayInfo>& DisplayList = getDisplayList();
	if (DisplayList.size() == 0) {
		return 0x0;
	}
	uint64_t hashAcc = 0x0;
	struct DisplayHash {
		int32_t index;
		int32_t resX; int32_t resY;
		int32_t posX; int32_t posY; 
		uint64_t refreshRate; uint8_t bitsPerPixel;
		size_t count;
	};
	DisplayHash dispHash; memset(&dispHash,0x0,sizeof(DisplayHash));
	for (size_t i = 0; i < DisplayList.size(); i++) {
		DisplayList[i].retriveDisplayInfo(
			&dispHash.index,
			&dispHash.resX,&dispHash.resY,
			&dispHash.posX,&dispHash.posY,
			(fp64*)((void*)&dispHash.refreshRate),&dispHash.bitsPerPixel
		);
		dispHash.count = DisplayList.size();
		hashAcc += fnv1a_hash((uint8_t*)((void*)&dispHash),sizeof(DisplayHash));
	}
	return hashAcc;
}