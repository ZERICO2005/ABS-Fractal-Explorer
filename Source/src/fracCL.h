#ifndef FRAC_CL_H
#define FRAC_CL_H

#include "gamma.h"

void renderFormula_OpenCL(frac* parameter, uint8_t* data, uint32_t resX, uint32_t resY);

void init_OpenCL(uint32_t resX, uint32_t resY);
void terminate_OpenCL();
void queryOpenCL_GPU();

#endif /* FRAC_CL_H */