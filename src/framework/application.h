#pragma once

#include "math/vec2.h"

#include "includes.h"

#include "camera.h"
#include "framework/entity.h"

class Application
{
public:
	static Application* instance;
	
	static Camera* camera;
	std::vector<Entity*> entity_list;

	int window_width;
	int window_height;

	bool flag_grid;
	bool flag_wireframe;

	bool close = false;
	bool orbiting;
	bool moving_2D;
	vec2 mouse_position;
	vec2 last_mouse_position;

	void init(GLFWwindow* window);
	void update(float dt);
	void render();
	void render_gui();
	void shut_down();

	void on_key_down(int key, int scancode);
	void on_key_up(int key, int scancode);
	void on_right_mouse_down();
	void on_right_mouse_up();
	void on_left_mouse_down();
	void on_left_mouse_up();
	void on_middle_mouse_down();
	void on_middle_mouse_up();
	void on_mouse_position(double xpos, double ypos);
	void on_scroll(double xOffset, double yOffset);
};