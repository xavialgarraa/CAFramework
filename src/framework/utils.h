/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This contains several functions that can be useful when programming your game.
*/

#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "math/math.h"

#include "includes.h"

//General functions **************
long get_time();
float* snapshot();
bool read_file(const std::string& filename, std::string& content);

//generic purposes fuctions
void draw_grid();
vec3 transform_quat(const vec3& a, const quat& q);

//check opengl errors
bool check_gl_errors();

std::string get_path();

//Vector2 getDesktopSize(int display_index = 0);

std::vector<std::string> tokenize(const std::string& source, const char* delimiters, bool process_strings = false);
std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems);
std::vector<std::string> split(const std::string& s, char delim);
bool replace(std::string& str, const std::string& from, const std::string& to);

//std::string getGPUStats();

//Used in the MESH and ANIM parsers
char* fetch_word(char* data, char* word);
char* fetch_float(char* data, float& f);
char* fetch_matrix4(char* data, mat4& m);
char* fetch_end_line(char* data);
char* fetch_buffer_float(char* data, std::vector<float>& vector, int num = 0);
char* fetch_buffer_vec3(char* data, std::vector<vec3>& vector);
char* fetch_buffer_vec2(char* data, std::vector<vec2>& vector);
char* fetch_buffer_vec3u(char* data, std::vector<vec3>& vector);
char* fetch_buffer_vec3u(char* data, std::vector<unsigned int>& vector);
char* fetch_buffer_vec4ub(char* data, std::vector<ivec4>& vector);
char* fetch_buffer_vec4(char* data, std::vector<vec4>& vector);
