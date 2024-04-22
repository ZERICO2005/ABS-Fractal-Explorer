cmake_minimum_required(VERSION 3.12)

set(SRC_DIR "./src")
set(IMGUI_DIR "./imgui")

set(PROJECT_NAME "ABS-Fractal-Explorer")
project(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin")

# Compiler
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

# Set C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Packages
find_package(SDL2 REQUIRED)
find_package(OpenCL REQUIRED)
find_package(OpenCV REQUIRED)

# ImGui
file(GLOB IMGUI_FILES "${IMGUI_DIR}/*.c" "${IMGUI_DIR}/*.cpp")
add_library(IMGUI_LIB OBJECT ${IMGUI_FILES})
target_compile_options(IMGUI_LIB PRIVATE -g -O3 -Wall -Wshadow)

# Source Files
file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp")



# Icon
set(ICON_FILE "./icons/ABS-Icon.o")

# Create an executable
if(WIN32)
	add_executable(${PROJECT_NAME} ${SRC_FILES} ${ICON_FILE})
elseif(UNIX AND NOT APPLE)
	add_executable(${PROJECT_NAME} ${SRC_FILES})
endif()

include_directories(SYSTEM ${SDL2_INCLUDE_DIRS} ${IMGUI_DIR})
# Compiler Flags Debug(-g -O0) Release(-O3)
set(OPT_FLAG -g -O3)

set(MARCH_FLAGS -msse2 -mavx)

set(WARNING_FLAGS -Wall -Wextra -Wshadow -Wconversion -Wsign-conversion -Wformat=2 -Wsign-compare)
set(WARNING_IGNORE_FLAGS -Wno-unused-parameter -Wno-unused-variable -Wno-missing-field-initializers -Wno-format-nonliteral -Wno-missing-braces -Wno-strict-aliasing)

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -fdiagnostics-color=always")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")

target_compile_options(${PROJECT_NAME} PUBLIC ${OPT_FLAG} ${MARCH_FLAGS} -fdiagnostics-color=always ${WARNING_FLAGS} ${WARNING_IGNORE_FLAGS})
target_link_libraries(${PROJECT_NAME} PUBLIC -static-libgcc -static-libstdc++ -lSDL2 -lSDL2main OpenCL quadmath ${OpenCV_LIBS} IMGUI_LIB)