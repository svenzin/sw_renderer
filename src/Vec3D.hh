#pragma once

#include <math.h>

struct Vec3D;

Vec3D operator - (const Vec3D & u, const Vec3D & v);
Vec3D operator ^ (const Vec3D & u, const Vec3D & v);
float operator * (const Vec3D & u, const Vec3D & v);

Vec3D operator / (const Vec3D & u, float f);

struct Vec3D {
	float x;
	float y;
	float z;

	float norm2() const { return (*this) * (*this); }
	float norm() const { return sqrtf(norm2()); }
	Vec3D normalized() const { return (*this) / norm(); }
};

Vec3D operator - (const Vec3D & u, const Vec3D & v) {
	return { u.x - v.x, u.y - v.y, u.z - v.z };
}

Vec3D operator ^ (const Vec3D & u, const Vec3D & v) {
	return {
		u.y * v.z - u.z * v.y,
		u.z * v.x - u.x * v.z,
		u.x * v.y - u.y * v.x
	};
}

float operator * (const Vec3D & u, const Vec3D & v) {
	return (u.x * v.x) + (u.y * v.y) + (u.z * v.z);
}

Vec3D operator / (const Vec3D & u, float f) {
	return { u.x / f, u.y / f, u.z / f };
}
