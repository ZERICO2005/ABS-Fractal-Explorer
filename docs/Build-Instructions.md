Build Instructions<br>
ABS-Fractal-Explorer v1.2.1 rev-2

Updated: 2024/06/09

You can read this guide:
https://github.com/ZERICO2005/ABS-Fractal-Explorer/wiki/How-to-Compile

# Build Configuration

Depending on your platform/compiler, you may enable/disable these features in CMakeLists.txt

## Float80
Enables 80bit floats (`long double` or `__float80`) for extra precision. Used to store cordinates if it is the highest available precision.

## Float128
Enables 128bit floats (quadmath.h) for extra precision. Used to store cordinates if it is the highest available precision.

## OpenCL
Enables OpenCL GPU rendering

## WindowsFileIO
(Not implemented in v1.2.1 rev-2)<br>
Uses the Windows file dialogs instead of the generic ones

## OpenCV_Scaler
Uses OpenCV to scale the current frame instead of the default implementation. This allows for faster and higher quality scaling, along with other interpolation methods such as linear, cubic, and lanczos.

## Static_OpenCV_Scaler
Statically links some OpenCV dependencies. You will need to compile these yourself from the OpenCV GitHub Repository. Replace the version numbers in CMakeLists.txt if needed.