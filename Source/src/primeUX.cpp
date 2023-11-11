/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "gamma.h"
#include "render.h"
#include "graphics2D.h"
#include "primeUX.h"

i32 mouseX = 0;
i32 mouseY = 0;
i32 mouseButton = 0;
enum MouseStatus {mNONE = 0,mHOLD = 1,mCLICK = 2};
enum MouseStatus mStatus = mNONE;
u8 clickRead = 0;

void updateMouse(i32 x, i32 y, i32 button) {
    mouseX = x;
    mouseY = y;
    if (button == 0) {
        mStatus = mNONE;
        mouseButton = 0;
        return;
    }
    if (mStatus == mNONE) {
        mouseButton = button;
        mStatus = mCLICK;
        return;
    } else if (mStatus == mCLICK && clickRead == 1) {
        mouseButton = 0;
        mStatus = mHOLD;
    }
};

i32 cursorX() {
    return mouseX;
}
i32 cursorY() {
    return mouseY;
}
i32 cursorButton() {
    return mouseButton;
}
void cursorClear() {
    mouseButton = 0;
}

void setClickBox(ClickBox* box, i32 x0, i32 y0, i32 x1, i32 y1) { // origin xy, length xy
    box->x0 = x0;
    box->y0 = y0;
    box->x1 = x0 + x1;
    box->y1 = y0 + y1;
}

void setClickBoxPoints(ClickBox* box, i32 x0, i32 y0, i32 x1, i32 y1) { //origin xy, end xy
    box->x0 = x0;
    box->y0 = y0;
    box->x1 = x1;
    box->y1 = y1;
}

u8 checkClickBox(ClickBox* box, i32 button) {
    if (mStatus != mCLICK) { //Recent Click
        return 0;
    }
    clickRead = 1;
    if ((mouseButton & button) == 0) { //Correct Button
        return 0;
    }
    if (mouseX < box->x0 || mouseX > box->x1) { //X bound
        return 0;
    }
    if (mouseY < box->y0 || mouseY > box->y1) { //Y bound
        return 0;
    }
    return 1;
}