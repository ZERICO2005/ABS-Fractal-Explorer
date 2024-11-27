/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRAC_MULTI_FLOAT64X2_SSE2_H
#define FRAC_MULTI_FLOAT64X2_SSE2_H

#include "frac_Multi_Internal.h"
#include "Float64x2/Float64x2.hpp"

#ifdef ENABLE_AVX_RENDERING

void quadraticRender_SSE2_FP64x2(FractalParameters(Float64x2, fp64));
void     cubicRender_SSE2_FP64x2(FractalParameters(Float64x2, fp64));
void   quarticRender_SSE2_FP64x2(FractalParameters(Float64x2, fp64));
void   quinticRender_SSE2_FP64x2(FractalParameters(Float64x2, fp64));
void    sexticRender_SSE2_FP64x2(FractalParameters(Float64x2, fp64));

#endif /* ENABLE_AVX_RENDERING */

#endif /* FRAC_MULTI_FLOAT64X2_SSE2_H */