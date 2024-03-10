#g++ ./src/main.cpp ./src/render.cpp ./glad/src/glad.c -I./glad/include -o prog.exe -lmingw32 -lSDL2main -lSDL2

#SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
#OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.o,$(SRC_FILES)) $(BIN_DIR)/glad.o

#Compilier
CC = g++ 
# Directories
SRC_DIR = ./src ./glad/src
OBJ_DIR = ./obj
BIN_DIR = ./bin

# Output
BIN_OUTPUT = ./bin/ABS-Fractal-Explorer_v1-0-2.exe
# Sources
#SRC_FILES = ./src/main.cpp ./src/render.cpp ./src/fracSingle.cpp ./src/fracMulti.cpp ./src/fracCL.cpp ./src/prime2D.cpp ./src/primeUX.cpp ./src/graphics2D.cpp ./src/fileManager.cpp
SRC_FILES = ./src/main.cpp ./src/render.cpp ./src/fracMulti.cpp ./src/primeUX.cpp ./src/graphics2D.cpp ./src/fileManager.cpp
# Flags
CFLAGS = -std=c++11 -I./glad/include -Wno-write-strings
#LDFLAGS = -lOpenCL -lmingw32 -lSDL2main -lSDL2 -lglfw3 -lglfw3dll -pthread
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -pthread -mwindows -static-libgcc -static-libstdc++
OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

$(BIN_OUTPUT): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $@ $(LDFLAGS) -O3

($OBJ_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ -O3

($OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -O3

clean:
	rm -f $(BIN_DIR)/*

.PHONY: all clean