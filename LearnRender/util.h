#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <implot.h>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "settings.h"

struct GraphData
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
		continue_drawing = true;
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

struct Coordinates {
	glm::vec2 top_left_corner{};
	int height{};
	int width{};

	Coordinates() = default;

	Coordinates(glm::vec<2, float, glm::defaultp> top_left_corner_, int height_, int width_) :
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

std::string getTimesStamp() {
	std::time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm newtime;
	localtime_s(&newtime, &current_time);
	std::ostringstream ss;
	ss << newtime.tm_mday << "-" << newtime.tm_mon << "-" << newtime.tm_hour << "-" << newtime.tm_min << "-" << newtime.tm_sec;
	return ss.str();
}

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

bool isOverlap(Coordinates a, Coordinates b) {
	const glm::vec2 leftA = a.top_left_corner, rightA = glm::vec2(a.top_left_corner + glm::vec2(a.width, a.height));
	const glm::vec2 leftB = b.top_left_corner, rightB = glm::vec2(b.top_left_corner + glm::vec2(b.width, b.height));
	if (leftA.x > rightB.x || rightA.x < leftB.x || leftA.y > rightB.y || rightA.y < leftB.y) {
		return false;
	}
	return true;
}