/*
**	Author: zerico2005 (2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "displayInfo.h"

/* class DisplayInfo */
	DisplayInfo::DisplayInfo(
		int32_t resX, int32_t resY,
		int32_t posX, int32_t posY,
		fp64 refreshRate, uint8_t bitsPerPixel,
		std::string name
	) {
		updateDisplayInfo(resX,resY,posX,posY,refreshRate,bitsPerPixel,name);
	}
	void DisplayInfo::updateDisplayInfo(
		int32_t resX, int32_t resY,
		int32_t posX, int32_t posY,
		fp64 refreshRate, uint8_t bitsPerPixel,
		std::string name
	) {
		ResX = resX;
		ResY = resY;
		PosX = posX;
		PosY = posY;
		RefreshRate = refreshRate;
		BitsPerPixel = bitsPerPixel;
		Name = name;
	}
void DisplayInfo::retriveDisplayInfo(
	int32_t* resX, int32_t* resY,
	int32_t* posX, int32_t* posY,
	fp64* refreshRate, uint8_t* bitsPerPixel,
	std::string* name
) const {
	if (resX != nullptr) { *resX = ResX; }
	if (resY != nullptr) { *resY = ResY; }
	if (posX != nullptr) { *posX = PosX; }
	if (posY != nullptr) { *posY = PosY; }
	if (refreshRate != nullptr) { *refreshRate = RefreshRate; }
	if (bitsPerPixel != nullptr) { *bitsPerPixel = BitsPerPixel; }
	if (name != nullptr) { *name = Name; }
}
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

void getDisplayAreaBounds(
	const DisplayInfo*& displayArray, size_t displayCount,
	int32_t& leftBound, int32_t& rightBound,
	int32_t& topBound, int32_t& bottomBound
) {
	if (displayCount <= 0) { return; }
	leftBound = displayArray[0].getLeftPosX();
	rightBound = displayArray[0].getRightPosX();
	topBound = displayArray[0].getTopPosY();
	bottomBound = displayArray[0].getBottomPosY();
	for (size_t i = 1; i < displayCount; i++) {
		leftBound   = (displayArray[i].getLeftPosX()   < leftBound  ) ? displayArray[i].getLeftPosX()   : leftBound  ;
		rightBound  = (displayArray[i].getRightPosX()  > rightBound ) ? displayArray[i].getRightPosX()  : rightBound ;
		topBound    = (displayArray[i].getTopPosY()    > topBound   ) ? displayArray[i].getTopPosY()    : topBound   ;
		bottomBound = (displayArray[i].getBottomPosY() < bottomBound) ? displayArray[i].getBottomPosY() : bottomBound;
	}
}

int32_t getDisplayFromPosition(
	const DisplayInfo*& displayArray, size_t displayCount,
	int32_t posX, int32_t posY
) {
	for (size_t i = 0; i < displayCount; i++) {
		if (displayArray[i].pointInBounds(posX,posY) == true) {
			return BASE_0_TO_1_INDEXING(i);
		}
	}
	return 0;
}

bool displayAboveMinimumResolution(const DisplayInfo& disp, int32_t minResX, int32_t minResY) {
	int32_t resX; int32_t resY;
	disp.getResolution(resX,resY);
	return (resX >= minResX || resY >= minResY) ? true : false;
}

