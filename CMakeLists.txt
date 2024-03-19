cmake_minimum_required(VERSION 3.12)

set(TOOLCHAIN_PATH "/c/msys64/mingw64")
set(BIN_PATH "${TOOLCHAIN_PATH}/bin")
set(INCLUDE_PATH "${TOOLCHAIN_PATH}/include")
set(LIB_PATH "${TOOLCHAIN_PATH}/lib")

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

# Icon
set(ICON_FILE "./icons/ABS-Icon.ico")
file(GLOB SRC_FILES "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp")
file(GLOB IMGUI_FILES "${IMGUI_DIR}/*.c" "${IMGUI_DIR}/*.cpp")
# Create an executable
add_executable(${PROJECT_NAME} ${SRC_FILES} ${IMGUI_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUI_DIR})
# Compiler Flags Debug(-g -O0) Release(-O3)
set(OPT_FLAG -O3)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=always")
set(MARCH_FLAGS "")
target_compile_options(${PROJECT_NAME} PUBLIC ${OPT_FLAG} ${MARCH_FLAGS} -fdiagnostics-color=always -Wall -Wshadow -Wno-unused-variable -Wno-comment)
target_link_libraries(${PROJECT_NAME} PUBLIC -static-libgcc -static-libstdc++ SDL2 SDL2main OpenCL quadmath ${OpenCV_LIBS})