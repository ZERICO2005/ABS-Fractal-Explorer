/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"
#include "programData.h"

/* Thread Ready */
	std::mutex pDat_Render_Ready_Mutex;
	bool pDat_Render_Ready = false;
	bool read_Render_Ready() {
		std::lock_guard<std::mutex> lock(pDat_Render_Ready_Mutex);
		return pDat_Render_Ready;
	}
	void write_Render_Ready(bool f) {
		std::lock_guard<std::mutex> lock(pDat_Render_Ready_Mutex);
		pDat_Render_Ready = f;
	}

	std::mutex pDat_Engine_Ready_Mutex;
	bool pDat_Engine_Ready = false;
	bool read_Engine_Ready() {
		std::lock_guard<std::mutex> lock(pDat_Engine_Ready_Mutex);
		return pDat_Engine_Ready;
	}
	void write_Engine_Ready(bool f) {
		std::lock_guard<std::mutex> lock(pDat_Engine_Ready_Mutex);
		pDat_Engine_Ready = f;
	}

/* Update_Level */
	std::mutex pDat_Update_Level_Mutex;
	int pDat_Update_Level = Change_Level::Full_Reset;
	uint64_t pDat_Update_Timecode = 0;

	// Used to deterimine if rendering should pause, continue, or reset
	int read_Update_Level() {
		std::lock_guard<std::mutex> lock(pDat_Update_Level_Mutex);
		return pDat_Update_Level;
	}
	uint64_t read_Update_Timecode() {
		std::lock_guard<std::mutex> lock(pDat_Update_Level_Mutex);
		return pDat_Update_Timecode;
	}
	// Stores the highest update level
	void write_Update_Level(int level) {
		std::lock_guard<std::mutex> lock(pDat_Update_Level_Mutex);
		if (level == 0) {
			return;
		}
		pDat_Update_Level = (level > pDat_Update_Level) ? level : pDat_Update_Level;
		pDat_Update_Timecode = getNanoTime();
		//printFlush("\nUpdate: %07llu",(pDat_Update_Timecode/1000) % 10000000);
	}
	// Used when render is complete
	void clear_Update_Level() {
		std::lock_guard<std::mutex> lock(pDat_Update_Level_Mutex);
		pDat_Update_Level = 0;
	}