int32_t matchDisplay(
	const DisplayInfo*& displayArray, size_t displayCount,
	Display_Bootup::Display_Bootup_Enum type,
	int32_t specificDisplay,
	int32_t cursorPosX, int32_t cursorPosY,
	int32_t minResX, int32_t minResY
) {
	if (displayCount == 0) {
		return 0;
	}
	size_t searchStart = 0;
	for (; searchStart < displayCount; searchStart++) {
		if (displayAboveMinimumResolution(displayArray[searchStart],minResX,minResY) == true) {
			break;
		}
	}
	int32_t bestDisplay = 0;
	if (searchStart == displayCount) { // Finding the highest resolution display if none met the minimum screen resolution
		for (size_t i = 1; i < displayCount; i++) {
			if (displayArray[i].getPixelCount() > displayArray[bestDisplay].getPixelCount()) {
				bestDisplay = i;
			}
		}
		return bestDisplay;
	}

	int32_t leftBound, rightBound, topBound, bottomBound;
	getDisplayAreaBounds(
		displayArray, displayCount,
		leftBound, rightBound,
		topBound, bottomBound
	);
	int32_t centerX = (leftBound + rightBound) / 2;
	int32_t centerY = (topBound + bottomBound) / 2;
	switch (type) {
		case Display_Bootup::Automatic:
		case Display_Bootup::CursorPosition:
		return getDisplayFromPosition(displayArray,displayCount,cursorPosX,cursorPosY);
		case Display_Bootup::First:
		return 1;
		case Display_Bootup::Last:
		return displayCount - 1;
		case Display_Bootup::Specific:
		return (specificDisplay < BASE_0_TO_1_INDEXING((int)displayCount)) ? specificDisplay : BASE_0_TO_1_INDEXING((int)displayCount - 1);
		case Display_Bootup::Left:
			for (size_t i = searchStart; i < displayCount; i++) {
				if (displayArray[i].getCenterPosX() < displayArray[bestDisplay].getCenterPosX() &&
				displayAboveMinimumResolution(displayArray[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::Right:
			for (size_t i = searchStart; i < displayCount; i++) {
				if (displayArray[i].getCenterPosX() > displayArray[bestDisplay].getCenterPosX() &&
				displayAboveMinimumResolution(displayArray[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::Top:
			for (size_t i = searchStart; i < displayCount; i++) {
				if (displayArray[i].getCenterPosY() > displayArray[bestDisplay].getCenterPosY() &&
				displayAboveMinimumResolution(displayArray[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::Bottom:
			for (size_t i = searchStart; i < displayCount; i++) {
				if (displayArray[i].getCenterPosY() < displayArray[bestDisplay].getCenterPosY() &&
				displayAboveMinimumResolution(displayArray[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::HighResolution:
			for (size_t i = searchStart; i < displayCount; i++) {
				if (displayArray[i].getPixelCount() > displayArray[bestDisplay].getPixelCount() &&
				displayAboveMinimumResolution(displayArray[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::LowResolution:
			for (size_t i = searchStart; i < displayCount; i++) {
				if (displayArray[i].getPixelCount() < displayArray[bestDisplay].getPixelCount() &&
				displayAboveMinimumResolution(displayArray[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::HighFrameRate:
			for (size_t i = searchStart; i < displayCount; i++) {
				if (displayArray[i].getRefreshRate() > displayArray[bestDisplay].getRefreshRate() &&
				displayAboveMinimumResolution(displayArray[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::LowFrameRate:
			for (size_t i = searchStart; i < displayCount; i++) {
				if (displayArray[i].getRefreshRate() < displayArray[bestDisplay].getRefreshRate() &&
				displayAboveMinimumResolution(displayArray[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::WideAspectRatio:
			for (size_t i = searchStart; i < displayCount; i++) {
				if (displayArray[i].getHorizontalAspectRatio() > displayArray[bestDisplay].getHorizontalAspectRatio() &&
				displayAboveMinimumResolution(displayArray[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::TallAspectRatio:
			for (size_t i = searchStart; i < displayCount; i++) {
				if (displayArray[i].getVerticalAspectRatio() > displayArray[bestDisplay].getVerticalAspectRatio() &&
				displayAboveMinimumResolution(displayArray[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::Center:
			for (size_t i = searchStart; i < displayCount; i++) {
				if (displayArray[i].getSquaredDistanceFromPoint(centerX,centerY) < displayArray[bestDisplay].getSquaredDistanceFromPoint(centerX,centerY) &&
				displayAboveMinimumResolution(displayArray[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::TopLeft:
			for (size_t i = searchStart; i < displayCount; i++) {
				if (displayArray[i].getSquaredDistanceFromPoint(leftBound,topBound) < displayArray[bestDisplay].getSquaredDistanceFromPoint(leftBound,topBound) &&
				displayAboveMinimumResolution(displayArray[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::TopRight:
			for (size_t i = searchStart; i < displayCount; i++) {
				if (displayArray[i].getSquaredDistanceFromPoint(rightBound,topBound) < displayArray[bestDisplay].getSquaredDistanceFromPoint(rightBound,topBound) &&
				displayAboveMinimumResolution(displayArray[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::BottomLeft:
			for (size_t i = searchStart; i < displayCount; i++) {
				if (displayArray[i].getSquaredDistanceFromPoint(leftBound,bottomBound) < displayArray[bestDisplay].getSquaredDistanceFromPoint(leftBound,bottomBound) &&
				displayAboveMinimumResolution(displayArray[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		case Display_Bootup::BottomRight:
			for (size_t i = searchStart; i < displayCount; i++) {
				if (displayArray[i].getSquaredDistanceFromPoint(rightBound,bottomBound) < displayArray[bestDisplay].getSquaredDistanceFromPoint(rightBound,bottomBound) &&
				displayAboveMinimumResolution(displayArray[i],minResX,minResY) == true) {
					bestDisplay = i;
				}
			}
			break;
		default:
		return 0;
	};
	return bestDisplay;
}

uint64_t getDisplayConfigHash(const DisplayInfo*& displayArray, size_t displayCount) {
	if (displayCount == 0) {
		return 0;
	}
	uint64_t hashAcc = 0;
	struct DisplayHash {
		int32_t resX; int32_t resY;
		int32_t posX; int32_t posY; 
		uint64_t refreshRate; uint8_t bitsPerPixel;
		size_t index; size_t count;
	};
	DisplayHash dispHash; memset(&dispHash,0,sizeof(DisplayHash));
	for (size_t i = 0; i < displayCount; i++) {
		displayArray[i].retriveDisplayInfo(
			&dispHash.resX,&dispHash.resY,
			&dispHash.posX,&dispHash.posY,
			(fp64*)((void*)&dispHash.refreshRate),&dispHash.bitsPerPixel
		);
		dispHash.index = i;
		dispHash.count = displayCount;
		hashAcc += fnv1a_hash((uint8_t*)((void*)&dispHash),sizeof(DisplayHash));
	}
	return hashAcc;
}