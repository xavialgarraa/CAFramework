#include "camera.h"

#include "includes.h"
#include <iostream>

Camera* Camera::current = nullptr;

Camera::Camera()
{
	this->current = this;

	this->view_matrix = mat4();
	this->projection_matrix = mat4();
	this->viewprojection_matrix = mat4();

	set_orthographic(-1, 1, 1, -1, -1, 1);
}

vec3 Camera::get_local_vector(const vec3& v)
{
	mat4 i_view = inverse(view_matrix);

	vec4 result = i_view * vec4(v, 0.0f);
	return vec3(result.x, result.y, result.z);
}

vec3 Camera::project_vector(vec3 pos, bool& negZ)
{
	vec4 result = viewprojection_matrix * vec4(pos, 1.0);
	negZ = result.z < 0;
	if (type == ORTHOGRAPHIC)
		return vec3(result.x, result.y, result.z);
	else
		return vec3(result.x, result.y, result.z) / result.w;
}

void Camera::rotate(float angle, const vec3& axis)
{
	vec3 front = center - eye;
	//normalize(front);
	quat rotation = angle_axis(angle, axis);

	vec3 new_front = rotation * front;
	center = new_front + eye;
	update_view_matrix();
}

vec3 Camera::transform_quat(const vec3& a, const quat& q)
{
	float qx = q.x, qy = q.y, qz = q.z, qw = q.w;
	float x = a.x, y = a.y, z = a.z;

	// var qvec = [qx, qy, qz];
	// var uv = vec3.cross([], qvec, a);
	float uvx = qy * z - qz * y,
		uvy = qz * x - qx * z,
		uvz = qx * y - qy * x;
	// var uuv = vec3.cross([], qvec, uv);
	float uuvx = qy * uvz - qz * uvy,
		uuvy = qz * uvx - qx * uvz,
		uuvz = qx * uvy - qy * uvx;
	// vec3.scale(uv, uv, 2 * w);
	float w2 = qw * 2;
	uvx *= w2;
	uvy *= w2;
	uvz *= w2;
	// vec3.scale(uuv, uuv, 2);
	uuvx *= 2;
	uuvy *= 2;
	uuvz *= 2;
	// return vec3.add(out, a, vec3.add(out, uv, uuv));

	vec3 out;

	out.x = x + uvx + uuvx;
	out.y = y + uvy + uuvy;
	out.z = z + uvz + uuvz;

	return out;
}

quat Camera::set_axis_angle(const vec3& axis, const float angle)
{
	quat q;
	float s;
	s = sinf(angle * 0.5f);

	q.x = axis.x * s;
	q.y = axis.y * s;
	q.z = axis.z * s;
	q.w = cosf(angle * 0.5f);

	return q;
}


void Camera::orbit(float yaw, float pitch)
{
	vec3 front = normalized(center - eye);
	float problem_angle = dot(front, up);

	vec4 right_aux = inverse(view_matrix) * vec4(1.f, 0.f, 0.f, 0.f);
	vec3 right = vec3(right_aux.x, right_aux.y, right_aux.z);
	vec3 dist = eye - center;

	// yaw
	quat R = angle_axis(-yaw, up);
	dist = transform_quat(dist, R);

	if (!(problem_angle > 0.99 && pitch > 0 || problem_angle < -0.99 && pitch < 0)) {
		// pitch
		R = set_axis_angle(right, pitch);
	}

	dist = transform_quat(dist, R);
	eye = dist + center;

	update_view_matrix();
}

void Camera::move(vec3 delta)
{
	vec3 local_delta = get_local_vector(delta);
	eye = eye - local_delta;
	center = center - local_delta;
	
	update_view_matrix();
}

void Camera::move(vec2 delta)
{
	vec3 front = normalized(center - eye) * -1.0f;
	vec4 right_aux = inverse(view_matrix) * vec4(1.f, 0.f, 0.f, 0.f);
	vec3 right = vec3(right_aux.x, right_aux.y, right_aux.z);
	vec3 up = normalized(cross(front, right));

	eye = eye + up * delta.y;
	eye = eye + right * delta.x;
	center = center + up * delta.y;
	center = center + right * delta.x;

	update_view_matrix();
}

