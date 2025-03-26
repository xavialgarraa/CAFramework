#pragma once

template<typename T>
struct TVec2 {
	union {
		struct {
			T x;
			T y;
		};
		T v[2];
	};
	inline TVec2() : x(T(0)), y(T(0)) { }
	inline TVec2(T _v) : x(_v), y(_v) { }
	inline TVec2(T _x, T _y) :	x(_x), y(_y) { }
	inline TVec2(T* fv) : x(fv[0]), y(fv[1]) { }
	
};
template<typename T>
inline TVec2<T> operator-(const TVec2<T>& l, const TVec2<T>& r) { return TVec2<T>(l.x - r.x, l.y - r.y); };

typedef TVec2<float> vec2;
typedef TVec2<int> ivec2;

vec2 operator+(const vec2& l, const vec2& r);
vec2 operator-(const vec2& l, const vec2& r);
vec2 operator*(const vec2& l, const vec2& r);
vec2 operator*(const vec2& v, const float f);