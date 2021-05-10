#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

int VIEWPORT_WIDTH = 1280;
int VIEWPORT_HEIGHT = 720;

float SIMULATION_SPEED = 0.f;

ImColor DEFAULT_COLOR = ImColor(255, 255, 0);
ImColor COVID_COLOR = ImColor(255, 0, 0);

struct Circle
{
	glm::vec2 center = glm::vec2(10.f, 10.f);
	float radius = 1.f;
	bool covid = false;
	glm::vec2 direction = glm::vec2(0.f, 0.f);
};

bool intersect(const Circle& c1, const Circle& c2)
{
	glm::vec2 diff = c2.center - c1.center;
	return diff.x * diff.x + diff.y * diff.y <= 4 * c1.radius * c1.radius;
}

bool outsideViewport(const Circle& c)
{
	return c.center.x + c.radius > VIEWPORT_WIDTH ||
		c.center.y + c.radius > VIEWPORT_HEIGHT ||
		c.center.x - c.radius < 0 ||
		c.center.y - c.radius < 0;

}

void invertVector2(glm::vec2& v)
{
	v *= -1.f;
}

void moveCircles(std::vector<Circle>& circles, float dt)
{
	for (auto& circle : circles)
	{
		glm::vec2 oldCenter = circle.center;
		glm::vec2 newCenter = circle.center + circle.direction * dt * SIMULATION_SPEED;
		circle.center = newCenter;
		if (outsideViewport(circle))
		{
			circle.center = oldCenter;
			invertVector2(circle.direction);
		}

		circle.center += circle.direction * dt * SIMULATION_SPEED;
	}
}

void markIntersectionCircles(std::vector<Circle>& circles)
{
	for (auto& covidCircle : circles)
	{
		if (covidCircle.covid)
		{
			for (auto& circle : circles)
			{
				if (!circle.covid)
				{
					circle.covid = intersect(covidCircle, circle);
				}
			}
		}
	}
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
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

	bool show_demo_window = false;

	static int CIRCLE_COUNT = 1000;
	// circle params
	std::vector<Circle> circles;
	circles.reserve(CIRCLE_COUNT);
	Circle circle;

	std::default_random_engine generator;
	std::uniform_int_distribution<int> width_distribution(0, VIEWPORT_WIDTH);
	std::uniform_int_distribution<int> height_distribution(0, VIEWPORT_HEIGHT);

	std::uniform_int_distribution<int> direction_distribution(-1, 1);

	for (int i = 0; i < CIRCLE_COUNT; i++)
	{
		circle.direction.x = direction_distribution(generator);
		circle.direction.y = direction_distribution(generator);

		circle.center.x = width_distribution(generator);
		circle.center.y = height_distribution(generator);
		circles.push_back(circle);
	}

	circles.front().covid = true;

	float dTime = 0.f;
	float currentTime = glfwGetTime();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.3f, 0.5f, 0.7f, 1.f);

		moveCircles(circles, dTime);
		markIntersectionCircles(circles);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		
		if (ImGui::Begin("Options"))
		{
			ImGui::SliderFloat("Simulation speed", &SIMULATION_SPEED, 0.f, 100.f);
		}
		ImGui::End();

		if (ImGui::Begin(
			"Viewport", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoFocusOnAppearing))
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			for (const auto& c : circles)
			{
				ImVec2 center = ImVec2(c.center.x, c.center.y);
				ImColor color = c.covid ? COVID_COLOR : DEFAULT_COLOR;
				drawList->AddCircleFilled(center, c.radius, color);
			}
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		dTime = glfwGetTime() - currentTime;
		currentTime = glfwGetTime();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}