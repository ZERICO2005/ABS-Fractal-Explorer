/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRAC_MULTI_FLOAT64x2_AVX_H
#define FRAC_MULTI_FLOAT64x2_AVX_H

#include "frac_Multi_Internal.h"
#include "../floats/double_Float64.hpp"

#ifdef ENABLE_AVX_RENDERING

void quadraticRender_AVX_FP64x2(FractalParameters(fp64x2, fp64));
void     cubicRender_AVX_FP64x2(FractalParameters(fp64x2, fp64));
void   quarticRender_AVX_FP64x2(FractalParameters(fp64x2, fp64));
void   quinticRender_AVX_FP64x2(FractalParameters(fp64x2, fp64));
void    sexticRender_AVX_FP64x2(FractalParameters(fp64x2, fp64));

#endif /* ENABLE_AVX_RENDERING */

#endif /* FRAC_MULTI_FLOAT64x2_AVX_H */