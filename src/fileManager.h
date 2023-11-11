/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "Common_Def.h"
#include "copyBuffer.h"

int writePNGImage(BufferBox* buf, char* path, char* name, int compression_level = 8);
int writeJPGImage(BufferBox* buf, char* path, char* name, uint32_t quality = 95);
int writeTGAImage(BufferBox* buf, char* path, char* name);
int writeBMPImage(BufferBox* buf, char* path, char* name);
int writeHDRImage(float* buf, uint32_t resX, uint32_t resY, uint8_t channels, char* path, char* name);

#endif /* FILEMANAGER_H */