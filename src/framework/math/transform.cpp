#include "transform.h"
#include <math.h>

// Transforms can be combined in the same way as matrices and quaternions and the effects of two transforms can be combined into one transform
// To keep things consistent, combining transforms should maintain a right-to-left combination order
Transform combine(const Transform& t1, const Transform& t2)
{
	Transform out;
	out.scale = t1.scale * t2.scale;
	out.rotation = t2.rotation * t1.rotation; // right-to-left multiplication (right is the first rotation applyed)
	// The combined position needs to be affected by the rotation and scale
	out.position = t1.rotation * (t1.scale * t2.position); // M = R*S*T
	out.position = t1.position + out.position;
	return out;
}

Transform inverse(const Transform& t)
{
	Transform inv;
	inv.rotation = inverse(t.rotation);
	inv.scale.x = fabs(t.scale.x) < VEC3_EPSILON ? 0.0f : 1.0f / t.scale.x;
	inv.scale.y = fabs(t.scale.y) < VEC3_EPSILON ? 0.0f : 1.0f / t.scale.y;
	inv.scale.z = fabs(t.scale.z) < VEC3_EPSILON ? 0.0f : 1.0f / t.scale.z;
	vec3 invTrans = t.position * -1.0f;
	inv.position = inv.rotation * (inv.scale * invTrans);
	return inv;
}

Transform mix(const Transform& a, const Transform& b, float t)
{
	quat bRot = b.rotation;
	if (dot(a.rotation, bRot) < 0.0f) {
		bRot = -bRot;
	}
	return Transform(
		lerp(a.position, b.position, t),
		nlerp(a.rotation, bRot, t),
		lerp(a.scale, b.scale, t));
}

// Extract the rotation and the translition from a matrix is easy. But not for the scale
// M = SRT, ignore the translation: M = SR -> invert R to isolate S
Transform mat4_to_transform(const mat4& m)
{
	Transform out;

	// 1. Extraer la traslaci�n (�ltima columna)
	out.position = vec3(m.tx, m.ty, m.tz);

	// 2. Extraer la rotaci�n directamente usando mat4_to_quat
	out.rotation = mat4_to_quat(m);

	// 3. Extraer la escala de los vectores base despu�s de "remover" la rotaci�n
	// Primero necesitamos la matriz de rotaci�n inversa
	quat invRot = inverse(out.rotation);

	// Crear un transform temporal solo con la rotaci�n inversa
	Transform invTransform;
	invTransform.rotation = invRot;
	invTransform.scale = vec3(1, 1, 1); // Escala neutral

	// Aplicar la rotaci�n inversa a los vectores base para aislar la escala
	vec3 right = transform_vector(invTransform, vec3(m.xx, m.xy, m.xz));
	vec3 up = transform_vector(invTransform, vec3(m.yx, m.yy, m.yz));
	vec3 forward = transform_vector(invTransform, vec3(m.zx, m.zy, m.zz));

	// La magnitud de estos vectores desrotados nos da la escala
	out.scale.x = sqrtf(right.x * right.x + right.y * right.y + right.z * right.z);
	out.scale.y = sqrtf(up.x * up.x + up.y * up.y + up.z * up.z);
	out.scale.z = sqrtf(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);

	// Manejar casos donde la escala podr�a ser cero
	if (fabsf(out.scale.x) < MAT4_EPSILON) out.scale.x = 0.0f;
	if (fabsf(out.scale.y) < MAT4_EPSILON) out.scale.y = 0.0f;
	if (fabsf(out.scale.z) < MAT4_EPSILON) out.scale.z = 0.0f;

	return out;
}

// Converts a transform into a mat4
mat4 transform_to_mat4(const Transform& t)
{
	// First create scale matrix
	mat4 scaleMat = mat4(
		t.scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, t.scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, t.scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	// Then create rotation matrix from quaternion
	mat4 rotationMat = quat_to_mat4(t.rotation);

	// Finally create translation matrix
	mat4 translationMat = mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		t.position.x, t.position.y, t.position.z, 1.0f
	);

	// Combine them in SRT order (Scale * Rotation * Translation)
	return scaleMat * rotationMat * translationMat;
}

vec3 transform_point(const Transform& a, const vec3& b)
{
	vec3 out;
	out = a.rotation * (a.scale * b);
	out = a.position + out;
	return out;
}

// First, apply the scale, then rotation
vec3 transform_vector(const Transform& t, const vec3& v)
{
	vec3 out;
	out = t.rotation * (t.scale * v);
	return out;
}