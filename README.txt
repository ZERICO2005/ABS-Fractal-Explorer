ABS Fractal Explorer
v1.0.2 | 2023/08/17 | zerico2005

Description:
	ABS Fractal Explorer will allow you to explore hundereds of different Mandelbrot variants.
	Some extra features include changing the starting Z values, along with viewing each fractals Julia set.
	Software is available for Windows and Linux
	More features, preformance improvements, and bug fixes will be released in later versions.

Controls:
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
	Cycling:
		[/]: Decrement/Increment by Formula ID
		-/+: Decrement/Increment by Fractal Family
	Rendering:
		M: GPU Rendering (Fastest)
		N: CPU Full Multithreading (Zoom further)
		B: CPU Half Multithreading (Slowest, Not Recommended) (Limits the amount of threads used to render fractals)
		
What are Fractal Families?
	A Fractal Family refers to fractals with the exact same ABS functions but different signage.
	The lower bits of the Formula ID control signage flips, while the higher bits control ABS Functions. Fractals that differ by only signage flips typically have similiar properties or may be rotational duplicates of on another.
		Quadratic fractals have 3 possible signage flips, forming a family size of 8.
		Cubic fractals have 6 signage flips forming a family of 64.
		Quartics have 7 signage flips forming a family of 128.
		Quintics have 8 possible signage flips, forming a family size of 256. 
		
Quadratic ABS Fractals:
	- Because there is only 256 Quadratic fractals, they have been much easier to study and observe for patterns.
	- Each Quadratic Fractal family has a size of 8, and the family can either be asymmetrical or symmetrical.
		- Asymmetrical families have 2 unique fractals, each with 4 rotated duplicates included inside the family.
		- Symmetrical families have 4 unique fractals, the mirrored duplicate of each is also included within the family.
	- The Quadratic Fractals contain 42 asymmetrical, and 36 symmetrical fractals. Doing the math (42A*4 + 36S*2) = 240, so are we missing 16 fractals?
		- An interesting quirk is that 40 asymmetrical, and 32 symmetrical fractals have only one copy of their rotational duplicates, while 2 asymmetrical and 4 symmetrical fractals have two copies of their rotational duplicates. (40A*4 + 32S*2) + (2A*8 + 4S*4) = 256
		- This anomaly arrises because (zr*zr + zi*zi) will always be positive, and adding an ABS function outside won't change anything. The unnecessary ABS therefore doubles the times that 6 different fractals appear.
	- Some fractals contain other Quadratic fractals inside of them. Here are some methods for spotting them.
		- Fractals can easily be found on the needle, they may even have the exact same coordinates as the Minibrots on the Mandelbrot set.
		- Parts of a fractal with a endless stream of bulbs converging towards the Imaginary or Real line is another way to look for other fractals.
			- You will have to go pretty close to the axis lines to get a good image of the fractal. If you are far out, they could look distorted, or might not exist.
				- On some fractals I noticed a line of fractals protruding from the bulb. Going closer to the axis lines showed more fractals in a line.
		- Using "dot patterns" is the other method that one can use to find additional fractals. The interior coloring is based off how close a point got to zero, and typically, the points that got pretty close to zero are the center of the Fractals, and the center of the bulbs protruding out of them.
			- To search for dot patterns, lower your iteration count, and look for some dots. If you see dots, turn up your iteration count.
				- If the dark spots get darker/sharper, then its likely convering towards the center of a fractal. If you see more dots, you may be able to recongnize the type of fractal you are approaching.
				- If they dark spots don't start to dim or sharpen, and stay as more of a slighty dark blob, then it could mean a fractal doesn't exist there. Turning up the iterations may confirm that the point has escaped.
Notes:
	- This UI uses some of my own Graphics libraries, which are pretty limited. In the future, I may use IMGUI or other libraries to make interacting/controling the software much easier.
	- The Quadratic fractal ID's are NOT related to the fractals shown in "75 Mandelbrot Variants". Please consult the conversion table included in the software package to get their Quadratic ID number.
	- If you would like a challenge, try finding how many Quadratic fractals are contained in a singular Quadratic Fractal.
		- There is 4 fractals contained within the "Anchor" fractal, can you find more? https://youtu.be/gu1QfON0zxg

System Requirements:
	- GPU capable of running OpenCL 1.2 (Most computers from 2012 or later)
	- 64bit x86 CPU
	- 640x480 minimum screen resolution
	- Program uses ~130megabytes of RAM

Attributions:
	- The 6x8 pixel font used is a slightly modified version of "even" font by Christian Munk (CMunk) from https://fontstruct.com/fontstructions/show/275268/even_2.
	The "even" font is lincesed under Attribution-ShareAlike 3.0 Unported (CC BY-SA 3.0)

ABS Fractal Videos:
	- 75 Mandelbrot Variants: https://youtu.be/HeUpSzC1vzU
	- 330 Cubic Fractals: https://youtu.be/NDi_4DVy8-E
	- 5265 Quartic Fractals [Demo Video]: https://youtu.be/pGSlsJEHIpA