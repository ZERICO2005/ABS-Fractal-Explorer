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

// Output BufferBox, Input ImageBuffer, src{{x00,y00},{x01,y01},{x10,y10}} dst{{x00,y00},{x01,y01},{x10,y10}}
int Image_Scaler_Parallelogram(
	BufferBox* buf, ImageBuffer* img, Render_Data* ren,
	uint32_t backgroundColor,
	BufferBox* backgroundImage, BufferBox* foregroundImage,
	int interpolation_mode,
	fp32 sx00, fp32 sy00,
	fp32 sx01, fp32 sy01, fp32 sx10, fp32 sy10,
	fp32 dx00, fp32 dy00,
	fp32 dx01, fp32 dy01, fp32 dx10, fp32 dy10
);

// Output BufferBox, Input ImageBuffer, src{{x00,y00},{x11,y11},{x01,y01},{x10,y10}} dst{{x00,y00},{x11,y11},{x01,y01},{x10,y10}}
int Image_Scaler_Quadrilateral(
	BufferBox* buf, ImageBuffer* img, Render_Data* ren,
	uint32_t backgroundColor,
	BufferBox* backgroundImage, BufferBox* foregroundImage,
	int interpolation_mode,
	fp32 sx00, fp32 sy00, fp32 sx11, fp32 sy11,
	fp32 sx01, fp32 sy01, fp32 sx10, fp32 sy10,
	fp32 dx00, fp32 dy00, fp32 dx11, fp32 dy11,
	fp32 dx01, fp32 dy01, fp32 dx10, fp32 dy10
);

#endif /* IMAGE_TRANSFORM_H */