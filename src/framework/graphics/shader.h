/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This allows to use compile and use shaders when rendering. Used for advanced lighting.
*/

#pragma once

#include "framework/includes.h"
#include "../math/vec2.h"
#include "../math/vec3.h"
#include "../math/vec4.h"
#include "../math/mat4.h"


#include <string>
#include <vector>
#include <map>
#include <cassert>

#ifdef _DEBUG
#define CHECK_SHADER_VAR(a,b) if (a == -1) return
//#define CHECK_SHADER_VAR(a,b) if (a == -1) { std::cout << "Shader error: Var not found in shader: " << b << std::endl; return; } 
#else
#define CHECK_SHADER_VAR(a,b) if (a == -1) return
#endif

class Texture;

class Shader
{
	int last_slot;

	static bool s_ready; //used to initialize shader vars

public:
	static Shader* current;

	Shader();
	virtual ~Shader();

	virtual void set_filenames(const std::string& vsf, const std::string& psf); //set but not compile
	virtual bool compile();
	virtual bool recompile();

	virtual bool load(const std::string& vsf, const std::string& psf, const char* macros);

	//internal functions
	virtual bool compile_from_memory(const std::string& vsm, const std::string& psm);
	virtual void release();
	virtual void enable();
	virtual void disable();

	static void init();
	static void disable_shaders();

	//check
	virtual bool is_uniform(const char* varname) { return (get_uniform_location(varname) != -1); } //uniform exist
	virtual bool is_attribute(const char* varname) { return (get_attribute_location(varname) != -1); } //attribute exist

	//upload
	void set_uniform(const char* varname, bool input) { assert(current == this); set_uniform1(varname, input); }
	void set_uniform(const char* varname, int input) { assert(current == this); set_uniform1(varname, input); }
	void set_uniform(const char* varname, float input) { assert(current == this); set_uniform1(varname, input); }
	void set_uniform(const char* varname, const vec2& input) { assert(current == this); set_uniform2(varname, input.x, input.y); }
	void set_uniform(const char* varname, const vec3& input) { assert(current == this); set_uniform3(varname, input.x, input.y, input.z); }
	void set_uniform(const char* varname, const vec4& input) { assert(current == this); set_uniform4(varname, input.x, input.y, input.z, input.w); }
	void set_uniform(const char* varname, const mat4& input) { assert(current == this); set_matrix4(varname, input); }
	void set_uniform(const char* varname, std::vector<mat4>& m_vector) { assert(current == this && m_vector.size()); set_matrix4_array(varname, &m_vector[0], static_cast<int>(m_vector.size())); }

	//for textures you must specify an slot (a number from 0 to 16) where this texture is stored in the shader
	void set_uniform(const char* varname, Texture* texture, int slot) { assert(current == this); set_texture(varname, texture, slot); }


	virtual void setInt(const char* varname, const int& input) { set_uniform1(varname, input); }
	virtual void setFloat(const char* varname, const float& input) { set_uniform1(varname, input); }
	virtual void setVector3(const char* varname, const vec3& input) { set_uniform3(varname, input.x, input.y, input.z); }
	virtual void set_matrix4(const char* varname, const float* m);
	virtual void set_matrix4(const char* varname, const mat4& m);
	virtual void set_matrix4_array(const char* varname, mat4* m_array, int num);

	virtual void set_uniform1_array(const char* varname, const float* input, const int count);
	virtual void set_uniform2_array(const char* varname, const float* input, const int count);
	virtual void set_uniform3_array(const char* varname, const float* input, const int count);
	virtual void set_uniform4_array(const char* varname, const float* input, const int count);

	virtual void set_uniform1_array(const char* varname, const int* input, const int count);
	virtual void set_uniform2_array(const char* varname, const int* input, const int count);
	virtual void set_uniform3_array(const char* varname, const int* input, const int count);
	virtual void set_uniform4_array(const char* varname, const int* input, const int count);

	virtual void set_uniform1(const char* varname, const bool input1);

	virtual void set_uniform1(const char* varname, const int input1);
	virtual void set_uniform2(const char* varname, const int input1, const int input2);
	virtual void set_uniform3(const char* varname, const int input1, const int input2, const int input3);
	virtual void set_uniform3(const char* varname, const vec3& input) { set_uniform3(varname, input.x, input.y, input.z); }
	virtual void set_uniform4(const char* varname, const int input1, const int input2, const int input3, const int input4);

	virtual void set_uniform1(const char* varname, const float input);
	virtual void set_uniform2(const char* varname, const float input1, const float input2);
	virtual void set_uniform3(const char* varname, const float input1, const float input2, const float input3);
	virtual void set_uniform4(const char* varname, const vec4& input) { set_uniform4(varname, input.x, input.y, input.z, input.w); }
	virtual void set_uniform4(const char* varname, const float input1, const float input2, const float input3, const float input4);

	//virtual void set_texture(const char* varname, const unsigned int tex) ;
	virtual void set_texture(const char* varname, Texture* texture, int slot);

	virtual int get_attribute_location(const char* varname);
	virtual int get_uniform_location(const char* varname);

	std::string get_info_log() const;
	bool has_info_log() const;
	bool compiled;

	void set_macros(const char* macros);

	static Shader* get(const char* vsf, const char* psf = NULL, const char* macros = NULL);
	static void reload_all();
	static std::map<std::string, Shader*> s_Shaders;

	//this is a way to load a single file that contains all the shaders 
	//to know more about the file format, it is based in this https://github.com/jagenjo/rendeer.js/tree/master/guides#the-shaders but with tiny differences
	static bool load_atlas(const char* filename);
	static std::string s_shader_atlas_filename;
	static std::map<std::string, std::string> s_shaders_atlas; //stores strings, no shaders

	static Shader* get_default_shader(std::string name);

protected:

	std::string info_log;
	std::string vs_filename;
	std::string ps_filename;
	std::string macros;
	bool from_atlas;

	bool create_vertex_shader_object(const std::string& shader);
	bool create_fragment_shader_object(const std::string& shader);
	bool create_shader_object(unsigned int type, GLuint& handle, const std::string& shader);
	void save_shader_info_log(GLuint obj);
	void save_program_info_log(GLuint obj);

	bool validate();

	GLuint vs;
	GLuint fs;
	GLuint program;
	std::string log;

	//this is a hack to speed up shader usage (save info locally)
private:

	struct ltstr
	{
		bool operator()(const char* s1, const char* s2) const
		{
			return strcmp(s1, s2) < 0;
		}
	};
	typedef std::map<const char*, int, ltstr> loctable;

public:
	GLint get_location(const char* varname, loctable* table);
	loctable locations;
};