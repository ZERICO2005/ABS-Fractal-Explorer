/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Graphics_Buffer.h"
#include "Graphics_Buffer_Internal.h"

struct Graphics_Box {
	uint8_t* buf;
	int32_t resX;
	int32_t resY;
	uint8_t channels;
	int32_t pitch;
	uint8_t padding;
}; typedef struct Graphics_Box Graphics_Box;

class Graphics_Buffer {
	public:
	Graphics_Buffer Graphics_Buffer() {

	}
	private:
};