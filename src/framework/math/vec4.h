#pragma once

#include "vec3.h"

template<typename T>
struct TVec4 {
	union {
		struct {
			T x;
			T y;
			T z;
			T w;
		};
		T v[4];
	};
	inline TVec4<T>() : x((T)0), y((T)0), z((T)0), w((T)0) { }
	inline TVec4<T>(T _v) : x(_v), y(_v), z(_v), w(_v) { }
	inline TVec4<T>(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) { }
	inline TVec4<T>(T* fv) : x(fv[0]), y(fv[1]), z(fv[2]), w(fv[3]) { }
	inline TVec4<T>(vec3 v, T _w) : x(v.x), y(v.y), z(v.z), w(_w) { }
};
typedef TVec4<float> vec4;
typedef TVec4<int> ivec4;
typedef TVec4<unsigned int> uivec4;

vec4 operator+(const vec4& l, const vec4& r);
vec4 operator-(const vec4& l, const vec4& r);
vec4 operator*(const vec4& l, const vec4& r);
vec4 operator*(const vec4& v, float f);

vec4 lerp(const vec4& s, const vec4& e, const float t);