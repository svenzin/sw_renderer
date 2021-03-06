#include <iostream>
#include <cstring>
#include <vector>
#include <deque>

#include <SDL2/SDL.h>

#include <Vec2D.hh>
#include <Vec3D.hh>
#include <Model.hh>

float clamp(float f, float min, float max) { return std::min(std::max(f, min), max); }

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

struct Frame {
	explicit Frame(int w, int h)
	: _width(w),
	  _height(h),
	  _data(w*h),
	  _z(w*h)
	{}
	virtual ~Frame() {}
	const int _width;
	const int _height;
	std::vector<Uint32> _data;
	std::vector<float> _z;
	Uint32 _dummy;
	float _dummyz;
	Uint32 & at(int x, int y) {
		if ((x >= 0) && (x < _width)) {
			if ((y >= 0) && (y < _height)) {
				return _data[_width * y + x];
			}
		}
		return _dummy;//_data.at(-1);
	}
	float & z(int x, int y) {
		if ((x >= 0) && (x < _width)) {
			if ((y >= 0) && (y < _height)) {
				return _z[_width * y + x];
			}
		}
		return _dummyz;//_data.at(-1);
	}
};

void load();
void update(Frame & pixels);

const int FPS = 33;
const int WIDTH = 320 * 4;
const int HEIGHT = 180 * 4;
const int SCALE = 1;
const float ZOOM = 0.5f * HEIGHT;

class Key {
	static Uint8 _old[SDL_NUM_SCANCODES];
	static Uint8 _new[SDL_NUM_SCANCODES];

public:
	static void update() {
		std::swap(_old, _new);
		SDL_PumpEvents();
		int nKeys;
		auto kb = SDL_GetKeyboardState(&nKeys);
		for (int i = 0; i < nKeys; ++i) { _new[i] = (kb[i] != 0); }
	}
	static bool isDown   (SDL_Scancode key) { return _new[key]; }
	static bool isUp     (SDL_Scancode key) { return !isDown(key); }
	static bool isPressed(SDL_Scancode key) { return (!_old[key] && _new[key]); }
};
Uint8 Key::_old[SDL_NUM_SCANCODES];
Uint8 Key::_new[SDL_NUM_SCANCODES];

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

	Frame pixels(WIDTH, HEIGHT);

	Uint32 t0 = SDL_GetTicks();
	Uint32 t1 = t0;

	load();

	bool quit = false;
	while (!quit) {
		Key::update();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) quit = true;
			if ((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE)) quit = true;
		}

		update(pixels);

		SDL_UpdateTexture(tex, NULL, pixels._data.data(), pixels._width * sizeof(Uint32));

		SDL_RenderCopy(ren, tex, NULL, NULL);
		SDL_RenderPresent(ren);

		t0 = t1;
		Uint32 delay = SDL_GetTicks() - t0;
		if (delay < FPS) SDL_Delay(FPS - delay);
		t1 = SDL_GetTicks();
		std::cout << 1000.0 / (t1 - t0) << std::endl;
	}

	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}

struct Rasterizer {
	Frame & pixels;

	Uint32 & xy(int x, int y) { return pixels.at(x, y); }
	Uint32 & yx(int x, int y) { return pixels.at(y, x); }

	void point(Vec2D p, Uint32 color) {
		xy(p.x, p.y) = color;
	}

	void line(Vec2D u, Vec2D v, Uint32 color) {
		const Vec2D d = v - u;
		auto pt = Rasterizer::xy;
		if (abs(d.x) < abs(d.y)) {
			u = u.transpose();
			v = v.transpose();
			pt = Rasterizer::yx;
		}

		if (u.x > v.x) std::swap(u, v);
		(this->*pt)(int(u.x), int(u.y)) = color;
		if (int(v.x) > int(u.x)) {
			const float d = (v.y - u.y) / (v.x - u.x);
			for (int x = int(u.x) + 1; x < int(v.x); ++x) {
				(this->*pt)(x, int(u.y + (x - u.x) * d)) = color;
			}
			(this->*pt)(int(v.x), int(v.y)) = color;
		}
	}

