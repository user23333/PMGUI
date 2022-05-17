/*
Author: CSlime
Github: https://github.com/cs1ime
*/

#pragma once

#ifndef _PMGUI_VECTOR_H_
#define _PMGUI_VECTOR_H_

#include "math.h"
#include "assert.h"

#define PM_SQRT(v)(sqrtf(v))
#define PM_ASSERT(v)(assert(v))

typedef float PM_FLOAT;

struct PmVec2 {
	union {
		struct {
			float x;
			float y;
		};
		struct {
			float w;
			float h;
		};
	};
	
	PmVec2() { this->x = 0.f; this->y = 0.f; }
	PmVec2(float _x, float _y) { this->x = _x; this->y = _y; }
	float lenth() { return PM_SQRT(this->x * this->x + this->y * this->y); }
	float dis(const PmVec2& vec) { float _x = vec.x - this->x; float _y = vec.y - this->y; return PM_SQRT(_x * _x + _y * _y); }
	PmVec2 clamp() { if (this->y < -89.0f)this->y = -89.0f; if (this->y > 89.0f)this->y = 89.0f; if (this->x < -180.0f)this->x += 360.0f; if (this->x > 180.0f)this->x -= 360.0f; return *this; }
	bool operator==(const PmVec2& v) { return ((this->x == v.x) && (this->y == v.y)); }
	PmVec2& operator+(const PmVec2& v) { return PmVec2(this->x + v.x, this->y + v.y); }
	PmVec2& operator-(const PmVec2& v) { return PmVec2(this->x - v.x, this->y - v.y); }
	PmVec2& operator*(const PmVec2& v) { return PmVec2(this->x * v.x, this->y * v.y); }
	PmVec2& operator/(const PmVec2& v) { return PmVec2(this->x / v.x, this->y / v.y); }
	float  operator[] (size_t idx) const { PM_ASSERT(idx <= 1); return (&x)[idx]; }
	float& operator[] (size_t idx) { PM_ASSERT(idx <= 1); return (&x)[idx]; }
};
struct PmVec3 {
	float x;
	float y;
	float z;
	PmVec3() { this->x = 0.f; this->y = 0.f; this->z = 0.f; }
	PmVec3(float _x, float _y, float _z) { this->x = _x; this->y = _y; this->z = _z; }
	float dis(const PmVec3& vec) { float _x = vec.x - this->x; float _y = vec.y - this->y; float _z = vec.z - this->z; return PM_SQRT(_x * _x + _y * _y + _z * _z); }
	float length() { return PM_SQRT(this->x * this->x + this->y * this->y + this->z * this->z); }
	bool operator==(const PmVec3& v) { return ((this->x == v.x) && (this->y == v.y) && (this->z == v.z)); }
	PmVec3& operator+(const PmVec3& v) { this->x += v.x; this->y += v.y; this->z += v.z; return *this; }
	PmVec3& operator-(const PmVec3& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; return *this; }
	PmVec3& operator*(const PmVec3& v) { this->x *= v.x; this->y *= v.y; this->z *= v.z; return *this; }
	PmVec3& operator/(const PmVec3& v) { this->x /= v.x; this->y /= v.y; this->z /= v.z; return *this; }
	float  operator[] (size_t idx) const { PM_ASSERT(idx <= 2); return (&x)[idx]; }
	float& operator[] (size_t idx) { PM_ASSERT(idx <= 2); return (&x)[idx]; }
};
struct PmVec4 {
	float x, y, z, w;
	PmVec4() { this->x = 0.f; this->y = 0.f; this->z = 0.f; this->w = 0.f; }
	PmVec4(float _x, float _y, float _z, float _w) { this->x = _x; this->y = _y; this->z = _z; this->w = _w; }

};
struct PmRect {
	union {
		struct {
			float x;
			float y;
		};
		PmVec2 pos;
	};
	union {
		struct {
			float w;
			float h;
		};
		PmVec2 wh;
	};
	PmRect() { this->x = 0.f; this->y = 0.f; this->w = 0.f; this->h = 0.f; }
	PmRect(float _x, float _y, float _w, float _h) { this->x = _x; this->y = _y; this->w = _w; this->h = _h; }
};



#endif // !_PMGUI_VECTOR_INCLUDED_

