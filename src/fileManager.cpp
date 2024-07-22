/*
**	Author: zerico2005 (2023-2024)
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
	#include <ShlObj.h>
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
		OPENFILENAME ofn = {0};
		memset(&ofn,0,sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFilter = filter;
		ofn.lpstrFile = filePath;
		ofn.nMaxFile = (DWORD)len;
		ofn.lpstrTitle = title;
		ofn.Flags = OFN_FILEMUSTEXIST;
		if (GetOpenFileName(&ofn)) {
			printFlush("\nOpened File: \"%s\"",filePath);
		} else {
			printWarning("Unable to Open File from: \"%s\"",filePath);
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
		OPENFILENAME ofn = {0};
		memset(&ofn, 0, sizeof(ofn));

		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFilter = filter;
		ofn.lpstrFile = filePath;
		ofn.nMaxFile = (DWORD)len;
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
			printFlush("\nFile Saved: \"%s\"", filePath);
		} else {
			printWarning("Unable to Save File to: \"%s\"",filePath);
			return 1;
		}
		return 0;
	}

	int selectFolderInterface(
		char* folderPath, size_t folderPathMaxLen,
		const char* title
	) {
		if (folderPathMaxLen == 0) { return -1; }
		if (title == nullptr) { return -1; }

		size_t len = (folderPathMaxLen < MAX_PATH) ? folderPathMaxLen : MAX_PATH;
		memset(folderPath, '\0', len);

		BROWSEINFO bi = {0};
		bi.lpszTitle = title;
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

		if (pidl != 0) {
			// Folder was selected
			SHGetPathFromIDList(pidl, folderPath);

			IMalloc *imalloc = 0;
			if (SUCCEEDED(SHGetMalloc(&imalloc))) {
				imalloc->Free(pidl);
				imalloc->Release();
			}

			// Append backslash
			if (len >= 2) {
				// Up to the second to last character
				for (size_t i = 0; i < len - 1; i++) {
					// Find last character that is not the last index
					if (folderPath[i] == '\0') {
						folderPath[i] = '\\';
						folderPath[i + 1] = '\0';
						break;
					}
				}
			}

			return 0;
		}
		// User canceled
		return 1;
	}

	int init_FileInterface(__attribute__((unused)) int argc, __attribute__((unused)) char* argv[]) {
		return 0;
	}
	int terminate_FileInterface() {
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
		if (title == nullptr) { return -1; }
		if (filter == nullptr) { return -1; }

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

		return 0;
	}

	int init_FileInterface(__attribute__((unused)) int argc, __attribute__((unused)) char* argv[]) {
		//gtk_init(&argc, &argv);
		return 0;
	}
	int terminate_FileInterface() {
		//gtk_widget_destroy(file_chooser_dialog);
		return 0;
	}
#endif



/* Image Writers */

int writePNGImage(const BufferBox* buf, const char* path, const char* name, uint32_t compression_level) {
	if (printValidateBufferBox(buf) == false) { return -1; }
	if (path == nullptr) { printError("char* path is NULL"); return -1; }
	if (name == nullptr) { printError("char* name is NULL"); return -1; }
	if (getBufferBoxSize(buf) > (size_t)INT32_MAX) {
		printError("Image size of %zubytes is too large, maximum size: %dbytes",getBufferBoxSize(buf),INT32_MAX);
		return -1;
	}
	stbi_write_png_compression_level = (int32_t)compression_level;
	int ret = 0;
	const size_t total_filePathLen = strlen(path) + strlen(name) + strlen(".png") + 1;
	char* fileName = (char*)calloc(total_filePathLen, sizeof(char));
	if (fileName == nullptr) {
		printError("Failed to allocate PNG char* fileName");
		return -1;
	}
	snprintf(fileName, total_filePathLen, "%s%s.png", path, name);
	printf("\nWriting File: %s", fileName); fflush(stdout);
	//stbi_flip_vertically_on_write(1);
    ret = stbi_write_png(fileName, (int32_t)buf->resX, (int32_t)buf->resY, (int32_t)buf->channels, buf->vram, (int32_t)getBufferBoxPitch(buf));
	FREE(fileName);
	return ret;
}

