/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#ifndef FRAC_MULTI_SSE2_H
#define FRAC_MULTI_SSE2_H

#include "Common_Def.h"
#include "Program_Def.h"

#include "fractal.h"
#include "copyBuffer.h"

#include "frac_Multi_Internal.h"

void quadraticRender_SSE2_FP32(FractalParameters(fp32, fp32));
void     cubicRender_SSE2_FP32(FractalParameters(fp32, fp32));
void   quarticRender_SSE2_FP32(FractalParameters(fp32, fp32));
void   quinticRender_SSE2_FP32(FractalParameters(fp32, fp32));
void    sexticRender_SSE2_FP32(FractalParameters(fp32, fp32));

void quadraticRender_SSE2_FP64(FractalParameters(fp64, fp64));
void     cubicRender_SSE2_FP64(FractalParameters(fp64, fp64));
void   quarticRender_SSE2_FP64(FractalParameters(fp64, fp64));
void   quinticRender_SSE2_FP64(FractalParameters(fp64, fp64));
void    sexticRender_SSE2_FP64(FractalParameters(fp64, fp64));

#endif /* FRAC_MULTI_SSE2_H */