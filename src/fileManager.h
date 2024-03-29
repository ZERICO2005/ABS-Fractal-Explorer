/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "Common_Def.h"
#include "copyBuffer.h"

#define fileManager_Platform_Windows
//#define fileManager_Platform_Linux

/* fileManager.h is intended to have platform specific methods for opening/saving files */

int openFileInterface(
	char* filePath, size_t filePathMaxLen,
	const char* title = "Select a File",
	const char* filter = "All Files (*.*)\0*.*\0"
);
int saveFileInterface(
	char* filePath, size_t filePathMaxLen,
	const char* title = "Save As",
	const char* filter = "All Files (*.*)\0*.*\0",
	const char* fileType = nullptr,
	const char* defaultName = nullptr
);

/* Images */

int writePNGImage(const BufferBox* buf, char* path, char* name, uint32_t compression_level = 8);
int writeJPGImage(const BufferBox* buf, char* path, char* name, uint32_t quality = 95);
int writeTGAImage(const BufferBox* buf, char* path, char* name);
int writeBMPImage(const BufferBox* buf, char* path, char* name);
int writeHDRImage(const float* buf, uint32_t resX, uint32_t resY, uint8_t channels, char* path, char* name);

#endif /* FILEMANAGER_H */