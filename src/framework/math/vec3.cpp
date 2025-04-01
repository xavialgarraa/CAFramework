#include "vec3.h"
#include <math.h>

// Basic operations
vec3 operator+(const vec3& l, const vec3& r)
{
	return vec3(l.x + r.x, l.y + r.y, l.z + r.z);
}

vec3 operator-(const vec3& l, const vec3& r)
{
	return vec3(l.x - r.x, l.y - r.y, l.z - r.z);
}

vec3 operator*(const vec3& l, const vec3& r)
{
	return vec3(l.x * r.x, l.y * r.y, l.z * r.z);
}

vec3 operator*(const vec3& v, float f)
{
	return vec3(v.x * f, v.y * f, v.z * f);
}

vec3 operator/(const vec3& l, const vec3& r)
{
	return vec3(l.x / r.x, l.y / r.y, l.z / r.z);
}

vec3 operator/(const vec3& l, float f)
{
	if (fabs(f) < VEC3_EPSILON) {
		if (f < VEC3_EPSILON) {
			f = -VEC3_EPSILON;
		}
		else {
			f = VEC3_EPSILON;
		}
	}
	return vec3(l.x / f, l.y / f, l.z / f);
}

// Used to measure how similar two vectors are
float dot(const vec3& l, const vec3& r)
{
	float dot_product = (l.x * r.x) + (l.y * r.y) + (l.z * r.z);

	return dot_product;
}

// Sometimes the length can be usefull in square space to avoid doing the square root operation
float len_sq(const vec3& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

float len(const vec3& v)
{
	float len_sq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (len_sq < VEC3_EPSILON) {
		return 0.0f;
	}
	return sqrtf(len_sq);
}

// Normalizes the input vector
void normalize(vec3& v)
{
	float len_sq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (len_sq < VEC3_EPSILON) { return; }
	float inv_len = 1.0f / sqrtf(len_sq);
	v.x *= inv_len;
	v.y *= inv_len;
	v.z *= inv_len;
}

// Return the normalization of the input vector without modifying it
vec3 normalized(const vec3& v)
{
	float len_sq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (len_sq < VEC3_EPSILON) { return v; }
	float inv_len = 1.0f / sqrtf(len_sq);
	return vec3(
		v.x * inv_len,
		v.y * inv_len,
		v.z * inv_len
	);
}

// In radians
float angle(const vec3& l, const vec3& r)
{
	float sq_mag_l = l.x * l.x + l.y * l.y + l.z * l.z;
	float sq_mag_r = r.x * r.x + r.y * r.y + r.z * r.z;
	if (sq_mag_l < VEC3_EPSILON || sq_mag_r < VEC3_EPSILON) {
		return 0.0f;
	}
	float dot = l.x * r.x + l.y * r.y + l.z * r.z;
	float len = sqrtf(sq_mag_l) * sqrtf(sq_mag_r);
	return acosf(dot / len); //rad
}

vec3 project(const vec3& a, const vec3& b)
{
	float mag_b_sq = len(b);
	if (mag_b_sq < VEC3_EPSILON) {
		return vec3();
	}
	float scale = dot(a, b) / mag_b_sq;
	return b * scale;
}

vec3 reject(const vec3& a, const vec3& b)
{
	vec3 projection = project(a, b);
	return a - projection;
}

// Bounce reflection. For a mirror reflection, negate the result or the incident vector
vec3 reflect(const vec3& a, const vec3& b)
{
	float mag_b_sq = len(b);
	if (mag_b_sq < VEC3_EPSILON) {
		return vec3();
	}
	float scale = dot(a, b) / mag_b_sq;
	vec3 proj2 = b * (scale * 2);
	return a - proj2;
}

// Returns a vector that is perpendicular to both input vectors
vec3 cross(const vec3& l, const vec3& r)
{
	// TODO
	// ..
	vec3 cross_vector;
	cross_vector.x = (l.y * r.z) - (r.y * l.z);
	cross_vector.y = -((l.x * r.z) - (r.x * l.z));
	cross_vector.z = (l.x * r.y) - (r.x * l.y);

	return cross_vector;
}

/*
* Linear interpolation.t is goes from 0 to 1
* When the value of t is close to 0, as slerp will yield unexpected results
* When the value of t is close to 0, fall back on lerp or normalized lerp (nlerp)
*/ 
vec3 lerp(const vec3& s, const vec3& e, float t)
{
	// TODO
	// ..

	return vec3();
}

// Spherical linear interpolation. Interpolates on the shortest arc (if t is close to 0, it can gives unexpected results)
vec3 slerp(const vec3& s, const vec3& e, float t)
{
	if (t < 0.01f) {
		return lerp(s, e, t);
	}
	vec3 from = normalized(s);
	vec3 to = normalized(e);
	float theta = angle(from, to);
	float sin_theta = sinf(theta);
	float a = sinf((1.0f - t) * theta) / sin_theta;
	float b = sinf(t * theta) / sin_theta;
	return from * a + to * b;
}

// Normalized Linear Interpolation. Close approximation to slerp but much cheaper and much faster in velocity (not constant)
vec3 nlerp(const vec3& s, const vec3& e, float t)
{
	vec3 linear(
		s.x + (e.x - s.x) * t,
		s.y + (e.y - s.y) * t,
		s.z + (e.z - s.z) * t
	);
	return normalized(linear);
}

// Comparison functions using the epsilon
bool operator==(const vec3& l, const vec3& r)
{
	vec3 diff(l - r);
	return len_sq(diff) < VEC3_EPSILON;
}

bool operator!=(const vec3& l, const vec3& r)
{
	return !(l == r);
}