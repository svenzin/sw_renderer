#include <iostream>
#include <SDL2/SDL.h>

Uint32 RGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	return (Uint32(r) << 24) + (Uint32(g) << 16) + (Uint32(b) << 8) + Uint32(a);
}

Uint32 RGB(Uint8 r, Uint8 g, Uint8 b) {
	return RGBA(r, g, b, SDL_ALPHA_OPAQUE);
}

Uint32 RGBA(float r, float g, float b, float a) {
	return RGBA(Uint8(255*r), Uint8(255*g), Uint8(255*b), Uint8(255*a));
}

Uint32 RGB(float r, float g, float b) {
	return RGB(Uint8(255*r), Uint8(255*g), Uint8(255*b));
}

const int FPS = 33;
const int WIDTH = 320 * 1;
const int HEIGHT = 180 * 1;
const int SCALE = 4;

int main(int argc, char** argv) {
	const int FULL_WIDTH = SCALE * WIDTH;
	const int FULL_HEIGHT = SCALE * HEIGHT;

	if (SDL_Init(SDL_INIT_EVERYTHING/*SDL_INIT_VIDEO*/) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window * win = SDL_CreateWindow("Software Renderer",
	                                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
										FULL_WIDTH, FULL_HEIGHT,
										SDL_WINDOW_SHOWN);
	if (win == nullptr){
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
	if (ren == nullptr){
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	SDL_RenderSetLogicalSize(ren, WIDTH, HEIGHT);

	SDL_Texture * tex = SDL_CreateTexture(ren,
	                                      SDL_PIXELFORMAT_RGBA8888,
										  SDL_TEXTUREACCESS_STREAMING,
										  WIDTH, HEIGHT);
	if (tex == nullptr){
		SDL_DestroyTexture(tex);
		std::cout << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	Uint32 * pixels = new Uint32[WIDTH*HEIGHT];

	Uint32 t0 = SDL_GetTicks();
	Uint32 t1 = t0;

	int t = 0;
	while (true) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {}

		for (int i = 0; i < WIDTH; ++i) {
			for (int j = 0; j < HEIGHT; ++j) {
				const float u = (i % WIDTH) / float(WIDTH);
				const float v = ((j + t) % HEIGHT) / float(HEIGHT);
				pixels[WIDTH * j + i] = RGB(u, 1.0f - u, v);
			}
		}
		SDL_UpdateTexture(tex, NULL, pixels, WIDTH * sizeof(Uint32));

		SDL_RenderCopy(ren, tex, NULL, NULL);
		SDL_RenderPresent(ren);

		t0 = t1;
		Uint32 delay = SDL_GetTicks() - t0;
		if (delay < FPS) SDL_Delay(FPS - delay);
		t1 = SDL_GetTicks();
		std::cout << 1000.0 / (t1 - t0) << std::endl;
		t++;
	}

	delete[] pixels;
	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}
