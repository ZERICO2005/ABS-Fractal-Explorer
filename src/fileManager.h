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

int writePNGImage(BufferBox* buf, char* path, char* name, int compression_level = 8);
int writeJPGImage(BufferBox* buf, char* path, char* name, uint32_t quality = 95);
int writeTGAImage(BufferBox* buf, char* path, char* name);
int writeBMPImage(BufferBox* buf, char* path, char* name);
int writeHDRImage(float* buf, uint32_t resX, uint32_t resY, uint8_t channels, char* path, char* name);

#endif /* FILEMANAGER_H */