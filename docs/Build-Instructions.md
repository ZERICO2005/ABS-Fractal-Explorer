Build Instructions<br>
ABS-Fractal-Explorer v1.2.2

Updated: 2024/08/09

You can read this guide:
https://github.com/ZERICO2005/ABS-Fractal-Explorer/wiki/How-to-Compile

# Build Configuration

Depending on your platform/compiler, you may enable/disable these features in CMakeLists.txt

## Float80
Enables 80bit floats (`long double` or `__float80`) for extra precision. Used to store cordinates if it is the highest available precision.

## Float128
Enables 128bit floats (quadmath.h) for extra precision. Used to store cordinates if it is the highest available precision.

## libqd
(Deprecated) Experimental double-double/quad-double float support. May require some manual configuration to enable it as a rendering mode.

## OpenCL
Enables OpenCL GPU rendering

## MPFR
Used to convert between coordinates and strings

## WindowsFileIO
(Not implemented in v1.2.1 rev-2)<br>
Uses the Windows file dialogs instead of the generic ones

## OpenCV_Scaler
Uses OpenCV to scale the current frame instead of the default implementation. This allows for faster and higher quality scaling, along with other interpolation methods such as linear, cubic, and lanczos.

## Static_OpenCV_Scaler
Statically links some OpenCV dependencies. You will need to compile these yourself from the OpenCV GitHub Repository. Replace the version numbers in CMakeLists.txt if needed.

# Optimizations
If you want to make ABS-Fractal-Explorer faster, you can try these:
* Using `-Ofast` instead of `-O3` can make CPU rendering faster. However, `-Ofast` may be unstable, or lead to unexpected behaviour. `-Ofast` will probably be the only noticable increase in performance.
* Hardcoding formulas can speed up rendering to a point.
* Changing the OpenCL compiler flags.
* Adding `-march=native` optimizes and tailors the program to run specifically for CPU and its instruction sets. This may cause ABS-Fractal-Explorer to not run on other computers.
* Link time optimization (`-flto`) reduces the size of the code slighty and significantly increases compile time. `-flto` might make the code slightly faster, although it would mostly apply to function/library calls.
* Stripping the exe/binary probably won't impact the performance of the code at all, but it will reduce the size of the code by removing debugging information.