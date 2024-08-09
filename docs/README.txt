ABS-Fractal-Explorer v1.2.2 (Windows 10 Edition)
zerico2005 | 2024/08/09

ABS-Fractal-Explorer will allow you to explore hundereds of different Mandelbrot variants. From Quadratic to Sextic fractals, there is always something new and novel to explore!

Some of	its features include rendering Julia-Sets, super-sampled images, key-bind editors, and advanced transformations to undistort previously unexplorable areas.

If you have an idea for a new feature, tell us about it at the link below:
https://forms.gle/x8tgwVbP3eyHTpi2A



ABS-Fractal-Explorer can be downloaded from Itch.io and GitHub. 
	Itch.io: https://zerico2005.itch.io/abs-fractal-explorer
	GitHub: https://github.com/ZERICO2005/ABS-Fractal-Explorer/releases

Controls (Full controls/keybinds are in the Key-binds Menu):
	Basic Controls:
		WASD: Move/Pan
		EQ: Zoom in/out
		R : Resets coordinates
		TG: Inc/dec iterations
		P : Screenshot
	Fractals:
		[]: Inc/dec fractal formula ID
		+-: Inc/dec fractal family
		L-Ctrl/L-Alt: Change fractal power
	Rendering:
		<>: Inc/dec subsampling (Renders faster but at a lower resolution)
		;": Inc/dec supersampling (Renders at higher quality but is much slower)
		M : fp32 GPU rendering (10^5.7 max zoom, fastest)
		N : fp64 CPU rendering (10^14.4 max zoom)
		B : fp80 CPU rendering (10^17.8 max zoom)
		Rctrl: fp64x2 CPU rendering (10^30.7 max zoom)
		View the rendering menu for more precision options.
	Julia-Sets:
		C: Toggle Julia Sets
		V: Toggle Starting Z-Value
		IJKL: Move/Change Julia Cordinate
		UO: Reset Real/Imag Julia Cordinate
	Transformations:
		Arrow-Keys: Rotation/Stretch
		F: Reset Transformations 



System Requirements:
	- Windows 10 64bit
	- OpenCL 1.2 support (Most computers from 2012 or later)
	- Windows 7 64bit might work, but I have no way to test this.

ABS-Fractal-Explorer is also available for Linux and 32bit Windows Vista.

If you encounter any bugs or issues, you can read through "basic-trouble-shooting.txt", or fill out a bug report at the link below:
https://forms.gle/zqmkkH8ESY1bhCyw9

You can checkout the source code on GitHub https://github.com/ZERICO2005/ABS-Fractal-Explorer.



If you need to quit/cancel rendering, there are two ways to do it:
	- clicking on the "Abort Rendering" button, which will tell all CPU threads to stop rendering;
	- or, if you are rendering on the GPU, or you are rendering with a ridiculous amount of threads, then the best way to quit rendering is to kill the task in task manager (or wait for the rendering to finish).
Rendering won't resume until the "Abort Rendering" button is clicked again.



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
	- The Quadratic fractal ID's are NOT related to the fractals shown in "75 Mandelbrot Variants". Please consult the conversion table included in the software package to get their Quadratic ID number.
	- If you would like a challenge, try finding how many Quadratic fractals are contained in a singular Quadratic Fractal.
		- There is 4 fractals contained within the "Anchor" fractal, can you find more? https://youtu.be/gu1QfON0zxg

Attributions:
	- The 6x8 pixel font used in the program is a slightly modified version of the "even" font created by Christian Munk (CMunk), which can be found at https://fontstruct.com/fontstructions/show/275268/even_2. The "even" font is lincesed under Attribution-ShareAlike 3.0 Unported (CC BY-SA 3.0). Original License URL: https://creativecommons.org/licenses/by-sa/3.0/
	- Checkout Libraries-Used.md for the list of the libraries and programs used to make ABS-Fractal-Explorer

ABS Fractal Videos:
	- 75 Mandelbrot Variants: https://youtu.be/HeUpSzC1vzU
	- 330 Cubic Fractals: https://youtu.be/NDi_4DVy8-E
	- 5265 Quartic Fractals [Demo Video]: https://youtu.be/pGSlsJEHIpA