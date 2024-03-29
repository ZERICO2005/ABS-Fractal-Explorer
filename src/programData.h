/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef PROGRAMDATA_H
#define PROGRAMDATA_H

#include "Common_Def.h"
#include "Program_Def.h"

#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>

#include "engine.h"
#include "render.h"
#include "fractal.h"
#include "copyBuffer.h"
#include "imageBuffer.h"

/*
** Handling shared data between threads
*/

/* Thread Ready */
	bool read_Render_Ready();
	void write_Render_Ready(bool f);
	bool read_Engine_Ready();
	void write_Engine_Ready(bool f);

struct Render_Item {
	int priority;
	int group;
	uint64_t time_sent;
	Render_Data render_data;
	Fractal_Data fractal_data;
}; typedef Render_Item Render_Item;

enum Render_Queue_Enum { Queue_Primary, Queue_Secondary, Queue_Count};

struct Render_Task {
	int priority;
	int group;
	uint64_t time_sent;
	Render_Data render_data;
	Fractal_Data fractal_data;
}; typedef Render_Task Render_Task;

/* Update Level */

	namespace Change_Level {
		enum Change_Level_Enum {
			Nothing, // Image buffer is perfect
			Refresh, // Image buffer is accurate
				Translation,Zoom,Stretch,Rotation,Resolution,
			Minor_Reset, // Image buffer is an approximatation
				Jump,Method_of_Rendering,Super_Sample,Breakout,Iterations,
			Moderate_Reset, // Image buffer is outdated
				Coloring,Polar_Power,Julia,	
			Major_Reset, // Image buffer is wrong
				Formula_Change,Power_Change,Fractal_Change,Recompile,
			Full_Reset, // Restart required
			Change_Level_Count
		};
	};

	// Used to deterimine if rendering should pause, continue, or reset
	int read_Update_Level();
	uint64_t read_Update_Timecode();
	// Stores the highest update level
	void write_Update_Level(int level);
	// Used when render is complete
	void clear_Update_Level();

/* Cycle Buffer */
	enum Cycle_Buffer_Enum {Primary_Full, Primary_Preview, Secondary_Full, Secondary_Preview, Cycle_Buffer_Count};
	int next_Read_Cycle_Pos(ImageBuffer** ptr, int buf);
	int next_Write_Cycle_Pos(ImageBuffer** ptr, int buf);
	void init_Cycle_Buffers();
	void delete_Cycle_Buffers();
	void trim_Cycle_Buffers();
	void clear_Cycle_Buffers();

	void write_Buffer_Size(BufferBox size);
	BufferBox read_Buffer_Size();

bool read_Abort_Render_Ongoing();
void write_Abort_Render_Ongoing(bool s);

struct Function_Status {
	bool triggered;
	nano64_t resetTime;
	nano64_t freqTime;
}; typedef struct Function_Status Function_Status;

int read_Function_Status(Function_Status* list);
int write_Function_Status(Function_Status* list);

void read_Parameters(Fractal_Data* frac, Render_Data* primary, Render_Data* secondary);
void write_Parameters(Fractal_Data* frac, Render_Data* primary, Render_Data* secondary);

/* Used to ensure buffers are not read during a swap */
int clear_Render_Buffers();
int read_Render_Buffers(BufferBox* primary);
int write_Render_Buffers(BufferBox* primary);

int clear_Image_Buffers();
int read_Image_Buffers(ImageBuffer* primary);
int write_Image_Buffers(ImageBuffer* primary);

void setRenderDelta(fp64 t);
fp64 getRenderDelta();

/* Image Render */
	void reset_Image_Render();
	void send_Image_Render(
		const Fractal_Data* frac, const Render_Data* super,
		uint32_t image_file_format, uint8_t image_quality
	);
	bool receive_Image_Render(
		Fractal_Data* frac, Render_Data* super,
		uint32_t* image_file_format, uint8_t* image_quality
	);

#endif /* PROGRAMDATA_H */