int writeJPGImage(const BufferBox* buf, const char* path, const char* name, uint32_t quality) {
	if (printValidateBufferBox(buf) == false) { return -1; }
	if (path == nullptr) { printError("char* path is NULL"); return -1; }
	if (name == nullptr) { printError("char* name is NULL"); return -1; }
	if (getBufferBoxSize(buf) > (size_t)INT32_MAX) {
		printError("Image size of %zubytes is too large, maximum size: %dbytes",getBufferBoxSize(buf),INT32_MAX);
		return -1;
	}
	int ret = 0;
	const size_t total_filePathLen = strlen(path) + strlen(name) + strlen(".jpg") + 1;
	char* fileName = (char*)calloc(total_filePathLen, sizeof(char));
	if (fileName == nullptr) {
		printError("Failed to allocate JPG char* fileName");
		return -1;
	}
	snprintf(fileName, total_filePathLen, "%s%s.jpg", path, name);
	printf("\nWriting File: %s", fileName); fflush(stdout);
	//stbi_flip_vertically_on_write(1);
    ret = stbi_write_jpg(fileName, (int32_t)buf->resX, (int32_t)buf->resY, (int32_t)buf->channels, buf->vram, (int32_t)quality);
	FREE(fileName);
	return ret;
}

int writeTGAImage(const BufferBox* buf, const char* path, const char* name) {
	if (printValidateBufferBox(buf) == false) { return -1; }
	if (path == nullptr) { printError("char* path is NULL"); return -1; }
	if (name == nullptr) { printError("char* name is NULL"); return -1; }
	if (getBufferBoxSize(buf) > (size_t)INT32_MAX) {
		printError("Image size of %zubytes is too large, maximum size: %dbytes",getBufferBoxSize(buf),INT32_MAX);
		return -1;
	}
	int ret = 0;
	const size_t total_filePathLen = strlen(path) + strlen(name) + strlen(".tga") + 1;
	char* fileName = (char*)calloc(total_filePathLen, sizeof(char));
	if (fileName == nullptr) {
		printError("Failed to allocate TGA char* fileName");
		return -1;
	}
	snprintf(fileName, total_filePathLen, "%s%s.tga", path, name);
	printf("\nWriting File: %s", fileName); fflush(stdout);
	//stbi_flip_vertically_on_write(1);
    ret = stbi_write_tga(fileName, (int32_t)buf->resX, (int32_t)buf->resY, (int32_t)buf->channels, buf->vram);
	FREE(fileName);
	return ret;
}

int writeBMPImage(const BufferBox* buf, const char* path, const char* name) {
	if (printValidateBufferBox(buf) == false) { return -1; }
	if (path == nullptr) { printError("char* path is NULL"); return -1; }
	if (name == nullptr) { printError("char* name is NULL"); return -1; }
	if (getBufferBoxSize(buf) > (size_t)INT32_MAX) {
		printError("Image size of %zubytes is too large, maximum size: %dbytes",getBufferBoxSize(buf),INT32_MAX);
		return -1;
	}
	int ret = 0;
	const size_t total_filePathLen = strlen(path) + strlen(name) + strlen(".bmp") + 1;
	char* fileName = (char*)calloc(total_filePathLen, sizeof(char));
	if (fileName == nullptr) {
		printError("Failed to allocate BMP char* fileName");
		return -1;
	}
	snprintf(fileName, total_filePathLen, "%s%s.bmp", path, name);
	printf("\nWriting File: %s", fileName); fflush(stdout);
	//stbi_flip_vertically_on_write(1);
    ret = stbi_write_bmp(fileName, (int32_t)buf->resX, (int32_t)buf->resY, (int32_t)buf->channels, buf->vram);
	FREE(fileName);
	return ret;
}

int writeHDRImage(const float* buf, uint32_t resX, uint32_t resY, uint8_t channels, const char* path, const char* name) {
	/* Trival Errors */
		if (buf == nullptr) { printError("float* buf is NULL"); return -1; }
		if (resX == 0 || resY == 0) { printError("Invalid dimensions %ux%u == 0",resX,resY); return -1; }
		if (channels == 0) { printError("Image has 0 channels"); return -1; }
		if (path == nullptr) { printError("char* path is NULL"); return -1; }
		if (name == nullptr) { printError("char* name is NULL"); return -1; }
	int ret = 0;
	const size_t total_filePathLen = strlen(path) + strlen(name) + strlen(".hdr") + 1;
	char* fileName = (char*)calloc(total_filePathLen, sizeof(char));
	if (fileName == nullptr) {
		printError("Failed to allocate HDR char* fileName");
		return -1;
	}
	snprintf(fileName, total_filePathLen, "%s%s.hdr", path, name);
	printf("\nWriting File: %s", fileName); fflush(stdout);
	//stbi_flip_vertically_on_write(1);
    ret = stbi_write_hdr(fileName, (int32_t)resX, (int32_t)resY, (int32_t)channels, buf);
	FREE(fileName);
	return ret;
}