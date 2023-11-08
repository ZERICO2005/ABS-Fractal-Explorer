#ifndef PRIMEUX_H
#define PRIMEUX_H

#include "gamma.h"

struct _ClickBox {
    i32 x0;
    i32 x1;
    i32 y0;
    i32 y1;
}; typedef struct _ClickBox ClickBox;

void setClickBox(ClickBox* box, i32 x0, i32 y0, i32 x1, i32 y1); // origin xy, length xy
void setClickBoxPoints(ClickBox* box, i32 x0, i32 y0, i32 x1, i32 y1); //origin xy, end xy
u8 checkClickBox(ClickBox* box, i32 button);

void updateMouse(i32 x, i32 y, i32 button);

i32 cursorX();
i32 cursorY();
i32 cursorButton();
void cursorClear();

#endif /* PRIMEUX_H */