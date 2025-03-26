#include "pose.h"

Pose::Pose() { }

Pose::Pose(unsigned int num_joints)
{
	resize(num_joints);
}

Pose::Pose(const Pose& p)
{
	*this = p;
}

// resize arrays
void Pose::resize(unsigned int size)
{
	parents.resize(size);
	joints.resize(size);
}

// get the number of joints
unsigned int Pose::size()
{
	return joints.size();
}

// set parent id
void Pose::set_parent(unsigned int id, unsigned int parent_id)
{
	parents[id] = parent_id;
}

// get parent id
int Pose::get_parent(unsigned int id)
{
	return parents[id];
}

//set local transform of the joint
void Pose::set_local_transform(unsigned int id, const Transform& transform)
{
	joints[id] = transform;
}

// get local transform of the joint
Transform Pose::get_local_transform(unsigned int id)
{
	return joints[id];
}

// get global (world) transform of the joint
Transform Pose::get_global_transform(unsigned int id)
{
	// use "combine()" function to combine two transforms
	Transform transform = joints[id];
	for (int i = parents[id]; i >= 0; i = parents[i]) {
		transform = combine(joints[i], transform);
	}
	return transform;
}

mat4 Pose::get_global_matrix(unsigned int id)
{
	Transform t = get_global_transform(id);
	return transform_to_mat4(t);
}

Transform Pose::operator[](unsigned int index)
{
	return get_global_transform(index);
}

// get global matrices of the joints
std::vector<mat4> Pose::get_global_matrices()
{
	unsigned int num_joints = size();
	std::vector<mat4> out(num_joints);
	if (out.size() != num_joints) {
		out.resize(num_joints);
	}

	// For every joint, find the global transform (using get_global_transform), convert it into a matrix, and store the result in a vector of matrices
	for (unsigned int i = 0; i < num_joints; i++) {
		Transform t = get_global_transform(i);
		out[i] = transform_to_mat4(t);
	}

	return out;
}