#pragma once

#include "camera.h"
#include "framework/utils.h"
#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "graphics/material.h"

#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat4.h"

#include "animations/pose.h"
#include "animations/skeleton.h"

class Entity
{
protected:
	static unsigned int name_id_counter;

	mat4 model;
	Transform transform;
	vec3 gui_rotation;

public:
	std::string name;

	bool flag_visible;
	bool flag_update;
	bool flag_apply_parent_transform;

	template <typename ChildEntity>
	ChildEntity* as() {
		return dynamic_cast<ChildEntity*>(this);
	}

	Mesh* mesh = nullptr;
	Material* material = nullptr;

	Entity* parent = nullptr;
	std::vector<Entity*> children;

	Entity(const char* _name = nullptr);

	virtual void render(Camera* camera);
	virtual void update(float dt);
	virtual void render_gui();

	mat4 get_model();
	Transform get_transform();

	void set_model(const mat4& m);
	void set_transform(const Transform& t);
	void set_children(std::vector<Entity*> children);
};

class LineHelper : public Entity
{
public:
	vec3 origin;	// world position
	vec3 end;		// relative position to the origin position

	vec4 color;

	bool unlocked = true;

	// (world position, relative position to the origin position, Entity name in the GUI)
	LineHelper(vec3 origin, vec3 end, const char* name = nullptr);

	void render(Camera* camera);
	void update(float dt);
	void render_gui();
};

class SkeletonHelper : public Entity
{
public:
	vec4 color;
	bool flag_editable;

	struct Bone {
		std::string name;
		std::vector<unsigned int> children;
	};

	std::map<unsigned int, Bone> bones;

	Pose* pose = nullptr;

	SkeletonHelper(Pose& pose, const char* _name = nullptr);
	SkeletonHelper(Skeleton& skeleton, const char* _name = nullptr);
	~SkeletonHelper();

	void render(Camera* camera);
	void update(float dt);
	void render_gui();

	void set_pose(Pose* pose, bool editable = true);
	void render_gui_bone(unsigned int id, Pose& pose, Bone bone);
};

class SkinnedEntity : public Entity
{
public:
	Skeleton* skeleton = nullptr;

	SkeletonHelper* skeleton_helper = nullptr;
	bool flag_apply_bind_pose;

	std::vector<mat4> pose_mat_joint_space;

	SkinnedEntity(const char* _name = nullptr);

	void render(Camera* camera);
	void update(float dt);
	void render_gui();

	void set_skeleton(const Pose& rest, const Pose& bind, const std::vector<std::string>& names);
};