ABS-Fractal-Explorer v1.1.0 alpha
zerico2005 | 2023/12/28

This version of ABS-Fractal-Explorer is a rewrite, which has fixed several flaws present in v1.0.9 and earlier versions. Some features have not been reimplemented yet, and will be added one at a time to prevent future issues from arising.

ABS-Fractal-Explorer v1.0.2 can be found under a different branch: https://github.com/ZERICO2005/ABS-Fractal-Explorer/tree/v1.0.2

Controls (Full controls/keybinds are in the Key-binds Menu):
	WASD: Move/Pan
	EQ: Zoom in/out
	R: Resets coordinates
	TG: Inc/dec iterations
	P: Takes screenshot
	<>: Inc/dec subsampling (Renders faster but at a lower resolution)
	[]: Inc/dec fractal formula ID
	M: fp32 GPU rendering
	N: fp64 CPU rendering

Attributions:
- The 6x8 pixel font used in the program is a slightly modified version of the "even" font created by Christian Munk (CMunk), which can be found at https://fontstruct.com/fontstructions/show/275268/even_2. The "even" font is lincesed under Attribution-ShareAlike 3.0 Unported (CC BY-SA 3.0). Original License URL: https://creativecommons.org/licenses/by-sa/3.0/

Compiliation Setup used:
	- Windows 10 64bit x86
	- MSYS2 MinGW64
	- GCC/G++ CMake Ninja
	
You may also need to copy some Dear ImGui files into ./imgui to compile the code. Additionally, changing #include<SDL.h> to #include<SDL2/SDL.h> could help if things are not working correctly.

Once the code is compiled, go into the build directory and run:
bin/ABS-Fractal-Explorer.exe "../text/Example-Key-Binds.fracExpKB"

If you need to quit rendering, there are two ways to do it:
	- clicking on the "Abort Rendering" button, which will tell all CPU threads to stop rendering;
	- or, if you are rendering on the GPU or you are rendering with a ridiculous amount of threads, then the best way to quit rendering is to kill the task in task manager (or wait for the rendering to finish).
Rendering won't resume until the "Abort Rendering" button is clicked again.

New change in v1.0.16: (2023/11/01)
.fracExpKB files are working close to their final form. FracExp_KeyBind files are designed to be "future proof" and human readable, and can be edited in a text editor* (ANSI encoding, ASCII characters, UNIX line endings, Tab character for tabs).

List of some of the changes in ABS-Fractal-Explorer v1.0.10: (2023/10/27)

	Proper User Interface:
		ABS-Fractal-Explorer now uses Dear ImGui for its GUI.

	Asynchronous threading: Multiple asynchronous threads will be used to accomplish various tasks:
		- Handeling UI/IO
		- Compiling OpenCL
		- Handeling Rendering Jobs
		- Rendering realtime/low quality fractal frames
		- Rendering high quality fractal frames
		- Saving the image from a "super" screenshot (Which can take awhile for a 30720x23040 image)

	Thread Yeilding:
		If a thread finishes early or isn't busy, it will yeild its timeslot to other tasks.
		
	CMake:
		I have switched from using Makefiles to CMake
