/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRAME_TRANSFORMATION_H
#define FRAME_TRANSFORMATION_H

#include "Common_Def.h"
#include "Program_Def.h"

#include "render.h"
#include "copyBuffer.h"
#include "imageBuffer.h"
#include "fractal.h"
#include "user_data.h"

int frame_Transform(
	const ImageBuffer& image,
	BufferBox& blit,
	const ABS_Mandelbrot& FRAC,
	const User_Rendering_Settings& config
);

#endif /* FRAME_TRANSFORMATION_H */