#pragma once

struct Vec2D {
	float x;
	float y;

	bool isNull() const { return (x == 0.0f) && (y == 0.0f); }

	Vec2D transpose() const { return { y, x }; }
};

bool operator == (const Vec2D & u, const Vec2D & v) { return (u.x == v.x) && (u.y == v.y); }

Vec2D operator - (const Vec2D & u, const Vec2D & v) { return { u.x - v.x, u.y - v.y }; }

Vec2D operator / (const Vec2D & u, float d) { return { u.x / d, u.y / d }; }
