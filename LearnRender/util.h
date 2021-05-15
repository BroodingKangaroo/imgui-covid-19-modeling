#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

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
	if (!glfwInit())
		return nullptr;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, title, NULL, NULL);
	if (!window) {
		glfwTerminate();
		return nullptr;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	// imgui stuff
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}

void invertVector2(glm::vec2& v, glm::vec2 intersection) {
	v *= intersection;
}