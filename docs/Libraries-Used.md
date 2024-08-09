ABS-Fractal-Explorer v1.2.0 (2024/05/08)

Different versions of ABS-Fractal-Explorer were made using the following libraries and tools

# Build Enviroments

## MSYS2 MinGW64 CMake GCC/Clang
https://www.msys2.org/
The build enviroment used to compile ABS-Fractal-Explorer for Windows 10. If you want to compile ABS-Fractal-Explorer yourself, you can follow this guide https://github.com/ZERICO2005/ABS-Fractal-Explorer/wiki/How-to-Compile

## MSYS.bat MinGW GCC
https://osdn.net/projects/mingw
The build enviroment used to compile ABS-Fractal-Explorer for 32bit Windows Vista. If you want to compile ABS-Fractal-Explorer yourself on legacy versions of Windows, you can follow this guide https://github.com/ZERICO2005/ABS-Fractal-Explorer/wiki/How-to-Compile-(Windows-Vista-32bit)

## Ubuntu Linux
https://ubuntu.com/
The terminal on Ubuntu Linux is used to compile ABS-Fractal-Explorer for Linux

# Text/Code Editors

## Notepad++
https://notepad-plus-plus.org/
A great portable text editor with support for syntax highlighting

## VS-Code
https://code.visualstudio.com/
Used for writing code

# Other Tools

## Git-Bash
https://git-scm.com/downloads
Used for version control and for tracking changes to ABS-Fractal-Explorer

# Libraries

## SDL2
https://www.libsdl.org/
Used to display the window, handling the keyboard, and other I/O

## Dear ImGui
https://github.com/ocornut/imgui
Used for the Graphical User Interface (GUI)

## OpenCL
https://www.khronos.org/opencl/
Used for GPU rendering

## QuadMath
https://gcc.gnu.org/onlinedocs/libquadmath/
Used for high precision 128bit float calculations

## MPFR
Used for converting between coordinates and strings with greater accuracy.

## libQD
https://www.davidhbailey.com/dhbsoftware/
Used for experimental quad-double and double-double arithmetic support.

## LIB-Dekker-Float
https://github.com/ZERICO2005/LIB-Dekker-Float
A header-only library I wrote for the Dekker float implementations used in ABS-Fractal-Explorer. Float64x2 is roughly the same speed as dd_real in libQD, while Float64x2 AVX is ~3.3x faster for rendering fractals.

## STB Image
https://github.com/nothings/stb
Used for writing PNG/JPG images

## OpenCV
https://opencv.org/
Used for frame scaling in some versions of ABS-Fractal-Explorer

