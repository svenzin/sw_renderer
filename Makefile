INCLUDE = -I"D:\Documents\Development\SDL2\SDL2-2.0.4\i686-w64-mingw32\include"
LIB = -L"D:\Documents\Development\SDL2\SDL2-2.0.4\i686-w64-mingw32\lib"
EXE = render.exe

CXX = g++
CXXFLAGS = -Wall -c -std=c++11 $(INCLUDE)
LDFLAGS = -mwindows -mconsole $(LIB) -lmingw32 -lSDL2main -lSDL2

all: $(EXE)

$(EXE): main.o
	$(CXX) $< $(LDFLAGS) -o $@

main.o: src/main.cc
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm *.o && rm $(EXE)
	