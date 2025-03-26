#include "mat4.h"
#include <iostream>
#include <math.h>

// It takes two numbers, the row of "a" and the column of "b", to dot together and the result is the dot product of the two.
#define M4D(aRow, bCol) \
 a.data[0 * 4 + aRow] * b.data[bCol * 4 + 0] + \
 a.data[1 * 4 + aRow] * b.data[bCol * 4 + 1] + \
 a.data[2 * 4 + aRow] * b.data[bCol * 4 + 2] + \
 a.data[3 * 4 + aRow] * b.data[bCol * 4 + 3]

// It takes the row of a matrix and performs a dot product of that row against the provided column vector.
#define M4V4D(mRow, x, y, z, w) \
 x * m.data[0 * 4 + mRow] + \
 y * m.data[1 * 4 + mRow] + \
 z * m.data[2 * 4 + mRow] + \
 w * m.data[3 * 4 + mRow]

// Swap the values of two variables
#define M4SWAP(x, y) \
 {float t = x; x = y; y = t; }

// It finds the minor of one element in the matrix, given an array of floats, and three rows and three columns to cut from the matrix.
#define M4_3X3MINOR(x, c0, c1, c2, r0, r1, r2) \
 (x[c0 * 4 + r0] * (x[c1 * 4 + r1] * x[c2 * 4 + r2] - x[c1 * 4 + r2] * x[c2 * 4 + r1]) - \
  x[c1 * 4 + r0] * (x[c0 * 4 + r1] * x[c2 * 4 + r2] - x[c0 * 4 + r2] * x[c2 * 4 + r1]) + \
  x[c2 * 4 + r0] * (x[c0 * 4 + r1] * x[c1 * 4 + r2] - x[c0 * 4 + r2] * x[c1 * 4 + r1]))


// Loop through matrices and compare all of their components
// Since floating point numbers are compared, an epsilon should be used
bool operator==(const mat4& a, const mat4& b)
{
	for (int i = 0; i < 16; ++i) {
		if (fabsf(a.data[i] - b.data[i]) > MAT4_EPSILON) {
			return false;
		}
	}
	return true;
}

bool operator!=(const mat4& a, const mat4& b)
{
	return !(a == b);
}

// Sum their respective components and store the result in a new matrix
mat4 operator+(const mat4& a, const mat4& b)
{
	return mat4(
		a.xx + b.xx, a.xy + b.xy, a.xz + b.xz, a.xw + b.xw,
		a.yx + b.yx, a.yy + b.yy, a.yz + b.yz, a.yw + b.yw,
		a.zx + b.zx, a.zy + b.zy, a.zz + b.zz, a.zw + b.zw,
		a.tx + b.tx, a.ty + b.ty, a.tz + b.tz, a.tw + b.tw
	);
}

// Multiply every element by the provided floating point number
mat4 operator*(const mat4& m, float f)
{
	return mat4(
		m.xx * f, m.xy * f, m.xz * f, m.xw * f,
		m.yx * f, m.yy * f, m.yz * f, m.yw * f,
		m.zx * f, m.zy * f, m.zz * f, m.zw * f,
		m.tx * f, m.ty * f, m.tz * f, m.tw * f
	);
}

// Right-to-left multiplication between matrices
mat4 operator*(const mat4& a, const mat4& b)
{
	return mat4(
		M4D(0, 0), M4D(1, 0), M4D(2, 0), M4D(3, 0), // Col 0
		M4D(0, 1), M4D(1, 1), M4D(2, 1), M4D(3, 1), // Col 1
		M4D(0, 2), M4D(1, 2), M4D(2, 2), M4D(3, 2), // Col 2
		M4D(0, 3), M4D(1, 3), M4D(2, 3), M4D(3, 3)  // Col 3
	);
}

// Matrix-vector multiplication
vec4 operator*(const mat4& m, const vec4& v)
{
	return vec4(
		M4V4D(0, v.x, v.y, v.z, v.w),
		M4V4D(1, v.x, v.y, v.z, v.w),
		M4V4D(2, v.x, v.y, v.z, v.w),
		M4V4D(3, v.x, v.y, v.z, v.w)
	);
}