/* Cycle Buffer */
	std::mutex pDat_Cycle_Mutex;

	#define Cycle_Buffer_Amount 3
	struct Cycle_Image_Buffer {
		ImageBuffer image_buf[Cycle_Buffer_Amount];
		int read_pos;
		int write_pos;
	}; typedef struct Cycle_Image_Buffer Cycle_Image_Buffer;
	Cycle_Image_Buffer Cycle_Buffer[Cycle_Buffer_Count];

	#define Cycle_Buf Cycle_Buffer[buf]
	// Gets the next frame to read from, returns false if there are no frames to grab
	int next_Read_Cycle_Pos(ImageBuffer** ptr, int buf) {
		if (ptr == nullptr) { return -2; }
		if (buf >= Cycle_Buffer_Count) { return -3; }
		std::lock_guard<std::mutex> lock(pDat_Cycle_Mutex);
		if (Cycle_Buffer[buf].read_pos == Cycle_Buffer[buf].write_pos) {
			printCriticalError(
				"next_Read_Cycle_Pos(): Overlapping Thread Data Pointers\nCycle_Buffer[%d].read_pos == Cycle_Buffer[%d].write_pos",
				Cycle_Buffer[buf].read_pos,Cycle_Buffer[buf].write_pos
			);
			return -1;
		}
		if ((Cycle_Buf.read_pos + 1) % Cycle_Buffer_Amount == Cycle_Buf.write_pos) {
			return 1;
		}
		Cycle_Buf.read_pos = (Cycle_Buf.read_pos + 1) % Cycle_Buffer_Amount;
		*ptr = &Cycle_Buf.image_buf[Cycle_Buf.read_pos];
		return 0;
	}
	// Gets the next buffer to write to, returns false if there are no buffers to grab
	int next_Write_Cycle_Pos(ImageBuffer** ptr, int buf) {
		if (ptr == nullptr) { return -2; }
		if (buf >= Cycle_Buffer_Count) { return -3; }
		std::lock_guard<std::mutex> lock(pDat_Cycle_Mutex);
		if (Cycle_Buffer[buf].read_pos == Cycle_Buffer[buf].write_pos) {
			printCriticalError(
				"next_Read_Cycle_Pos(): Overlapping Thread Data Pointers\nCycle_Buffer[%d].read_pos == Cycle_Buffer[%d].write_pos",
				Cycle_Buffer[buf].read_pos,Cycle_Buffer[buf].write_pos
			);
			return -1;
		}
		if ((Cycle_Buf.write_pos + 1) % Cycle_Buffer_Amount == Cycle_Buf.read_pos) {
			return 1;
		}
		Cycle_Buf.write_pos = (Cycle_Buf.write_pos + 1) % Cycle_Buffer_Amount;
		*ptr = &Cycle_Buf.image_buf[Cycle_Buf.write_pos];
		return 0;
	}
	// To be called by main
	void init_Cycle_Buffers() {
		std::lock_guard<std::mutex> lock(pDat_Cycle_Mutex);
		for (size_t i = 0; i < Cycle_Buffer_Count; i++) {
			for (size_t b = 0; b < Cycle_Buffer_Amount; b++) {
				Cycle_Buffer[i].image_buf[b] = ImageBuffer();
				Cycle_Buffer[i].read_pos = 0;
				Cycle_Buffer[i].write_pos = 1;
			}
		}
	}
	// Frees allocated memory | ImageBuffer.deleteBuffer()
	void delete_Cycle_Buffers() {
		std::lock_guard<std::mutex> lock(pDat_Cycle_Mutex);
		for (size_t i = 0; i < Cycle_Buffer_Count; i++) {
			for (size_t b = 0; b < Cycle_Buffer_Amount; b++) {
				Cycle_Buffer[i].image_buf[b].deleteBuffer();
			}
		}
	}
	// Frees unused portions of allocated memory | ImageBuffer.trimBuffer()
	void trim_Cycle_Buffers() {
		std::lock_guard<std::mutex> lock(pDat_Cycle_Mutex);
		for (size_t i = 0; i < Cycle_Buffer_Count; i++) {
			for (size_t b = 0; b < Cycle_Buffer_Amount; b++) {
				Cycle_Buffer[i].image_buf[b].trimBuffer();
			}
		}
	}
	// Clears buffer image to 0 | ImageBuffer.clearBuffer()
	void clear_Cycle_Buffers() {
		std::lock_guard<std::mutex> lock(pDat_Cycle_Mutex);
		for (size_t i = 0; i < Cycle_Buffer_Count; i++) {
			for (size_t b = 0; b < Cycle_Buffer_Amount; b++) {
				Cycle_Buffer[i].image_buf[b].clearBuffer();
			}
		}
	}

	std::mutex pDat_Buffer_Size_Mutex;
	BufferBox pDat_BufSize = {NULL,0,0,IMAGE_BUFFER_CHANNELS,0};
	void write_Buffer_Size(BufferBox size) {
		std::lock_guard<std::mutex> lock(pDat_Buffer_Size_Mutex);
		pDat_BufSize.resX = size.resX;
		pDat_BufSize.resY = size.resY;
		pDat_BufSize.channels = size.channels;
		pDat_BufSize.padding = size.padding;
	}
	BufferBox read_Buffer_Size() {
		std::lock_guard<std::mutex> lock(pDat_Buffer_Size_Mutex);
		return pDat_BufSize;
	}
	
std::mutex pDat_Abort_Render_Ongoing_Mutex;
bool pDat_Abort_Render_Ongoing = false;

bool read_Abort_Render_Ongoing() {
	std::lock_guard<std::mutex> lock(pDat_Abort_Render_Ongoing_Mutex);
	return pDat_Abort_Render_Ongoing;
}
void write_Abort_Render_Ongoing(bool s) {
	std::lock_guard<std::mutex> lock(pDat_Abort_Render_Ongoing_Mutex);
	pDat_Abort_Render_Ongoing = s;
}

