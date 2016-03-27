INC_DIR = -I"D:\Documents\Development\SDL2\SDL2-2.0.4\i686-w64-mingw32\include"
LIB_DIR = -L"D:\Documents\Development\SDL2\SDL2-2.0.4\i686-w64-mingw32\lib"

SRC=./src
BIN=./bin
OBJ=./obj
LIB=./lib
EXE = render.exe

CXX = g++
CXXFLAGS = -Wall -c -std=c++11 $(INC_DIR) -O3 -ffast-math
LDFLAGS = -mwindows -mconsole $(LIB_DIR) -lmingw32 -lSDL2main -lSDL2

all: $(BIN)/$(EXE) $(BIN)/SDL2.dll

$(BIN)/SDL2.dll: SDL2.dll
	cp SDL2.dll $(BIN)

$(BIN)/$(EXE): $(OBJ)/main.o
	$(CXX) $< $(LDFLAGS) -o $@

$(OBJ)/main.o: $(SRC)/main.cc
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf $(BIN) $(LIB) $(OBJ)
	mkdir -p $(BIN) $(LIB) $(OBJ)
