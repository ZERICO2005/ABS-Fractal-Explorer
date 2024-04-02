/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FRAC_CL_H
#define FRAC_CL_H

#include "gamma.h"

void renderFormula_OpenCL(frac* parameter, uint8_t* data, uint32_t resX, uint32_t resY);

void init_OpenCL(uint32_t resX, uint32_t resY);
void terminate_OpenCL();
void queryOpenCL_GPU();

#endif /* FRAC_CL_H */