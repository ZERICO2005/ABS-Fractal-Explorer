ABS-Fractal-Explorer v1.0.10 debug pre-alpha
zerico2005 | 2023/10/27

This version of ABS-Fractal-Explorer is a rewrite, which has fixed several flaws present in v1.0.9 and earlier versions. Some features have not been reimplemented yet, and will be added one at a time to prevent future issues from arising.

Compiliation Setup used:
	- Windows 10 64bit x86
	- MSYS2 MinGW64
	- GCC/G++ CMake Ninja



List of some of the changes in ABS-Fractal-Explorer v1.0.10:

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