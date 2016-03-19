all: render.exe

render: main.cpp
	g++ -o render.exe -c src/main.cpp