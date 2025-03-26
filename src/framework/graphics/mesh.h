#pragma once

#include <vector>
#include <map>
#include <string>

#include "../math/vec2.h"
#include "../math/vec3.h"
#include "../math/vec4.h"
#include "../math/mat4.h"

class Shader; //for binding
class Image; //for displace
class Skeleton; //for skinned meshes
class Pose;

//version from 21/01/2024
#define MESH_BIN_VERSION 12 //this is used to regenerate bins if the format changes

#define MAX_SUBMESH_DRAW_CALLS 16

class BoundingBox
{
public:
	vec3 center;
	vec3 halfsize;
	BoundingBox() {};
	BoundingBox(vec3 center, vec3 halfsize) : center(center), halfsize(halfsize) {};
};

//applies a transform to a AABB so it is 
BoundingBox transform_bounding_box(const mat4 m, const BoundingBox& box);

struct BoneInfo
{
	char name[32]; //max 32 chars per bone name
	mat4 bind_pose;
};

struct sSubmeshDrawCallInfo
{
	char material[32];
	size_t start; //in primitive
	size_t length; //in primitive
};

struct sSubmeshInfo
{
	unsigned int num_draw_calls;
	char name[32];
	sSubmeshDrawCallInfo draw_calls[MAX_SUBMESH_DRAW_CALLS];
};

struct sMaterialInfo
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
};

class Mesh
{
public:
	static std::map<std::string, Mesh*> s_meshes_loaded;
	static bool use_binary; //always load the binary version of a mesh when possible
	static bool interleave_meshes; //loaded meshes will me automatically interleaved
	static bool auto_upload_to_vram; //loaded meshes will be stored in the VRAM
	static long num_meshes_rendered;
	static long num_triangles_rendered;

	std::string name;

	std::vector<sSubmeshInfo> submeshes; //contains info about every submesh
	std::map<std::string, sMaterialInfo> materials; //contains info about every material

	std::vector<vec3> vertices;	//here we store the vertices
	std::vector<vec3> normals;	//here we store the normals
	std::vector<vec2> uvs;		//here we store the texture coordinates
	std::vector<vec2> uvs1;		//secondary sets of uvs
	std::vector<vec4> colors;	//here we store the colors

	struct tInterleaved
	{
		vec3 vertex;
		vec3 normal;
		vec2 uv;
	};

	std::vector<tInterleaved> interleaved; //to render interleaved

	std::vector<unsigned int> indices; //for indexed meshes

	//for animated meshes
	std::vector<ivec4> bones; //tells which bones afect the vertex (4 max)
	std::vector<vec4> weights; //tells how much affect every bone
	std::vector<BoneInfo> bones_info; //tells 
	mat4 bind_matrix;

	vec3 aabb_min;
	vec3 aabb_max;
	BoundingBox box;

	float radius;

	unsigned int vertices_vbo_id;
	unsigned int uvs_vbo_id;
	unsigned int normals_vbo_id;
	unsigned int colors_vbo_id;

	unsigned int indices_vbo_id;
	unsigned int interleaved_vao_id;
	unsigned int interleaved_vbo_id;
	unsigned int bones_vbo_id;
	unsigned int weights_vbo_id;
	unsigned int uvs1_vbo_id;

	Mesh();
	~Mesh();

	void clear();

	void cpu_skinning(Skeleton* skeleton, Pose pose);

	void render(unsigned int primitive, int submesh_id = -1, int num_instances = 0);
	void render_instanced(unsigned int primitive, const mat4* instanced_models, int number);
	void render_instanced(unsigned int primitive, const std::vector<vec3> positions, const char* uniform_name);
	void render_bounding(const mat4& model, bool world_bounding = true);
	void render_fixed_pipeline(int primitive); //sloooooooow

	void enable_buffers(Shader* shader);
	void draw_call(unsigned int primitive, int submesh_id, int draw_call_id, int num_instances);
	void disable_buffers(Shader* shader);

	bool read_bin(const char* filename);
	bool write_bin(const char* filename);

	unsigned int get_num_submeshes() { return (unsigned int)submeshes.size(); }
	unsigned int get_num_vertices() { return (unsigned int)interleaved.size() ? (unsigned int)interleaved.size() : (unsigned int)vertices.size(); }

	//collision testing
	void* collision_model;
	//bool createCollisionModel(bool is_static = false); //is_static sets if the inv matrix should be computed after setTransform (true) or before rayCollision (false)
	////help: model is the transform of the mesh, ray origin and direction, a Vector3 where to store the collision if found, a Vector3 where to store the normal if there was a collision, max ray distance in case the ray should go to infintiy, and in_object_space to get the collision point in object space or world space
	//bool testRayCollision(Matrix44 model, Vector3 ray_origin, Vector3 ray_direction, Vector3& collision, Vector3& normal, float max_ray_dist = 3.4e+38F, bool in_object_space = false);
	//bool testSphereCollision(Matrix44 model, Vector3 center, float radius, Vector3& collision, Vector3& normal);

	//loader
	static Mesh* get(const char* filename);
	void register_mesh(std::string name);

	//create help meshes
	void create_quad(float center_x, float center_y, float w, float h, bool flip_uvs);
	void create_plane(float size);
	void create_subdivided_plane(float size = 1, int subdivisions = 256, bool centered = false);
	void create_cube();
	void create_wire_box();
	void create_grid(float dist);
	void displace(Image* heightmap, float altitude);
	static Mesh* get_quad(); //get global quad

	void update_bounding_box();

	//optimize meshes
	void upload_to_vram();
	template <typename T>
	void upload_attributes_to_vram(const std::vector<T> values, unsigned int& id);

	bool interleave_buffers();

private:
	//bool loadASE(const char* filename);
	bool load_obj(const char* filename);
	bool parse_mtl(const char* filename);
	bool load_mesh(const char* filename); //personal format used for animations
};