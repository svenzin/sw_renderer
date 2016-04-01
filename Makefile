INC_DIR = -I"$(SRC)" -I"D:\Documents\Development\SDL2\SDL2-2.0.4\i686-w64-mingw32\include"
LIB_DIR = -L"D:\Documents\Development\SDL2\SDL2-2.0.4\i686-w64-mingw32\lib"

SRC=./src
BIN=./bin
OBJ=./obj
LIB=./lib
EXE = render.exe

CXX = g++
CXXFLAGS = -Wall -c -std=c++11 $(INC_DIR) -O0 -ffast-math -g
LDFLAGS = -mwindows -mconsole $(LIB_DIR) -lmingw32 -lSDL2main -lSDL2

exe: $(BIN)/$(EXE) $(BIN)/SDL2.dll

tests: $(BIN)/Test_Model.exe

all: exe tests

# Executable

$(BIN)/SDL2.dll: SDL2.dll
	cp SDL2.dll $(BIN)

$(BIN)/$(EXE): $(OBJ)/main.o
	$(CXX) $< $(LDFLAGS) -o $@

$(OBJ)/main.o: $(SRC)/main.cc $(SRC)/Model.hh $(SRC)/Vec3D.hh $(SRC)/Vec2D.hh
	$(CXX) $(CXXFLAGS) $< -o $@

# Tests

$(BIN)/Test_Model.exe: $(OBJ)/Test_Model.o
	$(CXX) $< $(LDFLAGS) -o $@

$(OBJ)/Test_Model.o: $(SRC)/test/Test_Model.cc
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf $(BIN) $(LIB) $(OBJ)
	mkdir -p $(BIN) $(LIB) $(OBJ)
