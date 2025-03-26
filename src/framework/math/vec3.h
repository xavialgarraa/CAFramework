#pragma once

// very small number used to compare floating numbers
#define VEC3_EPSILON 0.000001f

#include "vec2.h"

template<typename T>
struct TVec3 {
	union {
		struct {
			T x;
			T y;
			T z;
		};
		T v[3];
	};
	inline TVec3<T>() : x((T)0), y((T)0), z((T)0) { }
	inline TVec3<T>(T _v) : x(_v), y(_v), z(_v) { }
	inline TVec3<T>(T _x, T _y, T _z) : x(_x), y(_y), z(_z) { }
	inline TVec3<T>(T* fv) : x(fv[0]), y(fv[1]), z(fv[2]) { }
	inline TVec3<T>(vec2 v, T _z) : x(v.x), y(v.y), z(_z) { }
};
typedef TVec3<float> vec3;
typedef TVec3<int> ivec3;
typedef TVec3<unsigned int> uivec3;

vec3 operator+(const vec3& l, const vec3& r);
vec3 operator-(const vec3& l, const vec3& r);
vec3 operator*(const vec3& l, const vec3& r);
vec3 operator*(const vec3& v, float f);
vec3 operator/(const vec3& l, const vec3& r);
vec3 operator/(const vec3& l, float f);

// The dot product is used to measure how similar two vectors are
float dot(const vec3& l, const vec3& r);

/*
Finding the length of a vector involves a square root operation, which should
be avoided when possible. When checking the length of a vector, the check can
be done in squared space to avoid the square root. Ex: (dot(A, A) < 5 * 5)
*/
float len_sq(const vec3& v);
float len(const vec3& v);

// It akes a reference to a vectorand normalizes it in place
void normalize(vec3& v);

// Takes a constant reference and does not modify the input vector. It returns a new one
vec3 normalized(const vec3& v);

// In radians
float angle(const vec3& l, const vec3& r);
vec3 project(const vec3& a, const vec3& b);

vec3 reject(const vec3& a, const vec3& b);
// Bounce reflection. For a mirror reflection, negate the result or the incident vector
vec3 reflect(const vec3& a, const vec3& b);

// Cross product returns a third vector that is perpendicular to both input vectors
vec3 cross(const vec3& l, const vec3& r);

// Linear interpolation. The amount to lerp by is a normalized value between 0 and 1; Interpolates on the shortest path from one vector to another
vec3 lerp(const vec3& s, const vec3& e, float t);

// Spherical linear interpolation. Interpolates on the shortest arc
vec3 slerp(const vec3& s, const vec3& e, float t);

// It's a very close approximation and much cheaper to calculate than slerp .The only time it makes sense to use slerp instead is if constant interpolation velocity is required
vec3 nlerp(const vec3& s, const vec3& e, float t);
bool operator==(const vec3& l, const vec3& r);
bool operator!=(const vec3& l, const vec3& r);