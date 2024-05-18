# ABS-Fractal-Explorer Config File
`fracExpConfig` files are intended to store user preferances. `fracExp` files are intended for storing/sharing fractals, and `fracExpKB` files are used for storing keybinds.

# Version
	Contains the version (Major, Minor, Patch, Tags, Release Date, etc.) of ABS-Fractal-Explorer that the `fracExpConfig` file was exported/saved from. Also contains the `Export_Time`, which states the time the config file was saved at.
	
	Differing Major/Minor versions may not be compatible with each other as fields may be added/removed/changed. Differing Patch versions *should* be compatible with each other.

# Automatic_Behaviour

	## AutoLoad_Config_File 
	*(true)*<br>
	Automatically imports `./config.fracExpConfig`. If the file does not exist or this is set to `false`, the default configuration will be used.
		
	## AutoSave_Config_File
	*(true)*<br>
	Automatically exports configurations to `./config.fracExpConfig`

# File_Paths
	Absoulte paths to files/folders. These paths will not be used if ABS-Fractal-Explorer detects that it is being run from a different computer/system.
	
	## Path_Screenshots
		The directory/folder that screenshots will be written to.
		
	## Path_FracExpKB
		(Unimplemented)<br>
		The location of the file where key-binds will be loaded from at startup.

# Parameter_Sensitivity
	Determines the speed that position, zoom, etc. will change by.
	* global: 1.00
	global sensitivity adjusts all sensitivities by the same amount.
	* coordinate: 1.00
	* zoom: 1.00
	* invert_zoom: false
	* maxIter: 1.00
	* julia: 1.00
	* rotation: 1.00
	* stretch: 1.00
	* polar_power: 1.00
	* breakout_value: 1.00

# Display_Preferences
	## Display_Bootup_Type
	*(0 Automatic)*<br>
	Configures how the program should determine which display to open on. Doesn't have an effect if the system only has one monitor. 
	
	0. Automatic: Goes through a multi-step process to determine which display to use
		* (Skipped due to some bugs/issues) If the current and previous `Display_Config_Hash` values match, the display will open on the `Previous_Display_Used`.
		* If the current and previous `Display_Config_Hash` do **not** match, or the `Previous_Display_Used` is not set, the display will open on the `Cursors Position`.
		* If the `Cursor Position` display is invalid, the `Primary Display` will be used.
		* If the `Primary Display` is also invalid, then the first valid display will be used.
		* If none of the displays are valid, then the display with the highest resolution will be used.
	1. Cursor Position
	2. Primary Display
	2. First Display
	3. Last Display
	4. Specific Display (Opens on a specific display as defined by `Specific_Bootup_Display`, or the `Last Display` if it is not available)
	
	Directional Based: (Calculated from the center cordinate of a display)
	5. Left
	6. Right
	7. Center
	8. Top
	9. Bottom
	10. Top-Left
	11. Top-Right
	12. Bottom-Left
	13. Bottom-Right
	
	Attribute Based:
	14. Highest Resolution
	15. Highest Framerate
	16. Lowest Resolution
	17. Lowest Framerate
	18. Widest Aspect Ratio
	19. Tallest Aspect Ratio
	
	## Previous_Display_Used
	Stores which display the center of window was on when the application was closed.
	
	## Display_Config_Hash
	Generates a unique hash value for your display configuration. When `Display_Bootup_Type` is set to  `0 Automatic`, this hash value is used to detect if the display configuration has changed.
	
	## Specific_Bootup_Display
	Determines what display the program will open on if Display_Bootup_Type is set to `Specific Display`.
	
	## Bootup_Fullscreen
	*(0 Windowed)*<br>
	Changes how the application will open.
	
	0. Windowed: A resizable and moveable window that can be moved around.
	1. Windowed-Fullscreen: The window will take up the entire screen area.
	
	Fullscreen and/or Borderless may be added later on.
	
	## ScaleWindowToScreenSize
	*(true)*<br>
	When `true`, the initial window size will be determined dynamically from `Bootup_Window_Scale`. When `false`, the initial window size will be set to between the Default and Minimum window size.
	
	## Bootup_Window_Scale
	*(0.700)*<br>
	Determines how much of the display the window will initially take up if `ScaleWindowToScreenSize` is `true`.
	
	## Display_RefreshRate_Type
	*(0 Automatic)*<br>
	Determines how the maximum refresh-rate is determined
	
	0. Automatic (Currently does `1 Current Display`)
	1. Current Display
	2. Highest Refresh-Rate
	3. Lowest Refresh-Rate
	4. Constant Value
	
	## Automatically_Update_RefreshRate (Not Implemented)
	*(True)*<br>
	If Display_RefreshRate_Type is set to `0 Automatic` or `1 Current Display`, the refresh-rate will automatically update when the window is moved to another monitor.
	
	## Constant_RefreshRate_Value
	*(60.0)*<br>
	If `Display_RefreshRate_Type` is set to `4 Constant Value`, it will use the `Constant_RefreshRate_Value` for the maximum refresh-rate.
	
	## Maximum_FPS_Multiplier
	*(1)*<br>
	If `Display_RefreshRate_Type` is *Not* set to `4 Constant Value`, the maximum refresh-rate will be multiplied by a value derived from `Maximum_FPS_Multiplier`
	
	`N == 0`: Multiplied by `1`<br>
	`N > 0`: Multiplied `(N + 1)` times<br>
	`N < 0`: Multiplied `(1 / (-N + 1))` times<br>