// Matrix-vector multiplication in homogeneous space->vec4(vec3, 0.0) : assumes the vector represents a direction and magnitude
vec3 transform_vector(const mat4& m, const vec3& v)
{
	return vec3(
		M4V4D(0, v.x, v.y, v.z, 0.0f),
		M4V4D(1, v.x, v.y, v.z, 0.0f),
		M4V4D(2, v.x, v.y, v.z, 0.0f)
	);
}

// Matrix-point multiplication in homogeneous space -> vec4(vec3, 1.0)
vec3 transform_point(const mat4& m, const vec3& v)
{
	return vec3(
		M4V4D(0, v.x, v.y, v.z, 1.0f),
		M4V4D(1, v.x, v.y, v.z, 1.0f),
		M4V4D(2, v.x, v.y, v.z, 1.0f)
	);
}

// Matrix-point multiplication in homogeneous space, but it takes an additional W component
// The W component is a reference—it is a read-write. After the function is executed, the w component holds the value for W, if the input vector had been vec4
vec3 transform_point(const mat4& m, const vec3& v, float& w)
{
	float _w = w;
	w = M4V4D(3, v.x, v.y, v.z, _w);
	return vec3(
		M4V4D(0, v.x, v.y, v.z, _w),
		M4V4D(1, v.x, v.y, v.z, _w),
		M4V4D(2, v.x, v.y, v.z, _w)
	);
}

// It flips every element of the matrix across its main diagonal and puts the result in the same matrix
void transpose(mat4& m)
{
	M4SWAP(m.yx, m.xy);
	M4SWAP(m.zx, m.xz);
	M4SWAP(m.tx, m.xw);
	M4SWAP(m.zy, m.yz);
	M4SWAP(m.ty, m.yw);
	M4SWAP(m.tz, m.zw);
}

// It flips every element of the matrix across its main diagonal and returns the resulting matrix
mat4 transposed(const mat4& m)
{
	return mat4(
		m.xx, m.yx, m.zx, m.tx,
		m.xy, m.yy, m.zy, m.ty,
		m.xz, m.yz, m.zz, m.tz,
		m.xw, m.yw, m.zw, m.tw
	);
}

// The determinant multiplies each element by the cofactor, so some of the values need to be negated
float determinant(const mat4& m)
{
	return m.data[0]  * M4_3X3MINOR(m.data, 1, 2, 3, 1, 2, 3)
		 - m.data[4]  * M4_3X3MINOR(m.data, 0, 2, 3, 1, 2, 3)
		 + m.data[8]  * M4_3X3MINOR(m.data, 0, 1, 3, 1, 2, 3)
		 - m.data[12] * M4_3X3MINOR(m.data, 0, 1, 2, 1, 2, 3);
}

// It uses the M4_3X3MINOR macro to find the matrix of minors, then negates the appropriate elements to create the cofactor matrix
// It returns the transpose of the cofactor matrix
mat4 adjugate(const mat4& m)
{
	//Cof (M[i, j]) = Minor(M[i, j]] * pow(-1, i + j)
	mat4 cofactor;
	cofactor.data[0]  =  M4_3X3MINOR(m.data, 1, 2, 3, 1, 2, 3);
	cofactor.data[1]  = -M4_3X3MINOR(m.data, 1, 2, 3, 0, 2, 3);
	cofactor.data[2]  =  M4_3X3MINOR(m.data, 1, 2, 3, 0, 1, 3);
	cofactor.data[3]  = -M4_3X3MINOR(m.data, 1, 2, 3, 0, 1, 2);
	cofactor.data[4]  = -M4_3X3MINOR(m.data, 0, 2, 3, 1, 2, 3);
	cofactor.data[5]  =  M4_3X3MINOR(m.data, 0, 2, 3, 0, 2, 3);
	cofactor.data[6]  = -M4_3X3MINOR(m.data, 0, 2, 3, 0, 1, 3);
	cofactor.data[7]  =  M4_3X3MINOR(m.data, 0, 2, 3, 0, 1, 2);
	cofactor.data[8]  =  M4_3X3MINOR(m.data, 0, 1, 3, 1, 2, 3);
	cofactor.data[9]  = -M4_3X3MINOR(m.data, 0, 1, 3, 0, 2, 3);
	cofactor.data[10] =  M4_3X3MINOR(m.data, 0, 1, 3, 0, 1, 3);
	cofactor.data[11] = -M4_3X3MINOR(m.data, 0, 1, 3, 0, 1, 2);
	cofactor.data[12] = -M4_3X3MINOR(m.data, 0, 1, 2, 1, 2, 3);
	cofactor.data[13] =  M4_3X3MINOR(m.data, 0, 1, 2, 0, 2, 3);
	cofactor.data[14] = -M4_3X3MINOR(m.data, 0, 1, 2, 0, 1, 3);
	cofactor.data[15] =  M4_3X3MINOR(m.data, 0, 1, 2, 0, 1, 2);

	return transposed(cofactor);
}

