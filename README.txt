ABS-Fractal-Explorer v1.0.16 debug pre-alpha
zerico2005 | 2023/11/01

This version of ABS-Fractal-Explorer is a rewrite, which has fixed several flaws present in v1.0.9 and earlier versions. Some features have not been reimplemented yet, and will be added one at a time to prevent future issues from arising.

Attributions:
- The 6x8 pixel font used in the program is a slightly modified version of the "even" font created by Christian Munk (CMunk), which can be found at https://fontstruct.com/fontstructions/show/275268/even_2. The "even" font is lincesed under Attribution-ShareAlike 3.0 Unported (CC BY-SA 3.0). Original License URL: https://creativecommons.org/licenses/by-sa/3.0/

Compiliation Setup used:
	- Windows 10 64bit x86
	- MSYS2 MinGW64
	- GCC/G++ CMake Ninja
	
You may also need to copy some Dear ImGui files into ./imgui to compile the code. Additionally, changing #include<SDL.h> to #include<SDL2/SDL.h> could help if things are not working correctly.

Once the code is compiled, go into the build directory and run:
bin/ABS-Fractal-Explorer.exe "../text/Example-Key-Binds.fracExpKB"

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