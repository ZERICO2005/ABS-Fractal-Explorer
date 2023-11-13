/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef IMAGE_TRANSFORM_H
#define IMAGE_TRANSFORM_H

#include "Common_Def.h"
#include "Program_Def.h"
#include "copyBuffer.h"
#include "imageBuffer.h"
#include "render.h"

// Output BufferBox, Input ImageBuffer, cord{x00,y00} cord{x11,y11} cord{x01,y01} cord{x10,y10}
int Image_Scaler_Parallelogram(
	BufferBox* buf, ImageBuffer* img, Render_Data* ren,
	i32 sx00, i32 sy00,
	i32 sx01, i32 sy01, i32 sx10, i32 sy10,
	i32 dx00, i32 dy00,
	i32 dx01, i32 dy01, i32 dx10, i32 dy10
);

int Image_Scaler_Quadrilateral(
	BufferBox* buf, ImageBuffer* img, Render_Data* ren,
	i32 sx00, i32 sy00, i32 sx11, i32 sy11,
	i32 sx01, i32 sy01, i32 sx10, i32 sy10,
	i32 dx00, i32 dy00, i32 dx11, i32 dy11,
	i32 dx01, i32 dy01, i32 dx10, i32 dy10
);

#endif /* IMAGE_TRANSFORM_H */