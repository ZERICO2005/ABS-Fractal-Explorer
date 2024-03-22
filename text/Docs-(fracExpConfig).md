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

# Parameter_Sensitivity
	Determines the speed that position, zoom, etc. will change by.
	* global: 1.000000
	global sensitivity adjusts all sensitivities by the same amount.
	* coordinate: 1.000000
	* zoom: 1.000000
	* invert_zoom: false
	* maxIter: 1.000000
	* julia: 1.000000
	* rotation: 1.000000
	* stretch: 1.000000
	* polar_power: 1.000000
	* breakout_value: 1.000000

# Display_Preferences
	## Display_Bootup_Type
	*(0 Automatic)*<br>
	Configures how the program should determine which display to open on. Doesn't have an effect if the system only has one monitor. 
	
	0. Automatic: Goes through a multi-step process to determine which display to use
		* If the current and previous `Display_Config_Hash` values match, the display will open on the `Previous_Display_Used`.
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
	
	## Bootup_Fullscreen (Not Implemented)
	*(false)*<br>
	Opens the application in windowed-fullscreen.
	
	## ScaleWindowToScreenSize
	*(true)*<br>
	When `true`, the initial window size will be determined dynamically from `Bootup_Window_Scale`. When `false`, the initial window size will be set to between the Default and Minimum window size.
	
	## Bootup_Window_Scale
	*(0.700)
	Determines how much of the display the window will initially take up if `ScaleWindowToScreenSize` is `true`.
	
	## Display_RefreshRate_Type
	*(0 Automatic)*
	Determines how the maximum refresh-rate is determined
	
	0. Automatic (Currently does `1 Current Display`)
	1. Current Display
	2. Highest Refresh-Rate
	3. Lowest Refresh-Rate
	4. Constant Value
	
	## Constant_RefreshRate_Value
	*(60.0)*
	If `Display_RefreshRate_Type` is set to `4 COnstant Value`, it will use the `Constant_RefreshRate_Value` for the maximum refresh-rate.
	
	## Maximum_FPS_Multiplier
	*(1)*
	If `Display_RefreshRate_Type` is *Not* set to `4 Constant Value`, the maximum refresh-rate will be multiplied by a value derived from `Maximum_FPS_Multiplier`
	
	`N == 0`: Multiplied by `1` 
	`N > 0`: Multiplied `(N + 1)` times
	`N < 0`: Multiplied `(1 / (-N + 1))` times

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
	2. Light Theme (Not fully supported, may cause white-on-white text)
	
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
	* 85-94: High quality
	* 60-84: Medium quality
	* 25-59: Low quality (Not recommended)
	