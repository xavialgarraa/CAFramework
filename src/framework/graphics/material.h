#pragma once

#include "../camera.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"

#include "../math/vec4.h"
#include "../math/mat4.h"

struct Uniforms {
	mat4 model;
	Camera* camera = nullptr;
	std::vector<mat4> animated_matrices;
};

class Material {
public:
	
	Shader* shader = NULL;
	Texture* texture = NULL;
	vec4 color;

	virtual void set_uniforms(Uniforms& uniforms) = 0;
	virtual void render(Mesh* mesh, Uniforms& uniforms) = 0;
	virtual void render_gui() = 0;
};

class FlatMaterial : public Material {
public:

	FlatMaterial(vec4 color = vec4(1.f));
	~FlatMaterial();

	void set_uniforms(Uniforms& uniforms);
	void render(Mesh* mesh, Uniforms& uniforms);
	void render_gui();
};

class NormalMaterial : public FlatMaterial {
public:

	NormalMaterial();
	void render_gui();
};

class PBRMaterial : public FlatMaterial {
public:
	Texture* albedo_tex = NULL;
	Texture* normal_tex = NULL;
	Texture* met_rou_tex = NULL;

	float metallic;
	float roughness;

	PBRMaterial();
	void set_uniforms(Uniforms& uniforms);
	void render_gui();
};

class WireframeMaterial : public FlatMaterial {
public:

	WireframeMaterial();
	~WireframeMaterial();

	void render(Mesh* mesh, Uniforms& uniforms);
};