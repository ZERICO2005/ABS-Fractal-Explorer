/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "programData.h"

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

std::mutex pDat_Buffer_Size_Mutex;
BufferBox pDat_BufSize = {NULL,0,0,3,0};
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


std::mutex pDat_Render_Buffers_Mutex;
BufferBox pDat_primary = {NULL,0,0,3,0};
BufferBox pDat_secondary = {NULL,0,0,3,0};
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

/* Request */
/*
std::mutex pDat_Request_Mutex;

bool pDat_Buffer_Ready = false;

BufferBox pDat_BufCurrent;
BufferBox pDat_Buf0;

int receive_Buffer(BufferBox* prev, BufferBox* next) {
	if (next == NULL) {
		printError("BufferBox* next is NULL in receive_buffer()");
		return -1;
	}
	std::lock_guard<std::mutex> lock(pDat_Request_Mutex);
	if (pDat_Buffer_Ready == false) {
		FREE(prev->vram);
		next->vram = (uint8_t*)malloc(getBufferBoxSize(next));
		return 0;
	}
	pDat_Buffer_Ready = false;
	return 0;
}

int send_Buffer(BufferBox* buf) {
	if (buf == NULL) {
		printError("BufferBox* buf is NULL in send_buffer()");
		return -1;
	}
	std::lock_guard<std::mutex> lock(pDat_Request_Mutex);

	pDat_Buffer_Ready = true;
}

bool next_Buffer() {
	std::lock_guard<std::mutex> lock(pDat_Request_Mutex);
	pDat_Buffer_Ready = true;
}
*/

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