/* Key Function */
std::mutex pDat_Function_Status_Mutex;
Function_Status pDat_Function_Status[Key_Function::Parameter_Function_Count];

int read_Function_Status(Function_Status* list) {
	std::lock_guard<std::mutex> lock(pDat_Function_Status_Mutex);
	memcpy(list,pDat_Function_Status,sizeof(pDat_Function_Status));
	return 0;
}

int write_Function_Status(Function_Status* list) {
	std::lock_guard<std::mutex> lock(pDat_Function_Status_Mutex);
	memcpy(pDat_Function_Status,list,sizeof(pDat_Function_Status));
	return 0;
}

/* Parameters */

std::mutex pDat_Parameter_Mutex;
Fractal_Data pDat_Fractal_Data;
Render_Data pDat_Primary_Render_Data;
Render_Data pDat_Secondary_Render_Data;

void read_Parameters(Fractal_Data* frac, Render_Data* primary, Render_Data* secondary) {
	std::lock_guard<std::mutex> lock(pDat_Parameter_Mutex);
	if (frac != NULL) { memcpy(frac,&pDat_Fractal_Data,sizeof(Fractal_Data)); }
	if (primary != NULL) { memcpy(primary,&pDat_Primary_Render_Data,sizeof(Render_Data)); }
	if (secondary != NULL) { memcpy(secondary,&pDat_Secondary_Render_Data,sizeof(Render_Data)); }
}

void write_Parameters(Fractal_Data* frac, Render_Data* primary, Render_Data* secondary) {
	std::lock_guard<std::mutex> lock(pDat_Parameter_Mutex);
	if (frac != NULL) { memcpy(&pDat_Fractal_Data,frac,sizeof(Fractal_Data)); }
	if (primary != NULL) { memcpy(&pDat_Primary_Render_Data,primary,sizeof(Render_Data)); }
	if (secondary != NULL) { memcpy(&pDat_Secondary_Render_Data,secondary,sizeof(Render_Data)); }
}


/* Render Buffers */

std::mutex pDat_Render_Buffers_Mutex;
BufferBox pDat_primary = {NULL,0,0,IMAGE_BUFFER_CHANNELS,0};
BufferBox pDat_secondary = {NULL,0,0,IMAGE_BUFFER_CHANNELS,0};
bool pDat_Render_Buffer_Read = false;

int read_Render_Buffers(BufferBox* primary) {
	if (primary == NULL) { // Nothing to do
		printError("BufferBox* primary is NULL in read_Render_Buffers");
		return -1;	
	}
	std::lock_guard<std::mutex> lock(pDat_Render_Buffers_Mutex);
	if (pDat_Render_Buffer_Read == false) {
		//printfInterval(0.3,"\npDat_Render_Buffer_Read == false");
		return 1;
	}
		primary->resX = pDat_primary.resX;
		primary->resY = pDat_primary.resY;
		primary->channels = pDat_primary.channels;
		primary->padding = pDat_primary.padding;
		FREE(primary->vram);
		size_t bufSize = getBufferBoxSize(&pDat_primary);
		primary->vram = (uint8_t*)malloc(bufSize);
		memcpy(primary->vram,pDat_primary.vram,bufSize);
	return 0;
}

/* Used to ensure buffers are not read during a swap */
int clear_Render_Buffers() {
	std::lock_guard<std::mutex> lock(pDat_Render_Buffers_Mutex);
	pDat_Render_Buffer_Read = false;
	return 0;
}

int write_Render_Buffers(BufferBox* primary) {
	std::lock_guard<std::mutex> lock(pDat_Render_Buffers_Mutex);
	if (primary == NULL) { // Nothing to do
		printError("BufferBox* primary is NULL in write_Render_Buffers()");
		return -1;	
	}
		pDat_primary.resX = primary->resX;
		pDat_primary.resY = primary->resY;
		pDat_primary.channels = primary->channels;
		pDat_primary.padding = primary->padding;
		FREE(pDat_primary.vram);
		size_t bufSize = getBufferBoxSize(&pDat_primary);
		pDat_primary.vram = (uint8_t*)malloc(bufSize);
		memcpy(pDat_primary.vram,primary->vram,bufSize);
		pDat_Render_Buffer_Read = true;
	return 0;
}