// It returns a new matrix that is the inverse of the provided matrix
mat4 inverse(const mat4& m)
{
	float det = determinant(m);
	if (det == 0.0f) {
		std::cout << " Warning: Matrix determinant is 0\n";
		return mat4();
	}
	mat4 adj = adjugate(m);
	return adj * (1.0f / det);
}

// Invert the matrix inline, modifying the argument
void invert(mat4& m)
{
	float det = determinant(m);
	if (det == 0.0f) {
		std::cout << " Warning: Matrix determinant is 0\n";
		m = mat4();
		return;
	}
	m = adjugate(m) * (1.0f / det);
}

// It constructs a view frustum
mat4 frustum(float l, float r, float b, float t, float n, float f)
{
	if (l == r || t == b || n == f) {
		std::cout << "Error: Invalid frustum\n";
		return mat4(); // Error
	}
	return mat4(
		(2.0f * n) / (r - l), 0, 0, 0,
		0, (2.0f * n) / (t - b), 0, 0,
		(r + l) / (r - l), (t + b) / (t - b), (-(f + n)) / (f - n), -1,
		0, 0, (-2 * f * n) / (f - n), 0
	);
}

mat4 perspective(float fov, float aspect, float n, float f)
{
	float ymax = n * tanf(fov * 3.14159265359f / 360.0f);
	float xmax = ymax * aspect;
	return frustum(-xmax, xmax, -ymax, ymax, n, f);
}

mat4 orthographic(float l, float r, float b, float t, float n, float f)
{
	if (l == r || t == b || n == f) {
		std::cout << "Error: Orthographic matrix cannot be built\n";
		return mat4(); // Error
	}
	return mat4(
		2.0f / (r - l), 0, 0, 0,
		0, 2.0f / (t - b), 0, 0,
		0, 0, -2.0f / (f - n), 0,
		-((r + l) / (r - l)), -((t + b) / (t - b)), -((f + n) / (f - n)), 1
	);
}

// It constructs the view matrix
// The view matrix is the inverse of the camera's transformation (the position, rotation, and scale of the camera)
// Since the basis vectors are orthonormal, their inverse is the same as their transpose
mat4 look_at(const vec3& position, const vec3& target, const vec3& up)
{
	//Front/Forward vector
	vec3 f = normalized(target - position) * -1.0f;

	//Right/Side vector
	vec3 r = cross(up, f); // Right handed
	if (r == vec3(0, 0, 0)) {
		//std::cout << "Error: View and up vector are parallels\n";
		return mat4(); // Error
	}
	normalize(r);

	//Uo/Top vector
	vec3 u = normalized(cross(f, r)); // Right handed
	vec3 t = vec3(
		-dot(r, position),
		-dot(u, position),
		-dot(f, position)
	);
	return mat4(
		// Transpose upper 3x3 matrix to invert it
		r.x, u.x, f.x, 0,
		r.y, u.y, f.y, 0,
		r.z, u.z, f.z, 0,
		t.x, t.y, t.z, 1
	);
}

mat4 translate(const mat4& m, const vec3& v)
{
	mat4 result;
	result.position = m.position + m.right * v.x + m.up * v.y + m.forward * v.z;
	return result;
}

mat4 scale(const mat4& m, const vec3& v)
{
	mat4 result;
	result.right = m.right * v.x;
	result.up = m.up * v.y;
	result.forward = m.forward * v.z;
	return result;
}