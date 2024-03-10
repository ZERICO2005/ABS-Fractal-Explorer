/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "gamma.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

uint8_t imageOutPathSet = 0;
const char* imageOutPath = "";

void setPath(char* path) {
	
}

void writeImage(char* name, uint8_t* data, uint32_t resX, uint32_t resY) {
//	if (imageOutPathSet == 0) {
//		setPath(imageOutPath);
//	}
	char* fileName = (char*)malloc(strlen(imageOutPath) + strlen(name) + strlen(".png") + 1);
	sprintf(fileName,"%s%s.png",imageOutPath,name);
	printf("\nWriting File: %s",fileName); fflush(stdout);
	//stbi_flip_vertically_on_write(1);
    stbi_write_png(fileName, resX, resY, 3, data, (resX * 3));
	free(fileName);
}