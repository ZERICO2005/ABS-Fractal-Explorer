ABS-Fractal-Explorer
v1.0.1 | 2023/08/15 | zerico2005

Description:
	ABS Fractal Explorer will allow you to explore hundreds of different Mandelbrot variants.
	Some extra features include changing the starting Z values, along with viewing each fractals Julia set.
	More features, performance improvements, and bug fixes will be released in later versions.

Controls: (Also see Keybinds.png)
	Primary:
		WASD/Arrows: Panning/Moving
		E/Q: Zooming In/Out
		T/G: Increase/Decrease Maximum Iterations
		R: Reset Cordinates and Zoom
	Secondary:
		J/L: Change starting Z.real value
		I/K: Change starting Z.imag value
		U/O: Reset Z.real/Z.imag to 0.0
	Fractal:
		Z: Change Fractal ID (0-1048575) (Type in number then press Enter)
		X: Change Fractal Type (2-5) (Quadratic-Quintic)
		C: View Julia Set
		V: Toggle starting Z value for Mandelbrot set
		P: Take a screenshot (To the directory the EXE is located in)
	Rendering:
		M: GPU Rendering (Fastest)
		N: CPU Full Multithreading (Zoom further)
		B: CPU Half Multithreading (Slowest, Not Recommended) (Limits the amount of threads used to render fractals)

Notes:
	- This UI uses some of my own Graphics libraries, which are pretty limited. In the future, I may use IMGUI or other libraries to make interacting/controling the software much easier.
	- The Quadratic fractal ID's are NOT related to the fractals shown in "75 Mandelbrot Variants". Please consult the conversion table included in the software package to get their Quadratic ID number.
	- If you would like a challenge, try finding how many Quadratic fractals are contained in a singular Quadratic Fractal.
		- There is 4 fractals contained within the "Anchor" fractal, can you find more? https://youtu.be/gu1QfON0zxg

System Requirements:
	-	GPU capable of running OpenCL 1.2 (Most computers from 2012 or later)
	-	64bit x86 CPU
	-	640x400 minimum screen resolution
	-	Program uses ~130megabytes of RAM

The 6x8 pixel font used is a slightly modified version of "even" font by Christian Munk (CMunk) from https://fontstruct.com/fontstructions/show/275268/even_2.
The "even" font is lincesed under Attribution-ShareAlike 3.0 Unported (CC BY-SA 3.0)

ABS Fractal Videos:
	- 75 Mandelbrot Variants: https://youtu.be/HeUpSzC1vzU
	- 330 Cubic Fractals: https://youtu.be/NDi_4DVy8-E
	- 5265 Quartic Fractals [Demo Video]: https://youtu.be/pGSlsJEHIpA