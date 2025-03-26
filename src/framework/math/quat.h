#pragma once

#include "vec3.h"
#include "mat4.h"

#define PI 3.14159265359f
#define QUAT_DEG2RAD PI/180
#define QUAT_RAD2DEG 180/PI
#define QUAT_EPSILON 0.000001f

struct quat {
	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};
		struct {
			vec3 vector;
			float scalar;
		};
		float v[4];
	};
	inline quat() :
		x(0), y(0), z(0), w(1) { }
	inline quat(float _x, float _y, float _z, float _w)
		: x(_x), y(_y), z(_z), w(_w) { }
};

// angle in radians
quat angle_axis(float angle, const vec3& axis);
quat from_to(const vec3& from, const vec3& to);
vec3 get_axis(const quat& quat);
// angle in radians
float get_angle(const quat& quat);

quat operator+(const quat& a, const quat& b);
quat operator-(const quat& a, const quat& b);
quat operator*(const quat& a, float b);
quat operator-(const quat& q);
bool operator==(const quat& left, const quat& right);
bool operator!=(const quat & a, const quat & b);

bool same_orientation(const quat& l, const quat& r);

float dot(const quat& a, const quat& b);
float len_sq(const quat& q);
float len(const quat& q);
void normalize(quat& q);
quat normalized(const quat& q);
quat conjugate(const quat& q);
quat inverse(const quat& q);

quat operator*(const quat& q1, const quat& q2);
vec3 operator*(const quat& q, const vec3& v);
quat operator^(const quat& q, float f);

quat mix(const quat& from, const quat& to, float t);
quat nlerp(const quat& from, const quat& to, float t);
quat slerp(const quat& start, const quat& end, float t);

quat look_rotation(const vec3& direction, const vec3& up);
mat4 quat_to_mat4(const quat& q);
quat mat4_to_quat(const mat4& m);

vec3 quat_to_euler(const quat& q);
quat euler_to_quat(float roll, float pitch, float yaw);