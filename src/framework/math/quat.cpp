#include "quat.h"
#include <math.h>

quat angle_axis(float angle, const vec3& axis)
{
	vec3 norm = normalized(axis);
	float s = sinf(angle * 0.5f);
	return quat(
		norm.x * s,
		norm.y * s,
		norm.z * s,
		cosf(angle * 0.5f)
	);
}

/*
* Shortest arc between two points lies on a plane that contains both points and the center fo the sphere
* The plane is perpendicular to the axis of rotation between those vectors
* To get the axis of rotation --> normalize them and compute the cross product
* To get the angle --> compute dot product
* Finding the angle between the two vectors would be expensive, but the half-angle can be counted without knowing what the angle is
* Construct a quaternion using v1 and this halfway vector
*
* Quaternions double the angle --> compute the angle and divide it by 2 or compute the rotation with the half vector
*
* q.x = axis.x
* q.y = axis.y
* q.z = axis.z
* q.w = cos(angle/2)
*/
quat from_to(const vec3& from, const vec3& to)
{
	vec3 f = normalized(from);
	vec3 t = normalized(to);
	
	if (f == t) { // parallel vectors 
		return quat();
	}
	else if (f == t * -1.0f) { 	// check whether the two vectors are opposites of each other
		vec3 ortho = vec3(1, 0, 0);
		if (fabsf(f.y) < fabsf(f.x)) {
			ortho = vec3(0, 1, 0);
		}
		if (fabsf(f.z) < fabs(f.y) && fabs(f.z) < fabsf(f.x)) {
			ortho = vec3(0, 0, 1);
		}
		vec3 axis = normalized(cross(f, ortho));
		return quat(axis.x, axis.y, axis.z, 0);
	}
	// half vector between the from and to vectors
	vec3 half = normalized(f + t);
	vec3 axis = cross(f, half);
	return quat(axis.x, axis.y, axis.z, dot(f, half));
}

vec3 get_axis(const quat& quat)
{
	return normalized(vec3(quat.x, quat.y, quat.z));
}

// The angle of rotation is double the inverse cosine of the real component
float get_angle(const quat& quat)
{
	return 2.0f * acosf(quat.w);
}