void Camera::set_orthographic(float left, float right, float top, float bottom, float near_plane, float far_plane)
{
	type = ORTHOGRAPHIC;

	this->left = left;
	this->right = right;
	this->top = top;
	this->bottom = bottom;
	this->near_plane = near_plane;
	this->far_plane = far_plane;

	update_projection_matrix();
}

void Camera::set_perspective(float fov, float aspect, float near_plane, float far_plane)
{
	type = PERSPECTIVE;

	this->fov = fov;
	this->aspect = aspect;
	this->near_plane = near_plane;
	this->far_plane = far_plane;

	this->min_fov = 10;
	this->max_fov = 110;

	update_projection_matrix();
}

void Camera::look_at(const vec3& eye, const vec3& center, const vec3& up)
{
	this->eye = eye;
	this->center = center;
	this->up = up;

	update_view_matrix();
}

void Camera::update_view_matrix()
{
	// Reset Matrix (Identity)
	view_matrix = mat4();

	// Front/Forward vector
	vec3 f = normalized(center - eye) * -1.0f;

	// Right/Side vector
	vec3 r = cross(up, f); // Right handed
	if (r == vec3(0, 0, 0)) {
		std::cout << "Error: View and up vectors are parallel\n";
		return; // Error
	}
	normalize(r);

	// Uo/Top vector
	vec3 u = normalized(cross(f, r)); // Right handed
	vec3 t = vec3(
		-dot(r, eye),
		-dot(u, eye),
		-dot(f, eye)
	);
	view_matrix = mat4(
		// Transpose upper 3x3 matrix to invert it
		r.x, u.x, f.x, 0,
		r.y, u.y, f.y, 0,
		r.z, u.z, f.z, 0,
		t.x, t.y, t.z, 1
	);

	update_viewprojection_matrix();
}

// Create a projection matrix
void Camera::update_projection_matrix()
{
	// Reset Matrix (Identity)
	projection_matrix = mat4();

	if (left == right || top == bottom || near_plane == far_plane) {
		std::cout << "Error: Invalid frustum\n";
		return; // Error
	}

	if (type == PERSPECTIVE) {
		float ymax = near_plane * tanf(fov * 3.14159265359f / 360.0f);
		float xmax = ymax * aspect;
		left = -xmax; right = xmax; bottom = -ymax; top = ymax;
		projection_matrix = mat4(
			(2.0f * near_plane) / (right - left), 0, 0, 0,
			0, (2.0f * near_plane) / (top - bottom), 0, 0,
			(right + left) / (right - left), (top + bottom) / (top - bottom), (-(far_plane + near_plane)) / (far_plane - near_plane), -1,
			0, 0, (-2 * far_plane * near_plane) / (far_plane - near_plane), 0
		);
	}
	else if (type == ORTHOGRAPHIC) {
		projection_matrix = mat4(
			2.0f / (right - left), 0, 0, 0,
			0, 2.0f / (top - bottom), 0, 0,
			0, 0, -2.0f / (far_plane - near_plane), 0,
			-((right + left) / (right - left)), -((top + bottom) / (top - bottom)), -((far_plane + near_plane) / (far_plane - near_plane)), 1
		);
	}

	update_viewprojection_matrix();
}

void Camera::update_viewprojection_matrix()
{
	viewprojection_matrix = projection_matrix * view_matrix;
}

mat4 Camera::get_viewprojection_matrix()
{
	update_view_matrix();
	update_projection_matrix();

	return viewprojection_matrix;
}

void Camera::render_gui()
{
	bool changed = false;
	changed |= ImGui::DragFloat3("Eye", &eye.x, 1, -100, 100);
	changed |= ImGui::DragFloat3("Center", &center.x, 1, -100, 100);
	changed |= ImGui::DragFloat3("Up", &up.x, 1, -100, 100);
	changed |= ImGui::SliderFloat("FOV", &fov, min_fov, max_fov);
	changed |= ImGui::SliderFloat("Near", &near_plane, 0.01f, far_plane);
	changed |= ImGui::SliderFloat("Far", &far_plane, near_plane, 1000.f);

	if (changed) {
		update_view_matrix();
		update_projection_matrix();
	}
}