	void wireframe(Vec2D u, Vec2D v, Vec2D w, Uint32 color) {
		line(u, v, color);
		line(v, w, color);
		line(w, u, color);
	}

	void hline(Vec3D u, Vec3D v, Uint32 color) {
		if (v.x < u.x) std::swap(u, v);
		int y = int(u.y);
		float dz = (u.x == v.x) ? 0.0f : (v.z - u.z) / (v.x - u.x);

		float zmin = u.z, zmax = v.z;
		if (zmin > zmax) std::swap(zmin, zmax);
		for (int x = int(u.x); x <= int(v.x); ++x) {
			const float xx = clamp(x, u.x, v.x);
			float z = u.z + (xx - u.x) * dz;
			if (z > pixels.z(x, y)) {
				pixels.z(x, y) = z;
				pixels.at(x, y) = color;
				Uint8 _ = Uint8(clamp(z + 128.0f, 10.0f, 250.0f));
//				pixels.at(x, y) = RGB(_, _, _);
			}
		}
	}
	void triangle(Vec3D u, Vec3D v, Vec3D w, Uint32 color) {
		if (u.y > v.y) std::swap(u, v);
		if (u.y > w.y) std::swap(u, w);
		if (v.y > w.y) std::swap(v, w);

		const int uy = int(u.y);
		const int vy = int(v.y);
		const int wy = int(w.y);

		const Vec3D uv = v - u;
		const Vec3D uw = w - u;
		const Vec3D vw = w - v;

		const Vec3D duv = uv / uv.y;
		const Vec3D duw = uw / uw.y;
		const Vec3D dvw = vw / vw.y;

		if (uy == vy) {
			hline(u, v, color);
		} else {
			for (int y = uy; y < vy; ++y) {
				const float yy = clamp(y, u.y, v.y);
				const Vec3D vv = u + (yy - u.y) * duv;
				const Vec3D ww = u + (yy - u.y) * duw;
				hline({ vv.x, y, vv.z }, { ww.x, y, ww.z }, color);
			}
		}

		if (vy == wy) {
			hline(v, w, color);
		} else {
			for (int y = vy; y <= wy; ++y) {
				const float yu = clamp(y, u.y, w.y);
				const float yv = clamp(y, v.y, w.y);
				const Vec3D vv = v + (yv - v.y) * dvw;
				const Vec3D ww = u + (yu - u.y) * duw;
				hline({ vv.x, y, vv.z }, { ww.x, y, ww.z }, color);
			}
		}
	}
};

Model m;
void load() {
	m = Model::OBJ("D:\\Documents\\Development\\workspace\\sw_renderer\\assets\\floor.obj");
	m = Model::OBJ("D:\\Documents\\Development\\workspace\\sw_renderer\\assets\\african_head\\african_head.obj");
	Mat3D t = Mat3D::Id();
	t.row[1].y = -1.0f;
	for (auto & v : m.vertices) {
		v = t * v;
	}
}

