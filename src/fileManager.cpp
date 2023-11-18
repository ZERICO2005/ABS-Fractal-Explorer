/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "fileManager.h"

#include "copyBuffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

/* File Openers */

#ifdef fileManager_Platform_Windows
	#include <windows.h>
	int openFileInterface(
		char* filePath, size_t filePathMaxLen,
		const char* title,
		const char* filter
	) {
		if (filePathMaxLen == 0) { return -1; }
		if (title == nullptr) { return -1; }
		if (filter == nullptr) { return -1; }	
		size_t len = (filePathMaxLen < MAX_PATH) ? filePathMaxLen : MAX_PATH;
		memset(filePath,'\0',len);
		OPENFILENAME ofn;
		memset(&ofn,0,sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFilter = filter;
		ofn.lpstrFile = filePath;
		ofn.nMaxFile = len;
		ofn.lpstrTitle = title;
		ofn.Flags = OFN_FILEMUSTEXIST;
		if (GetOpenFileName(&ofn)) {
			printFlush("\nOpened File: %s",filePath);
		} else {
			return 1;
		}
		return 0;
	}

	int saveFileInterface(
		char* filePath, size_t filePathMaxLen,
		const char* title,
		const char* filter,
		const char* fileType,
		const char* defaultName
	) {
		if (filePathMaxLen == 0) { return -1; }
		if (title == nullptr) { return -1; }
		if (filter == nullptr) { return -1; }	
		size_t len = (filePathMaxLen < MAX_PATH) ? filePathMaxLen : MAX_PATH;
		memset(filePath, '\0', len);
		OPENFILENAME ofn;
		memset(&ofn, 0, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFilter = filter;
		ofn.lpstrFile = filePath;
		ofn.nMaxFile = len;
		ofn.lpstrTitle = title;
		ofn.Flags = OFN_OVERWRITEPROMPT;

		ofn.lpstrDefExt = fileType;
		char* defaultName_copy = nullptr;
		if (defaultName != nullptr) {
			defaultName_copy = (char*)malloc(strnlen(defaultName,len) + 1);
		}
		ofn.lpstrFileTitle = defaultName_copy;
		bool fileSaved = GetSaveFileName(&ofn);
		FREE(defaultName_copy);

		if (fileSaved == true) {
			printf("\nFile Saved: %s", filePath);
		} else {
			return 1;
		}
		return 0;
	}
#endif

#ifdef fileManager_Platform_Linux
	int openFileInterface(
		char* filePath, size_t filePathMaxLen,
		const char* title,
		const char* filter
	) {
		if (filePathMaxLen == 0) { return -1; }
		return 0;
	}
	int saveFileInterface(
		char* filePath, size_t filePathMaxLen,
		const char* title,
		const char* filter,
		const char* fileType,
		const char* defaultName
	) {
		if (filePathMaxLen == 0) { return -1; }
		return 0;
	}

#endif



/* Image Writers */

int writePNGImage(BufferBox* buf, char* path, char* name, int compression_level) {
	if (printValidateBufferBox(buf) == false) { return -1; }
	if (path == NULL) { printError("char* path is NULL"); return -1; }
	if (name == NULL) { printError("char* name is NULL"); return -1; }
	if (getBufferBoxSize(buf) > (size_t)INT32_MAX) {
		printError("Image size of %zubytes is too large, maximum size: %dbytes",getBufferBoxSize(buf),INT32_MAX);
		return -1;
	}
	stbi_write_png_compression_level = compression_level;
	int ret = 0;
	char* fileName = (char*)malloc(strlen(path) + strlen(name) + strlen(".png") + 1);
	if (fileName == NULL) {
		printError("Failed to allocate PNG char* fileName");
		return -1;
	}
	sprintf(fileName,"%s%s.png",path,name);
	printf("\nWriting File: %s",fileName); fflush(stdout);
	//stbi_flip_vertically_on_write(1);
    ret = stbi_write_png(fileName, buf->resX, buf->resY, buf->channels, buf->vram, getBufferBoxPitch(buf));
	FREE(fileName);
	return ret;
}

int writeJPGImage(BufferBox* buf, char* path, char* name, uint32_t quality) {
	if (printValidateBufferBox(buf) == false) { return -1; }
	if (path == NULL) { printError("char* path is NULL"); return -1; }
	if (name == NULL) { printError("char* name is NULL"); return -1; }
	if (getBufferBoxSize(buf) > (size_t)INT32_MAX) {
		printError("Image size of %zubytes is too large, maximum size: %dbytes",getBufferBoxSize(buf),INT32_MAX);
		return -1;
	}
	int ret = 0;
	char* fileName = (char*)malloc(strlen(path) + strlen(name) + strlen(".jpg") + 1);
	if (fileName == NULL) {
		printError("Failed to allocate JPG char* fileName");
		return -1;
	}
	sprintf(fileName,"%s%s.jpg",path,name);
	printf("\nWriting File: %s",fileName); fflush(stdout);
	//stbi_flip_vertically_on_write(1);
    ret = stbi_write_jpg(fileName, buf->resX, buf->resY, buf->channels, buf->vram,quality);
	FREE(fileName);
	return ret;
}

int writeTGAImage(BufferBox* buf, char* path, char* name) {
	if (printValidateBufferBox(buf) == false) { return -1; }
	if (path == NULL) { printError("char* path is NULL"); return -1; }
	if (name == NULL) { printError("char* name is NULL"); return -1; }
	if (getBufferBoxSize(buf) > (size_t)INT32_MAX) {
		printError("Image size of %zubytes is too large, maximum size: %dbytes",getBufferBoxSize(buf),INT32_MAX);
		return -1;
	}
	int ret = 0;
	char* fileName = (char*)malloc(strlen(path) + strlen(name) + strlen(".tga") + 1);
	if (fileName == NULL) {
		printError("Failed to allocate TGA char* fileName");
		return -1;
	}
	sprintf(fileName,"%s%s.tga",path,name);
	printf("\nWriting File: %s",fileName); fflush(stdout);
	//stbi_flip_vertically_on_write(1);
    ret = stbi_write_tga(fileName, buf->resX, buf->resY, buf->channels, buf->vram);
	FREE(fileName);
	return ret;
}

int writeBMPImage(BufferBox* buf, char* path, char* name) {
	if (printValidateBufferBox(buf) == false) { return -1; }
	if (path == NULL) { printError("char* path is NULL"); return -1; }
	if (name == NULL) { printError("char* name is NULL"); return -1; }
	if (getBufferBoxSize(buf) > (size_t)INT32_MAX) {
		printError("Image size of %zubytes is too large, maximum size: %dbytes",getBufferBoxSize(buf),INT32_MAX);
		return -1;
	}
	int ret = 0;
	char* fileName = (char*)malloc(strlen(path) + strlen(name) + strlen(".bmp") + 1);
	if (fileName == NULL) {
		printError("Failed to allocate BMP char* fileName");
		return -1;
	}
	sprintf(fileName,"%s%s.bmp",path,name);
	printf("\nWriting File: %s",fileName); fflush(stdout);
	//stbi_flip_vertically_on_write(1);
    ret = stbi_write_bmp(fileName, buf->resX, buf->resY, buf->channels, buf->vram);
	FREE(fileName);
	return ret;
}

int writeHDRImage(float* buf, uint32_t resX, uint32_t resY, uint8_t channels, char* path, char* name) {
	/* Trival Errors */
		if (buf == NULL) { printError("float* buf is NULL"); return -1; }
		if (resX == 0 || resY == 0) { printError("Invalid dimensions %ux%u == 0",resX,resY); return -1; }
		if (channels == 0) { printError("Image has 0 channels"); return -1; }
		if (path == NULL) { printError("char* path is NULL"); return -1; }
		if (name == NULL) { printError("char* name is NULL"); return -1; }
	int ret = 0;
	char* fileName = (char*)malloc(strlen(path) + strlen(name) + strlen(".hdr") + 1);
	if (fileName == NULL) {
		printError("Failed to allocate HDR char* fileName");
		return -1;
	}
	sprintf(fileName,"%s%s.hdr",path,name);
	printf("\nWriting File: %s",fileName); fflush(stdout);
	//stbi_flip_vertically_on_write(1);
    ret = stbi_write_hdr(fileName, resX, resY, channels, buf);
	FREE(fileName);
	return ret;
}