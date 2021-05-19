#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <implot.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "settings.h"

struct GraphValues
{
	std::vector<float>susceptible;
	std::vector<float>infected;
	std::vector<float>recovered;
	std::vector<float>dead;
	std::vector<float>time;
	bool continue_drawing = true;

	void update(float susceptible_, float infected_, float recovered_, float dead_, float time_) {
		if (continue_drawing) {
			susceptible.push_back(susceptible_);
			infected.push_back(infected_);
			recovered.push_back(recovered_);
			dead.push_back(dead_);
			time.push_back(time_);
		}
	}

	void clearGraphData() {
		susceptible.clear();
		infected.clear();
		recovered.clear();
		dead.clear();
		time.clear();
	}

	void drawData() {
		ImGui::Begin("Graph");
		ImGui::Checkbox("Continue drawing", &continue_drawing);
		ImGui::SameLine();
		if (ImGui::Button("Clear graph")) {
			clearGraphData();
		}
		static ImPlotAxisFlags xflags = ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit;
		static ImPlotAxisFlags yflags = ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit;
		if (ImPlot::BeginPlot("My Plot", "time", "people", ImVec2(700, 400), 0, xflags, yflags)) {

			ImPlot::PlotLine("Susceptible", time.data(), susceptible.data(), susceptible.size());
			ImPlot::PlotLine("Infected", time.data(), infected.data(), infected.size());
			ImPlot::PlotLine("Recovered", time.data(), recovered.data(), recovered.size());
			ImPlot::PlotLine("Dead", time.data(), dead.data(), dead.size());
			ImPlot::EndPlot();
		}
		ImGui::End();
	}
};

/**
 * Control time withing the program.
 * Makes time be able to slow down or hurry up depending on the scale provided in update() function.
 */
struct TimeController
{
	float previous_update_time;
	float scaled_current_time;

	TimeController() {
		previous_update_time = glfwGetTime();
		scaled_current_time = previous_update_time;
	}
	
	void update(const float& scale) {
		const auto current_time = static_cast<float>(glfwGetTime());
		scaled_current_time += (current_time - previous_update_time) * scale;
		previous_update_time = current_time;
	}
};

struct WindowCoordinates {
	glm::vec2 top_left_corner{};
	int height{};
	int width{};

	WindowCoordinates(){}
	
	WindowCoordinates(glm::vec<2, float, glm::defaultp> top_left_corner_, int height_, int width_) :
		top_left_corner(top_left_corner_),
		height(height_),
		width(width_) {}
};

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

GLFWwindow* GLFWBeginRendering(const char* title) {
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

	return window;
}

void IMGUIBeginRendering(GLFWwindow* window) {
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
}

void reflectVector2(glm::vec2& v, glm::vec2 reflection) {
	v *= reflection;
}