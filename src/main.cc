#include <iostream>
#include <SDL2/SDL.h>

Uint32 RGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	return (Uint32(r) << 24) + (Uint32(g) << 16) + (Uint32(b) << 8) + Uint32(a);
}

Uint32 RGB(Uint8 r, Uint8 g, Uint8 b) {
	return RGBA(r, g, b, SDL_ALPHA_OPAQUE);
}

Uint32 RGBA(float r, float g, float b, float a) {
	return RGBA(Uint8(256*r), Uint8(256*g), Uint8(256*b), Uint8(256*a));
}

Uint32 RGB(float r, float g, float b) {
	return RGB(Uint8(256*r), Uint8(256*g), Uint8(256*b));
}

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window * win = SDL_CreateWindow("Software Renderer", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (win == nullptr){
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr){
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_Texture * tex = SDL_CreateTexture(ren,
	                                      SDL_PIXELFORMAT_RGBA8888,
										  SDL_TEXTUREACCESS_STATIC,
										  640, 480);
	if (tex == nullptr){
		SDL_DestroyTexture(tex);
		std::cout << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	Uint32 * pixels = new Uint32[640 * 480];
	for (int i = 0; i < 640; ++i) {
		for (int j = 0; j < 480; ++j) {
			const float u = i/640.0f;
			const float v = j/480.0f;
			pixels[640 * j + i] = RGB(u, 1.0f - u, v);
		}
	}

	SDL_UpdateTexture(tex, NULL, pixels, 640 * sizeof(Uint32));

	SDL_RenderCopy(ren, tex, NULL, NULL);
	SDL_RenderPresent(ren);

	SDL_Delay(2000);

	delete[] pixels;
	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}