int t = 1;
float rcz = -3.14159f / 2;
float rlz = 0.0f;
float roy = 0.0f;
void update(Frame & pixels) {
	if (Key::isDown(SDL_SCANCODE_LEFT))  roy -= 0.1f;
	if (Key::isDown(SDL_SCANCODE_RIGHT)) roy += 0.1f;
	if (Key::isDown(SDL_SCANCODE_A)) rcz += 0.1f;
	if (Key::isDown(SDL_SCANCODE_D)) rcz -= 0.1f;
	if (Key::isDown(SDL_SCANCODE_J)) rlz += 0.1f;
	if (Key::isDown(SDL_SCANCODE_L)) rlz -= 0.1f;

	const int w = pixels._width;
	const int h = pixels._height;

	bool gradient = true;
	if (gradient) {
		for (int i = 0; i < w; ++i) {
			for (int j = 0; j < h; ++j) {
				const float u = i / float(w);
				const float v = (j + t) / float(h);
				pixels.at(i, j) = RGB(u, 1.0f - u, v);
			}
		}
	}
	if (Key::isPressed(SDL_SCANCODE_SPACE)) {
		t += FPS;
	}

	std::fill_n(pixels._z.begin(), pixels._z.size(), -1e20f);
	Rasterizer r { pixels };
	Uint32 white = RGB(1.0f, 1.0f, 1.0f);
	Uint32 black = RGB(0.0f, 0.0f, 0.0f);

	r.line({20.1, 20.1}, {20.1, 20.1}, white);
	r.line({40, 20}, {50, 20}, white);
	r.line({60, 20}, {60, 10}, white);
	r.line({80, 20}, {70, 20}, white);
	r.line({100, 20}, {100, 30}, white);
	r.line({40, 40}, {50, 50}, white);
	r.line({60, 40}, {70, 30}, white);
	r.line({80, 40}, {70, 50}, white);
	r.line({100, 40}, {90, 30}, white);
	r.line({40, 60}, {50, 55}, white);
	r.line({60, 60}, {70, 65}, white);
	r.line({80, 60}, {85, 70}, white);
	r.line({100, 60}, {95, 70}, white);
	r.line({120, 60}, {110, 65}, white);
	r.line({140, 60}, {130, 55}, white);
	r.line({160, 60}, {155, 50}, white);
	r.line({180, 60}, {185, 50}, white);

	r.line({100, 100}, {120, 110}, white);
	r.line({120, 110}, {100, 100}, white);

	auto d = 0.01*(t%100);
	r.line({1, 1}, {10, 2}, white);
	r.line({1, 11.5}, {10, 12.5}, white);
	r.line({1, 21.99}, {10, 22.99}, white);
	r.line({1, 31 + d}, {10, 32 + d}, white);
	r.line({21 + d, 31}, {22 + d, 40}, white);
	r.line({1 + d, 41}, {10 + d, 42}, white);

	r.wireframe({200+d, 50}, {280+d, 20}, {250+d, 150}, white);
	r.wireframe({200, 100+d}, {200, 100+d}, {200, 100+d}, white);

	r.triangle({10+d, 140, 10}, {30+d, 50, 50}, {60+d, 160, 200}, white);
	if ((t/FPS)%2) {
		r.wireframe({10+d, 140}, {30+d, 50}, {60+d, 160}, black);
	}

	Mat3D rot = { Vec3D { cos(roy), 0.0f, sin(roy) },
	              Vec3D {     0.0f, 1.0f,     0.0f },
				  Vec3D {-sin(roy), 0.0f, cos(roy) } };

	auto camera = Vec3D { cos(rcz), 0.0f, sin(rcz) }.normalized();
	Vec3D light = Vec3D { cos(rlz), 1.0f, sin(rlz) }.normalized();
	for (auto t : m.faces) {
		Vec3D v3[3];
		Vec2D v2[3];
		for (int i = 0; i < 3; ++i) {
			v3[i] = rot * m.vertices[t.vertices[i]];
			v3[i].x = ZOOM * v3[i].x + 0.5f * WIDTH;
			v3[i].y = ZOOM * v3[i].y + 0.5f * HEIGHT;
			v3[i].z = ZOOM * v3[i].z;
		}

		Vec3D n;
		n = rot * m.normal(t).normalized();

		const float c = clamp(light * n, 0.0f, 1.0f);
		if ((camera * n) >= 0.0f) {
			r.triangle(v3[0], v3[1], v3[2], RGB(c, c, c));
		}
	}
	if ((t/FPS)%2) {
		for (auto t : m.faces) {
			Vec3D v3[3];
			Vec2D v2[3];
			for (int i = 0; i < 3; ++i) {
				v3[i] = rot * m.vertices[t.vertices[i]];
				v2[i].x = int(ZOOM * v3[i].x) + WIDTH / 2;
				v2[i].y = int(ZOOM * v3[i].y) + HEIGHT / 2;
			}

			if ((camera * (rot * m.normal(t).normalized())) >= 0.0f) {
				r.wireframe(v2[0], v2[1], v2[2], black);
			}
		}
	}
}
