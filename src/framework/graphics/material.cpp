#include "material.h"

#include "framework/application.h"

#include <istream>
#include <fstream>
#include <algorithm>

#include "../math/vec3.h"

FlatMaterial::FlatMaterial(vec4 color)
{
	this->color = color;
	shader = Shader::get("res/shaders/basic.vs", "res/shaders/flat.fs");
}

FlatMaterial::~FlatMaterial() { }

void FlatMaterial::set_uniforms(Uniforms& uniforms)
{
	//upload node uniforms
	shader->set_uniform("u_viewprojection", uniforms.camera->viewprojection_matrix);
	shader->set_uniform("u_camera_position", uniforms.camera->eye);
	shader->set_uniform("u_model", uniforms.model);
	if (uniforms.animated_matrices.size()) {
		shader->set_uniform("u_animated", uniforms.animated_matrices);
	}
	shader->set_uniform("u_color", color);
}

void FlatMaterial::render(Mesh* mesh, Uniforms& uniforms)
{
	if (mesh && shader) {
		// enable shader
		shader->enable();
		
		// upload uniforms
		set_uniforms(uniforms);

		// do the draw call
		mesh->render(GL_TRIANGLES);

		shader->disable();
	}
}

void FlatMaterial::render_gui()
{
	ImGui::ColorEdit3("Color", (float*)&color);
}

NormalMaterial::NormalMaterial()
{
	shader = Shader::get("res/shaders/basic.vs", "res/shaders/normal.fs");
}

void NormalMaterial::render_gui() { }

PBRMaterial::PBRMaterial()
{
	metallic = 1.f;
	roughness = 1.f;

	shader = Shader::get("res/shaders/basic.vs", "res/shaders/texture.fs");
}

void PBRMaterial::set_uniforms(Uniforms& uniforms)
{
	//upload node uniforms
	shader->set_uniform("u_viewprojection", uniforms.camera->viewprojection_matrix);
	shader->set_uniform("u_camera_position", uniforms.camera->eye);
	shader->set_uniform("u_model", uniforms.model);

	if (uniforms.animated_matrices.size()) {
		shader->set_uniform("u_animated", uniforms.animated_matrices);
	}

	if (albedo_tex) shader->set_uniform("u_texture", albedo_tex, 0);
	//if (normal_tex) shader->set_uniform("u_normal_tex", normal_tex, 1);
	//if (met_rou_tex) shader->set_uniform("u_met_rou_tex", met_rou_tex, 2);
}

void PBRMaterial::render_gui() { }

WireframeMaterial::WireframeMaterial()
{
	color = vec4(1.f);

	shader = Shader::get("res/shaders/basic.vs", "res/shaders/flat.fs");
}

WireframeMaterial::~WireframeMaterial() { }

void WireframeMaterial::render(Mesh* mesh, Uniforms& uniforms)
{
	if (shader && mesh)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);

		//enable shader
		shader->enable();

		//upload material specific uniforms
		set_uniforms(uniforms);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}