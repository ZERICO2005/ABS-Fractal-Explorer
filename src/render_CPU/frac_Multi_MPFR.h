/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRAC_MULTI_MPFR_H
#define FRAC_MULTI_MPFR_H

#include "frac_Multi_Internal.h"

#if defined(ENABLE_MPFR_RENDERING) && Enable_FloatMPFR

void quadraticRender_MPFR(FractalParameters(fp64, fp64));
// void     cubicRender_MPFR(FractalParameters(fpCord, fp64));
// void   quarticRender_MPFR(FractalParameters(fpCord, fp64));
// void   quinticRender_MPFR(FractalParameters(fpCord, fp64));
// void    sexticRender_MPFR(FractalParameters(fpCord, fp64));

#endif /* ENABLE_MPFR_RENDERING */

#endif /* FRAC_MULTI_MPFR_H */