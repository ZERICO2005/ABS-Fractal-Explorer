/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "frac_Multi_Internal.h"
#include "frac_Multi_AVX512_F.h"

#ifdef ENABLE_AVX512_F_RENDERING

#ifndef __AVX512F__
	#error "__AVX512F__ is required to enable AVX512F rendering"
#endif

#include <immintrin.h>



#endif /* ENABLE_AVX512_F_RENDERING */