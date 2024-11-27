cmake_minimum_required(VERSION 3.12)

set(SRC_DIR "./src")
set(INCLUDE_DIR "./include")
set(IMGUI_DIR "${INCLUDE_DIR}/imgui")


set(PROJECT_NAME "ABS-Fractal-Explorer")
project(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin")

### Configuration ###

	# x87 __float80 (long double)
	set(FEATURE_Float80       "")

	# GCC quadmath __float128
	set(FEATURE_Float128      "")
	
	# (Deprecated) Requires libqd.a
#	set(FEATURE_LIBQD         "")
	
	# MPFR converts between coordinates and strings
	set(FEATURE_FloatMPFR     "")
	
	# GPU Rendering (OpenCL 1.2 or later)
	set(FEATURE_OpenCL        "")

	# Uses the Windows File dialog over the generic ones
	set(FEATURE_WindowsFileIO "")

	# Provides higher quality frame scaling
#	set(FEATURE_OpenCV_Scaler "")
	# Requires libquadmath.a
#	set(STATIC_OpenCV_Scaler  "")

	# Long compile times! Optimizes and reduces the size of the binary for release
#   set(COMPILE_FLTO "")

# Compiling
	set(CMAKE_C_COMPILER "gcc")
	set(CMAKE_CXX_COMPILER "g++")

# Set C and C++ standards
	set(CMAKE_C_STANDARD 11)
	set(CMAKE_CXX_STANDARD 17)

# Compiler Flags
	set(PROGRAM_OPT_FLAG -O3)
	set(LIBRARY_OPT_FLAG -O3)
	# -Ofast appears to have a noticable speedup compared to -O3 for SSE2 and AVX rendering.
	set(RENDER_CPU_OPT_FLAG -Ofast)
	set(MARCH_FLAGS -msse2)

# Source Files
	file(GLOB_RECURSE SRC_FILES
		"${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp"
		"${INCLUDE_DIR}/LIB-Dekker-Float/Float64x2/*.cpp" "${INCLUDE_DIR}/LIB-Dekker-Float/Float64x2/*.c"
		"${INCLUDE_DIR}/LIB-Dekker-Float/Float64x4/*.cpp" "${INCLUDE_DIR}/LIB-Dekker-Float/Float64x4/*.c"
		"${INCLUDE_DIR}/LIB-Dekker-Float/Float80x2/*.cpp" "${INCLUDE_DIR}/LIB-Dekker-Float/Float80x2/*.c"
	)
	if(NOT DEFINED FEATURE_OpenCL)
		file (GLOB REMOVE_FILES "${SRC_DIR}/render_GPU/*.c" "${SRC_DIR}/render_GPU/*.cpp")
		list(REMOVE_ITEM SRC_FILES ${REMOVE_FILES})
	endif()
	if(NOT DEFINED FEATURE_OpenCV_Scaler)
		file (GLOB REMOVE_FILES "${SRC_DIR}/imageTransform.cpp")
		list(REMOVE_ITEM SRC_FILES ${REMOVE_FILES})
	endif()


	file(GLOB SRC_SSE2_FILES "${SRC_DIR}/render_CPU/frac_Multi_SSE2.cpp")
	set_source_files_properties(SOURCE ${SRC_SSE2_FILES} PROPERTY COMPILE_FLAGS "-msse2 ${RENDER_CPU_OPT_FLAG}")
	
	file(GLOB SRC_AVX_FILES "${SRC_DIR}/render_CPU/frac_Multi_AVX.cpp")
	set_source_files_properties(SOURCE ${SRC_AVX_FILES} PROPERTY COMPILE_FLAGS "-mavx ${RENDER_CPU_OPT_FLAG}")
	
	file(GLOB SRC_AVX_DEKKER_FILES
		"${SRC_DIR}/render_CPU/frac_Multi_Float64x2_AVX.cpp"
		"${SRC_DIR}/render_CPU/frac_Multi_Float64x4_AVX.cpp"
		"${INCLUDE_DIR}/LIB-Dekker-Float/Float64x2/Float64x2_AVX.c"
		"${INCLUDE_DIR}/LIB-Dekker-Float/Float64x4/Float64x4_AVX.c"
	)
	set_source_files_properties(SOURCE ${SRC_AVX_DEKKER_FILES} PROPERTY COMPILE_FLAGS "-mavx ${RENDER_CPU_OPT_FLAG} -fno-associative-math")

	file(GLOB SRC_SSE2_DEKKER_FILES
		"${SRC_DIR}/render_CPU/frac_Multi_Float64x2_SSE2.cpp"
		"${SRC_DIR}/render_CPU/frac_Multi_Float64x4_SSE2.cpp"
	)
	set_source_files_properties(SOURCE ${SRC_SSE2_DEKKER_FILES} PROPERTY COMPILE_FLAGS "-msse2 ${RENDER_CPU_OPT_FLAG} -fno-associative-math")

	file(GLOB SRC_GENERIC_DEKKER_FILES
		"${SRC_DIR}/render_CPU/frac_Multi.cpp"
		"${INCLUDE_DIR}/LIB-Dekker-Float/Float64x2/Float64x2.cpp"
		"${INCLUDE_DIR}/LIB-Dekker-Float/Float64x4/Float64x4.cpp"
		"${INCLUDE_DIR}/LIB-Dekker-Float/Float80x2/Float80x2.cpp"
	)
	set_source_files_properties(SOURCE ${SRC_GENERIC_DEKKER_FILES} PROPERTY COMPILE_FLAGS "${RENDER_CPU_OPT_FLAG} -fno-associative-math")

# Icon
	set(ICON_FILE "./icons/ABS-Icon.o")

# Create an executable
	if(WIN32)
		add_executable(${PROJECT_NAME} ${SRC_FILES} ${ICON_FILE})
	elseif(UNIX AND NOT APPLE)
		add_executable(${PROJECT_NAME} ${SRC_FILES})
	endif()

# Packages and Macros
	find_package(SDL2 REQUIRED)

	# Defines the NDEBUG macro to disable <cassert>
#	target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)

	if(DEFINED FEATURE_Float80)
		target_compile_definitions(${PROJECT_NAME} PRIVATE Enable_Float80)
	endif()

	if(DEFINED FEATURE_Float128)
		#set(LIB_QuadMath "quadmath")
		set(LIB_QuadMath "-l:libquadmath.a")
		target_compile_definitions(${PROJECT_NAME} PRIVATE Enable_Float128)
	else()
		set(LIB_QuadMath "")
	endif()

	if(DEFINED FEATURE_LIBQD)
		set(LIB_QuadDouble "-l:libqd.a")
		target_compile_definitions(${PROJECT_NAME} PRIVATE Enable_LIBQD)
	else()
		set(LIB_QuadDouble "")
	endif()

	if(DEFINED FEATURE_FloatMPFR)
		#set(LIB_MPFR "-lmpfr -lgmp")
		set(LIB_MPFR "-l:libmpfr.a -l:libgmp.a")
		
		target_compile_definitions(${PROJECT_NAME} PRIVATE Enable_FloatMPFR)
	else()
		set(LIB_MPFR "")
	endif()

	if(DEFINED FEATURE_OpenCL)
		find_package(OpenCL REQUIRED)
		set(LIB_OpenCL "OpenCL")
		target_compile_definitions(${PROJECT_NAME} PRIVATE Enable_OpenCL)
	else()
		set(LIB_OpenCL "")
	endif()

	if(DEFINED FEATURE_OpenCV_Scaler)
		find_package(OpenCV REQUIRED)
		set(INCLUDE_DIR_OpenCV ${OpenCV_INCLUDE_DIRS})
		if(DEFINED STATIC_OpenCV_Scaler)
			# You may need to compile these yourself from the opencv git repo
			set(LIB_OpenCV
				"-l:libopencv_imgproc4100.a"
				"-l:libopencv_core4100.a"
				"-l:libzlib.a"
			)
		else()
			set(LIB_OpenCV ${OpenCV_LIBS})
		endif()
		
		
		target_compile_definitions(${PROJECT_NAME} PRIVATE Enable_OpenCV_Scaler)
	else()
		set(INCLUDE_DIR_OpenCV "")
		set(LIB_OpenCV "")
	endif()

	if(DEFINED FEATURE_WindowsFileIO)
		target_compile_definitions(${PROJECT_NAME} PRIVATE Enable_WindowsFileDialog)
	endif()

	if (DEFINED COMPILE_FLTO)
		set(FLTO_FLAG "-flto")
	else()
		set(FLTO_FLAG "")
	endif()

# Dear ImGui
	file(GLOB IMGUI_FILES "${IMGUI_DIR}/*.c" "${IMGUI_DIR}/*.cpp" "${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp" "${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp")
	add_library(IMGUI_LIB OBJECT ${IMGUI_FILES})
	target_compile_options(IMGUI_LIB PRIVATE ${LIBRARY_OPT_FLAG} ${FLTO_FLAG} ${MARCH_FLAGS} -Wall -Wshadow)

	include_directories(SYSTEM
		${SDL2_INCLUDE_DIRS}
		${IMGUI_DIR}
		"${INCLUDE_DIR}/bitGraphics_Font"
		"${INCLUDE_DIR}/LIB-Dekker-Float"
		${OpenCV_INCLUDE_DIRS}
	)

# Compile Options

	set(WARNING_FLAGS
		-Wall -Wextra -Wshadow
		-Werror=infinite-recursion
		-Wconversion -Wsign-conversion -Wformat=2 -Wsign-compare -Wfloat-conversion
	)
	set(WARNING_IGNORE_FLAGS
		-Wno-missing-field-initializers -Wno-format-nonliteral -Wno-missing-braces
	)

	set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -fdiagnostics-color=always")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")

	target_compile_options(${PROJECT_NAME} PRIVATE
		${PROGRAM_OPT_FLAG} ${FLTO_FLAG} ${MARCH_FLAGS}
		-fdiagnostics-color=always
		${WARNING_FLAGS} ${WARNING_IGNORE_FLAGS}
	)

# If using the OpenCV Scaler, add ${OpenCV_LIBS}
	target_link_libraries(${PROJECT_NAME} PRIVATE
		-static-libgcc -static-libstdc++ -lSDL2 IMGUI_LIB
		${LIB_QuadMath}
		${LIB_libQD}
		${LIB_MPFR}
		${LIB_OpenCL}
		${LIB_OpenCV}
	)
