#include "entity.h"

#include "application.h"
#include "utils.h"

#include "ImGuizmo.h"

#include <istream>
#include <fstream>
#include <algorithm>

unsigned int Entity::name_id_counter = 0;

Entity::Entity(const char* _name)
{
	if (_name && *_name) { name = _name; }
	else { name = "Entity_" + std::to_string(++name_id_counter); }

	flag_visible = true;
	flag_update = false;
	flag_apply_parent_transform = true;

	model = mat4();
	transform = Transform();
	gui_rotation = vec3();

	children.resize(0);
}

void Entity::render(Camera* camera)
{
	if (flag_visible) {
		if (material) {
			Uniforms uniforms;
			uniforms.camera = camera;
			uniforms.model = model;

			if (parent) {
				uniforms.model = model * parent->model;
			} 
			material->render(mesh, uniforms);			
		}

		if (children.size() > 0) {
			for (unsigned int i = 0; i < children.size(); i++) {				
				children[i]->render(camera);			
			}
		}
	}
}

void Entity::update(float dt)
{
	if (children.size() > 0) {
		for (unsigned int i = 0; i < children.size(); i++) {
			children[i]->update(dt);			
		}
	}
}

void Entity::render_gui()
{
	ImGui::Checkbox("Visible", &flag_visible);
	
	// Transform
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(230, 150, 50)));
	bool is_open = ImGui::TreeNodeEx("Transform");
	ImGui::PopStyleColor();
	if (is_open) {
		bool changed = false;
		changed |= ImGui::DragFloat3("Translation", transform.position.v, 0.1f);
		changed |= ImGui::DragFloat3("Rotation", gui_rotation.v, 1.f, -360.f, 360.f);
		changed |= ImGui::DragFloat3("Scale", transform.scale.v, 0.1f);
		
		if (changed) {
			transform.rotation = euler_to_quat(gui_rotation.x * QUAT_DEG2RAD, gui_rotation.y * QUAT_DEG2RAD, gui_rotation.z * QUAT_DEG2RAD);
			model = transform_to_mat4(transform);
		}

		ImGui::TreePop();
	}

	// Material
	if (material) {
		if (ImGui::TreeNode("Material")) {
			material->render_gui();
			ImGui::TreePop();
		}
	}
	
	// Children
	if (children.size() > 0) {
		if (ImGui::TreeNode("Children")) {
			for (unsigned int i = 0; i < children.size(); i++) {
				if (ImGui::TreeNode(children[i]->name.c_str())) {
					children[i]->render_gui();
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
}

mat4 Entity::get_model()
{
	return model;
}

Transform Entity::get_transform()
{
	return transform;
}

void Entity::set_model(const mat4& m)
{
	model = m;
	transform = mat4_to_transform(m);

	// also update the gui information
	gui_rotation = quat_to_euler(transform.rotation);
}

void Entity::set_transform(const Transform& t)
{
	transform = t;
	model = transform_to_mat4(t);

	// also update the gui information
	gui_rotation = quat_to_euler(transform.rotation);
}

void Entity::set_children(std::vector<Entity*> entities)
{
	for(unsigned int i = 0; i < entities.size(); i++) {
		entities[i]->parent = this;
	}
	children = entities;
}

LineHelper::LineHelper(vec3 origin, vec3 end, const char* _name) : origin(origin), end(end), Entity(_name)
{
	if (!(_name && *_name)) { name = "LineHelper_" + std::to_string(name_id_counter); }

	color = vec4(1.f);

	// create mesh
	mesh = new Mesh();

	mesh->vertices.push_back(vec3(origin.x, origin.y, origin.z));
	mesh->vertices.push_back(vec3(end.x + origin.x, end.y + origin.y, end.z + origin.z));

	mesh->colors.push_back(vec4(color.x, color.y, color.z, color.w));
	mesh->colors.push_back(vec4(color.x, color.y, color.z, color.w));

	mesh->upload_to_vram();
}

void LineHelper::render(Camera* camera)
{
	WireframeMaterial mat = WireframeMaterial();
	mat.color = vec4(color.x, color.y, color.z, color.w);
	
	if (flag_visible && mat.shader && mesh) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		//enable shader
		mat.shader->enable();

		Uniforms uniforms;
		uniforms.camera = camera;
		uniforms.model = model;

		//upload material specific uniforms
		mat.set_uniforms(uniforms);

		//do the draw call
		mesh->render(GL_LINES);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void LineHelper::update(float dt)
{
	if (flag_update) {
		// update the mesh info
		mesh->vertices[0] = vec3(origin.x, origin.y, origin.z);
		mesh->vertices[1] = vec3(end.x + origin.x, end.y + origin.y, end.z + origin.z);
		mesh->colors[0] = vec4(color.x, color.y, color.z, color.w);
		mesh->colors[1] = vec4(color.x, color.y, color.z, color.w);
		mesh->upload_to_vram();
		
		flag_update = false;
	}
}

void LineHelper::render_gui()
{
	ImGui::Checkbox("Unlock Line", &unlocked);

	ImGui::BeginDisabled(!unlocked);

	flag_update |= ImGui::DragFloat3("Vector Origin", origin.v, 0.1f);
	flag_update |= ImGui::DragFloat3("Vector Target", end.v, 0.1f, -1.f, 1.f);
	normalize(end); // unitary vector

	ImGui::ColorEdit3("Line color", color.v);
	
	ImGui::EndDisabled();
}

SkeletonHelper::SkeletonHelper(Pose& current_pose, const char* _name) : Entity(_name)
{
	if (!(_name && *_name)) { name = "SkeletonHelper_" + std::to_string(name_id_counter); }
	
	pose = new Pose(current_pose);
	color = vec4(1.f);
	flag_editable = true;
	flag_apply_parent_transform = true;
	
	for (int i = 0; i < pose->size(); i++) {
		if (!bones.contains(i)) {
			Bone bone;
			bone.name = "Bone_" + std::to_string(i);
			bones[i] = bone;
		}

		int parent_id = pose->get_parent(i);

		if (parent_id < 0) continue; // skip joints without parent 		

		if (!bones.contains(parent_id)) {
			Bone parent;
			parent.name = "Bone_" + std::to_string(parent_id);
			bones[parent_id] = parent;
		}
		bones[parent_id].children.push_back(i);
	}

	set_pose(pose);
}

SkeletonHelper::SkeletonHelper(Skeleton& skeleton, const char* _name) : Entity(_name)
{
	if (!(_name && *_name)) { name = "SkeletonHelper_" + std::to_string(name_id_counter); }

	color = vec4(1.f);
	flag_editable = true;

	std::vector<std::string> names = skeleton.get_joint_names();
	Pose* pose = &skeleton.get_rest_pose();

	for (int i = 0; i < pose->size(); i++) {
		if (!bones.contains(i)) {
			Bone bone;
			bone.name = names[i];
			bones[i] = bone;
		}

		int parent_id = pose->get_parent(i);

		if (parent_id < 0) continue; // skip joints without parent 		

		if (!bones.contains(parent_id)) {
			Bone parent;
			parent.name = names[parent_id];
			bones[parent_id] = parent;
		}
		bones[parent_id].children.push_back(i);
	}

	set_pose(pose);	
}

SkeletonHelper::~SkeletonHelper()
{
	if (pose) {
		delete pose;
		pose = nullptr;
	}
}

void SkeletonHelper::render(Camera* camera)
{
	WireframeMaterial mat = WireframeMaterial();
	mat.color = vec4(color.x, color.y, color.z, color.w);

	mat4 model_mat = model;
	if (parent && flag_apply_parent_transform) {
		model_mat = model_mat * parent->get_model();
	}

	if (flag_visible && mat.shader && mesh)	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		//enable shader
		mat.shader->enable();

		Uniforms uniforms;
		uniforms.camera = camera;
		uniforms.model = model_mat;

		//upload material specific uniforms
		mat.set_uniforms(uniforms);

		//do the draw call
		mesh->render(GL_LINES);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void SkeletonHelper::update(float dt) 
{
	if (pose) {
		mesh->clear();

		// ..

		mesh->upload_to_vram();
	}
}

void SkeletonHelper::set_pose(Pose* current_pose, bool editable)
{
	// create mesh from the skeleton
	pose = current_pose;
	flag_editable = editable;
	mesh = new Mesh();
	
	update(0.f);
}


void SkeletonHelper::render_gui()
{
	Entity::render_gui();
	
	if (flag_visible) {
		ImGui::ColorEdit3("Line color", color.v);
	}

	if (ImGui::TreeNode("Skeleton")) {
		unsigned int root_id = 0;
		render_gui_bone(root_id, *pose, bones[root_id]);		

		ImGui::TreePop();
	}
}

void SkeletonHelper::render_gui_bone(unsigned int id, Pose& pose, Bone bone)
{
	if (ImGui::TreeNodeEx(bone.name.c_str())) {

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(230, 150, 50)));
		bool is_open = ImGui::TreeNodeEx("Transform");
		ImGui::PopStyleColor();

		bool changed = false;
		Transform transform = pose.get_local_transform(id);
		
		if (is_open) {
			changed = false;
			changed |= ImGui::DragFloat3("Translation", transform.position.v, flag_editable ? 0.1f : 0.f);
			vec3 rotation = quat_to_euler(transform.rotation);
			changed |= ImGui::DragFloat3("Rotation", rotation.v, flag_editable ? 0.1f : 0.f);
			changed |= ImGui::DragFloat3("Scale", transform.scale.v, flag_editable ? 0.1f : 0.f);

			if (changed) {
				transform.rotation = euler_to_quat(rotation.x, rotation.y, rotation.z);
				pose.set_local_transform(id, transform);
			}

			ImGui::TreePop();
		}

		for (unsigned int i = 0; i < bone.children.size(); i++) {
			Bone child = bones[bone.children[i]];
			render_gui_bone(bone.children[i], pose, child);
		}

		ImGui::TreePop();
	}
}

SkinnedEntity::SkinnedEntity(const char* _name) : Entity(_name)
{
	if (!(_name && *_name)) { name = "SkinnedEntity_" + std::to_string(name_id_counter); }
	
	flag_apply_bind_pose = false;
}

void SkinnedEntity::render(Camera* camera)
{
	if (flag_visible) {
		if (material) {
			Uniforms uniforms;
			uniforms.camera = camera;
			uniforms.model = model;
			
			if (parent && flag_apply_parent_transform) {
				uniforms.model = model * parent->get_model();
			}
			
			material->render(mesh, uniforms);
		}

		if (children.size() > 0) {
			for (unsigned int i = 0; i < children.size(); i++) {
				children[i]->render(camera);
			}
		}

	}
	if (skeleton_helper) {
		skeleton_helper->render(camera);
	}
}

void SkinnedEntity::update(float dt)
{
	if (children.size() > 0) {
		for (unsigned int i = 0; i < children.size(); i++) {
			children[i]->update(dt);
		}
	}

	if (mesh && skeleton) {
		Pose current_pose = skeleton->get_rest_pose();
		if (parent && parent->as<SkinnedEntity>()->flag_apply_bind_pose) {
			current_pose = skeleton->get_bind_pose();
		}

		// CPU Skinning
		// ..

		// GPU Skinning
		// ..
	}
	if (skeleton_helper) {
		skeleton_helper->update(dt);
	}
}

void SkinnedEntity::render_gui()
{
	Entity::render_gui();

	if (skeleton_helper) {
		if (ImGui::Checkbox("Show bind pose", &flag_apply_bind_pose)) {
			if (flag_apply_bind_pose) {
				flag_apply_parent_transform = true;
				skeleton_helper->set_pose(&skeleton->get_bind_pose(), false);
			}
			else {
				flag_apply_parent_transform = true;
				skeleton_helper->set_pose(&skeleton->get_rest_pose());
			}

			for (unsigned int i = 0; i < children.size(); i++) {
				children[i]->flag_apply_parent_transform = flag_apply_parent_transform;
			}
		}

		skeleton_helper->flag_apply_parent_transform = flag_apply_parent_transform;
		if (ImGui::TreeNode(skeleton_helper->name.c_str())) {
			skeleton_helper->render_gui();
			ImGui::TreePop();
		}
	}
}

void SkinnedEntity::set_skeleton(const Pose& rest, const Pose& bind, const std::vector<std::string>& names)
{
	skeleton = new Skeleton(rest, bind, names);
	skeleton_helper = new SkeletonHelper(*skeleton, (name + "_helper").c_str());
	skeleton_helper->parent = this;

	for (unsigned int i = 0; i < children.size(); i++) {
		children[i]->as<SkinnedEntity>()->skeleton = skeleton;
	}
}