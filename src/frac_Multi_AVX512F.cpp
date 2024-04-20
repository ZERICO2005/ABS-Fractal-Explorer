/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"

#include "frac_Multi_Internal.h"
#include "frac_Multi_AVX512F.h"

#include "fractal.h"
#include "render.h"

#ifdef ENABLE_AVX512F_RENDERING

#include <immintrin.h>



#endif /* ENABLE_AVX512F_RENDERING */