quat operator+(const quat& a, const quat& b)
{
	return quat(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

quat operator-(const quat& a, const quat& b)
{
	return quat(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

quat operator*(const quat& a, float b)
{
	return quat(a.x * b, a.y * b, a.z * b, a.w * b);
}

quat operator-(const quat& q)
{
	return quat(-q.x, -q.y, -q.z, -q.w);
}

bool operator==(const quat& left, const quat& right)
{
	return (fabsf(left.x - right.x) <= QUAT_EPSILON &&
			fabsf(left.y - right.y) <= QUAT_EPSILON &&
			fabsf(left.z - right.z) <= QUAT_EPSILON &&
			fabsf(left.w - right.w) <= QUAT_EPSILON);
}

bool operator!=(const quat& a, const quat& b)
{
	return !(a == b);
}

bool same_orientation(const quat& l, const quat& r)
{
	return (fabsf(l.x - r.x) <= QUAT_EPSILON &&
			fabsf(l.y - r.y) <= QUAT_EPSILON &&
			fabsf(l.z - r.z) <= QUAT_EPSILON &&
			fabsf(l.w - r.w) <= QUAT_EPSILON) ||
		   (fabsf(l.x + r.x) <= QUAT_EPSILON &&
			fabsf(l.y + r.y) <= QUAT_EPSILON &&
			fabsf(l.z + r.z) <= QUAT_EPSILON &&
			fabsf(l.w + r.w) <= QUAT_EPSILON);
}

float dot(const quat& a, const quat& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// The length of a quaternion is the dot product of the quaternion with itself
float len_sq(const quat& q)
{
	return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}

float len(const quat& q)
{
	float len_sq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (len_sq < QUAT_EPSILON) {
		return 0.0f;
	}
	return sqrtf(len_sq);
}

void normalize(quat& q)
{
	float len_sq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (len_sq < QUAT_EPSILON) {
		return;
	}
	float inv_len = 1.0f / sqrtf(len_sq);
	q.x *= inv_len;
	q.y *= inv_len;
	q.z *= inv_len;
	q.w *= inv_len;
}

quat normalized(const quat& q)
{
	float len_sq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (len_sq < QUAT_EPSILON) {
		return quat();
	}
	float inv_len = 1.0f / sqrtf(len_sq);
	return quat(
		q.x * inv_len, 
		q.y * inv_len, 
		q.z * inv_len, 
		q.w * inv_len
	);
}

quat conjugate(const quat& q)
{
	return quat(
		-q.x,
		-q.y,
		-q.z,
		 q.w
	);
}

quat inverse(const quat& q)
{
	float len_sq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (len_sq < QUAT_EPSILON) {
		return quat();
	}
	float recip = 1.0f / len_sq;
	return quat(
		-q.x * recip,
		-q.y * recip,
		-q.z * recip,
		 q.w * recip
	);
}

// Right-to-left multiplication (as matrices)
quat operator*(const quat& q1, const quat& q2)
{
	return quat(
		q2.x * q1.w + q2.y * q1.z - q2.z * q1.y + q2.w * q1.x,
		-q2.x * q1.z + q2.y * q1.w + q2.z * q1.x + q2.w * q1.y,
		q2.x * q1.y - q2.y * q1.x + q2.z * q1.w + q2.w * q1.z,
		-q2.x * q1.x - q2.y * q1.y - q2.z * q1.z + q2.w * q1.w
	);
}

vec3 operator*(const quat& q, const vec3& v)
{
	return q.vector * 2.0f * dot(q.vector, v) +
		v * (q.scalar * q.scalar - dot(q.vector, q.vector)) +
		cross(q.vector, v) * 2.0f * q.scalar;
}

// lerp
quat mix(const quat& from, const quat& to, float t)
{
	return from * (1.0f - t) + to * t;
}

// Normalized Linear Interpolation. Close approximation to slerp but much cheaper and much faster in velocity (not constant)
quat nlerp(const quat& from, const quat& to, float t)
{
	return normalized(from + (to - from) * t);
}

// To raise a quaternion to some power, it needs to be decomposed into an angle and an axis
// Then, the angle can be adjusted by the power and a new quaternion can be built from the adjusted angle and axis
quat operator^(const quat& q, float f)
{
	float angle = 2.0f * acosf(q.scalar);
	vec3 axis = normalized(q.vector);
	float halfCos = cosf(f * angle * 0.5f);
	float halfSin = sinf(f * angle * 0.5f);
	return quat(
		axis.x * halfSin,
		axis.y * halfSin,
		axis.z * halfSin,
		halfCos
	);
}

/*
* Spherical linear interpolation
* To interpolate between two quaternions, find the delta quaternion from the start rotation
* to the end rotation. This delta quaternion is the interpolation path. Raise the angle to
* the power of how much the two quaternions are being interpolated between (usually
* represented as t) and multiply the start quaternion back.
* 
* The input vectors to slerp should be normalized, which means you could use
* conjugate instead of inverse in the slerp function. Most of the time, nlerp will
* be used over slerp.
*/
quat slerp(const quat& start, const quat& end, float t)
{
	if (fabsf(dot(start, end)) > 1.0f - QUAT_EPSILON) {
		return nlerp(start, end, t);
	}
	quat delta = inverse(start) * end;
	return normalized((delta ^ t) * start);
}

// Creates the "view quaternion"
quat look_rotation(const vec3& direction, const vec3& up)
{
	// Find orthonormal basis vectors
	vec3 f = normalized(direction); // Object Forward
	vec3 u = normalized(up); // Desired Up
	vec3 r = cross(u, f); // Object Right
	u = cross(f, r); // Object Up
	// From world forward to object forward
	quat worldToObject = from_to(vec3(0, 0, 1), f);
	// what direction is the new object up?
	vec3 objectUp = worldToObject * vec3(0, 1, 0);
	// From object up to desired up
	quat u2u = from_to(objectUp, u);
	// Rotate to forward direction first
	// then twist to correct up
	quat result = worldToObject * u2u;
	// Don't forget to normalize the result
	return normalized(result);
}

mat4 quat_to_mat4(const quat& q)
{
	// Normalize quaternion
	quat unit_q = normalized(q);

	// Components
	float x = unit_q.x;
	float y = unit_q.y;
	float z = unit_q.z;
	float w = unit_q.w;

	// Calcular los términos necesarios para la matriz
	float x2 = x + x;
	float y2 = y + y;
	float z2 = z + z;

	float xx = x * x2;
	float xy = x * y2;
	float xz = x * z2;

	float yy = y * y2;
	float yz = y * z2;
	float zz = z * z2;

	float wx = w * x2;
	float wy = w * y2;
	float wz = w * z2;

	return mat4(
		1.0f - (yy + zz), xy + wz, xz - wy, 0.0f,	// Column 0: X basis vector
		xy - wz, 1.0f - (xx + zz), yz + wx, 0.0f,	// Column 1: Y basis vector
		xz + wy, yz - wx, 1.0f - (xx + yy), 0.0f,	// Column 2: Z basis vector
		0.0f, 0.0f, 0.0f, 1.0f						// Column 3: Translation (none)
	);

}

quat mat4_to_quat(const mat4& m)
{
	vec3 up = normalized(vec3(m.up.x, m.up.y, m.up.z));
	vec3 forward = normalized(vec3(m.forward.x, m.forward.y, m.forward.z));
	vec3 right = cross(up, forward);
	up = cross(forward, right);

	return look_rotation(forward, up);
}

vec3 quat_to_euler(const quat& q)
{
	vec3 euler_angles;

	// roll (x-axis rotation)
	double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	euler_angles.x = atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = sqrt(1 + 2 * (q.w * q.y - q.x * q.z));
	double cosp = sqrt(1 - 2 * (q.w * q.y - q.x * q.z));
	euler_angles.y = 2 * atan2(sinp, cosp) - PI / 2;

	// yaw (z-axis rotation)
	double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	euler_angles.z = atan2(siny_cosp, cosy_cosp);

	return euler_angles;
}

quat euler_to_quat(float roll, float pitch, float yaw)
{
	// Abbreviations for the various angular functions
	double cr = cos(roll * 0.5);
	double sr = sin(roll * 0.5);
	double cp = cos(pitch * 0.5);
	double sp = sin(pitch * 0.5);
	double cy = cos(yaw * 0.5);
	double sy = sin(yaw * 0.5);

	quat q;
	q.w = cr * cp * cy + sr * sp * sy;
	q.x = sr * cp * cy - cr * sp * sy;
	q.y = cr * sp * cy + sr * cp * sy;
	q.z = cr * cp * sy - sr * sp * cy;

	return q;
}