/* Image Buffer */

std::mutex pDat_Image_Buffers_Mutex;
ImageBuffer pDat_primaryImage = ImageBuffer(IMAGE_BUFFER_CHANNELS);
bool pDat_Image_Buffers_Read = false;

int clear_Image_Buffers() {
	std::lock_guard<std::mutex> lock(pDat_Image_Buffers_Mutex);
	pDat_primaryImage.clearBuffer();
	pDat_Image_Buffers_Read = false;
	return 0;
}

int read_Image_Buffers(ImageBuffer* primary) {
	if (primary == NULL) { // Nothing to do
		printError("ImageBuffer* primary is NULL in read_Render_Buffers");
		return -1;	
	}
	std::lock_guard<std::mutex> lock(pDat_Image_Buffers_Mutex);
	if (pDat_Image_Buffers_Read == false) {
		return 1;
	}
	primary->vramCopy(&pDat_primaryImage);
	primary->setTransformationData(&pDat_primaryImage);
	return 0;
}

int write_Image_Buffers(ImageBuffer* primary) {
	if (primary == NULL) { // Nothing to do
		printError("ImageBuffer* primary is NULL in write_Render_Buffers()");
		return -1;	
	}
	std::lock_guard<std::mutex> lock(pDat_Image_Buffers_Mutex);
	pDat_primaryImage.vramCopy(primary);
	pDat_primaryImage.setTransformationData(primary);
	pDat_Image_Buffers_Read = true;
	return 0;
}

/* Render Delta Time */

std::mutex pDat_Request_Mutex;
fp64 renderDelta;

void setRenderDelta(fp64 t) {
	std::lock_guard<std::mutex> lock(pDat_Request_Mutex);
	renderDelta = t;
}

fp64 getRenderDelta() {
	std::lock_guard<std::mutex> lock(pDat_Request_Mutex);
	return renderDelta;
}

/* Image Render */
	std::mutex pDat_Image_Render_Mutex;
	Fractal_Data pDat_Image_Render_Fractal;
	Render_Data pDat_Image_Render_Data;
	bool pDat_Image_Render_Ready = false;
	uint32_t pDat_Image_File_Format = 0;
	uint8_t pDat_Image_Quality = 8;

	void reset_Image_Render() {
		std::lock_guard<std::mutex> lock(pDat_Image_Render_Mutex);
		pDat_Image_Render_Ready = false;
		memset(&pDat_Image_Render_Fractal,0,sizeof(Fractal_Data));
		memset(&pDat_Image_Render_Data,0,sizeof(Render_Data));
	}

	void send_Image_Render(
		const Fractal_Data* frac, const Render_Data* super,
		uint32_t image_file_format, uint8_t image_quality
	) {
		if (frac == nullptr || super == nullptr) {
			return;
		}
		std::lock_guard<std::mutex> lock(pDat_Image_Render_Mutex);
		pDat_Image_Render_Ready = false;
		pDat_Image_Render_Fractal = *frac;
		pDat_Image_Render_Data = *super;
		pDat_Image_Render_Ready = true;
	}

	bool receive_Image_Render(
		Fractal_Data* frac, Render_Data* super, uint32_t* image_file_format, uint8_t* image_quality
	) {
		if (frac == nullptr || super == nullptr || image_file_format == nullptr || image_quality == nullptr) {
			return false;
		}
		std::lock_guard<std::mutex> lock(pDat_Image_Render_Mutex);
		if (pDat_Image_Render_Ready == false) {
			return false;
		}
		*frac = pDat_Image_Render_Fractal;
		*super = pDat_Image_Render_Data;
		*image_file_format = pDat_Image_File_Format;
		*image_quality = pDat_Image_Quality;
		pDat_Image_Render_Ready = false;
		return true;
	}