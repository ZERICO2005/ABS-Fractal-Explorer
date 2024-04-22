/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRAC_MULTI_GENERIC_H
#define FRAC_MULTI_GENERIC_H

#include "frac_Multi_Internal.h"

template<typename fpX, typename fpColor>
void quadraticRender_Generic(FractalParameters(fpX, fpColor));

template<typename fpX, typename fpColor>
void cubicRender_Generic(FractalParameters(fpX, fpColor));

template<typename fpX, typename fpColor>
void quarticRender_Generic(FractalParameters(fpX, fpColor));

template<typename fpX, typename fpColor>
void quinticRender_Generic(FractalParameters(fpX, fpColor));

template<typename fpX, typename fpColor>
void sexticRender_Generic(FractalParameters(fpX, fpColor));

// template <typename fpX, typename fpColor>
// void polynomialRender_Generic(FractalParameters(fpX, fpColor));

template<typename fpX, typename fpColor>
void polarRender_Generic(FractalParameters(fpX, fpColor));

#endif /* FRAC_MULTI_GENERIC_H */