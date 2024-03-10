/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRAC_MULTI_H
#define FRAC_MULTI_H

#include "gamma.h"

void renderFormula_MultiThread(frac* parameter, uint8_t* data, u32 resX, u32 resY, u32 threadCount, uint32_t precision);

#endif /* FRAC_MULTI_H */