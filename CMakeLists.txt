cmake_minimum_required(VERSION 3.12)

set(TOOLCHAIN_PATH "/c/msys64/mingw64")
set(BIN_PATH "${TOOLCHAIN_PATH}/bin")
set(INCLUDE_PATH "${TOOLCHAIN_PATH}/include")
set(LIB_PATH "${TOOLCHAIN_PATH}/lib")

set(SRC_DIR "./src")
set(IMGUI_DIR "./imgui")

set(PROJECT_NAME "Easy-GUI")
project(${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin")

# Compiler
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

# Set C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Find SDL2
find_package(SDL2 REQUIRED)
file(GLOB SRC_FILES "${SRC_DIR}/*.c" "${SRC_DIR}/*.cpp")
file(GLOB IMGUI_FILES "${IMGUI_DIR}/*.c" "${IMGUI_DIR}/*.cpp")
# Create an executable
add_executable(${PROJECT_NAME} ${SRC_FILES} ${IMGUI_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUI_DIR})
# Define compiler flags
target_compile_options(${PROJECT_NAME} PUBLIC -g -Wall)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL2 SDL2main)