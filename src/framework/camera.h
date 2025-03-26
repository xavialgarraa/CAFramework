#pragma once

#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat4.h"
#include "math/quat.h"

class Camera
{
public:
	static Camera* current;

	// Types of cameras available
	enum { PERSPECTIVE, ORTHOGRAPHIC };
	char type;

	// Vectors to define the orientation of the camera
	vec3 eye;		// Where is the camera
	vec3 center;	// Where is it pointing
	vec3 up;		// The up pointing up

	// Properties of the projection of the camera
	float fov;			// View angle in degrees (1/zoom)
	float min_fov;
	float max_fov;
	float aspect;		// Aspect ratio (width/height)
	float near_plane;	// Near plane
	float far_plane;	// Far plane

	// For orthogonal projection
	float left, right, top, bottom;

	// Matrices
	mat4 view_matrix;
	mat4 projection_matrix;
	mat4 viewprojection_matrix;

	Camera();

	// Setters
	void set_aspect_ratio(float aspect) { this->aspect = aspect; };

	// Translate and rotate the camera
	void move(vec3 delta);
	void move(vec2 delta);
	void rotate(float angle, const vec3& axis);
	vec3 transform_quat(const vec3& a, const quat& q); // check glm implmentation
	quat set_axis_angle(const vec3& axis, const float angle); // check glm implmentation
	void orbit(float yaw, float pitch);

	// Transform a local camera vector to world coordinates
	vec3 get_local_vector(const vec3& v);

	// Project 3D Vectors to 2D Homogeneous Space
	// If negZ is true, the projected point IS NOT inside the frustum, 
	// so it does not have to be rendered!
	vec3 project_vector(vec3 pos, bool& negZ);

	// Set the info for each projection
	void set_perspective(float fov, float aspect, float near_plane, float far_plane);
	void set_orthographic(float left, float right, float top, float bottom, float near_plane, float far_plane);
	void look_at(const vec3& eye, const vec3& center, const vec3& up);

	// Compute the matrices
	void update_view_matrix();
	void update_projection_matrix();
	void update_viewprojection_matrix();

	mat4 get_viewprojection_matrix();

	void render_gui();
};