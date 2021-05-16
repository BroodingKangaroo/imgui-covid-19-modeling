#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <implot.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "settings.h"

struct Intersection {
	static glm::vec2* LEFT;
	static glm::vec2* TOP;
	static glm::vec2* RIGHT;
	static glm::vec2* BOTTOM;
	static glm::vec2* NO_INTERSECTION;
};

glm::vec2* Intersection::LEFT = new glm::vec2(-1, 1);
glm::vec2* Intersection::TOP = new glm::vec2(1, -1);
glm::vec2* Intersection::RIGHT = new glm::vec2(-1, 1);
glm::vec2* Intersection::BOTTOM = new glm::vec2(1, -1);
glm::vec2* Intersection::NO_INTERSECTION = nullptr;

GLFWwindow* create_window(const char* title) {
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit()) {
		puts("Could not initialize GLFW");
		return nullptr;
	}

	//glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
	
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, title, NULL, NULL);
	if (!window) {
		glfwTerminate();
		puts("Could not create GLFW window");
		return nullptr;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		puts("Could not initialize Glad");
		return nullptr;
	}

	// imgui stuff
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}

void reflectVector2(glm::vec2& v, glm::vec2 reflection) {
	v *= reflection;
}