# GUI_Settings
	## LockKeyInputsInMenus
	*(true)*<br>
	Prevents key inputs from moving the fractal around while in menus.
	
	## AutoResizeWindows
	*(false)*<br>
	Automatically scales window size to `WindowAutoScale`.
	
	## PreventOutOfBoundsWindows
	*(false)*<br>
	Ensures that the window never goes out of bounds.
	
	## GUI_Theme:
	*(1 Dark Theme)*<br>
	Sets the ImGui Theme.
	
	0. Classic Theme
	1. Dark Theme
	2. Light Theme
	
	## WindowOpacity:
	*(0.950)*<br>
	Makes the background of the window transparent, but not the text.
	
	## WindowAutoScale
	*(0.700)*<br>
	Used for the default window size/scale. Used for automatically scaling the window `AutoResizeWindows` is `true`, 

# Screenshot_Settings
	## screenshotFileType
	*(0 PNG)*<br>
	Sets the file format screenshots will be saved in.
	
	0. PNG (Recommended)
	1. JPG/JPEG
	2. TGA
	3. BMP (Not recommended)
	
	HDR images might be supported in the future.
	
	## PNG_Compression_Level
	*(8)*<br>
	PNG images use *lossless* compression, using higher compression levels yeilds smaller file sizes.
	
	* Level 9: Smallest file-size
	* Level 7-8: Smaller file-size (Recommended)
	* Level 5-6: Balanced
	* Level 3-4: Faster saving
	* Level 1-2: Fastest saving (Large file-size)
	
	## JPG_Quality_Level
	*(95)*<br>
	JPG/JPEG images use *lossy* compression, which allows them to have a smaller file size than PNG images at the expense of image quality.
	
	With a quality level of 95-100, JPG/JPEG images can take up only 25-60% the size of a PNG image. It is strongly advised that you use quality level 90 or above to reduce compression artifacts in fractal images.
	
	* 95-100: Very high quality (Recommended)
	* 90-94: High quality
	* 75-84: Medium quality
	* 25-74: Low quality (Not recommended)

# Rendering_Settings
	## Hardware_Hash
	Generates a unique hash value for your hardware configuration. This hash is used to detect any changes to hardware, which can help prevent one's slow laptop from attempting to render at the same presets as a powerful desktop computer. If changes to the hardware configuration are detected, you will be prompted with a choice of loading the `Rendering_Settings` from the config file (Which may or may not perform well on another computer), or loading the default `Rendering_Settings`, which are designed to be able to run fine on most computers.
	
	## Frame_Interpolation_Method
	*(0 Nearest Neighbor)*<br>
	Only applicable if ABS-Fractal-Explorer is using the OpenCV scaler.
	
	0. Nearest Neighbor: (Fastest) Doesn't blend/smear pixels but may look pixelated.
	1. Linear: (Fast) Provides a smoother transition between pixels.
	2. Bicubic: (Slow) Provides a higher quality transition between pixels.
	3. Area Interpolation: (Fast) Supposedly the same as Nearest Neighbor when zoomed in, but does a nicer job when zoomed out.
	4. Lanczos4: (Slowest) Uses an advanced algorithm to interpolate between pixels.
	
	## Image_Render_Bounding_Box
	*(0 Fill Area)*<br>
	The area that is used for taking a super-screenshot or high quality render if the aspect ratio's of the view-port and super-screenshot area do not match.
	
	0. Fill Area: Ensures that the entirety of the view-port is visible in the super-screenshot, but may include areas outside the view-port.
	1. Fit Area: Ensures that the super-screenshot area lies entirely inside the view-port, this may exclude some parts of the view-port.
	2. Fit Width: Ensures the left and right sides of the view-port are included in the super-screenshot, this may crop or expand the top and bottom sides of the view-port.
	3. Fit Height: Ensures the top and bottom sides of the view-port are included in the super-screenshot, this may crop or expand the left and right sides of the view-port.
	4. Expand from Center: The center 1:1 square of the view-port will remain visible, but pixels may be cropped or expanded on the left/right and top/bottom sides of the super-screenshot.
	5. Stretch to Fit (Not Implemented): Stretches the super-screenshot to cover the exact same area as the view-port, this may distort the image.
	
	## JuliaPoint_Enabled
	*(True)*<br>
	Plots the current Julia-Cordinate point on screen.
	
	## JuliaPoint_OuterRadius
	*(8.0)*
	The outer-radius of the Julia-Cordinate point. Larger values can make the Julia-Cordinate easier to spot.
	
	## JuliaPoint_InnerRadius
	*(2.4)*
	The inner-radius of the Julia-Cordinate point, used to create a crosshair or "donut" shape. You can disable the inner-radius by setting it to 0.0