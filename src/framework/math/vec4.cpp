#include "vec4.h"
#include <math.h>

// Basic operations
vec4 operator+(const vec4& l, const vec4& r)
{
	return vec4(l.x + r.x, l.y + r.y, l.z + r.z, l.w + r.w);
}

vec4 operator-(const vec4& l, const vec4& r)
{
	return vec4(l.x - r.x, l.y - r.y, l.z - r.z, l.w - r.w);
}

vec4 operator*(const vec4& l, const vec4& r)
{
	return vec4(l.x * r.x, l.y * r.y, l.z * r.z, l.w * r.w);
}

vec4 operator*(const vec4& v, float f)
{
	return vec4(v.x * f, v.y * f, v.z * f, v.w * f);
}

vec4 lerp(const vec4& s, const vec4& e, const float t)
{
	return vec4(
		s.x + (e.x - s.x) * t,
		s.y + (e.y - s.y) * t,
		s.z + (e.z - s.z) * t,
		s.w + (e.w - s.w) * t
	);
}