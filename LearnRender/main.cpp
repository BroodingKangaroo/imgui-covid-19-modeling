#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "cage.h"
#include "circle.h"
#include "settings.h"
#include "util.h"


std::vector<int>susceptible;
std::vector<int>infected;
std::vector<int>recovered;
std::vector<int>dead;

int main(void) {
	GLFWwindow* window = create_window("COVID-19 modeling");
	if (!window)	return -1;

	Cage cage(300, 500, 500, glm::vec2(50, 50));
	cage.populate();
	cage.populateInfected(1);

	float current_time = glfwGetTime();
	float scaled_current_time = current_time;
	while (!glfwWindowShouldClose(window)) {
		scaled_current_time += (glfwGetTime() - current_time) * SIMULATION_SPEED;
		current_time = glfwGetTime();

		glClearColor(.5f, .5f, .5f, .5f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (SHOW_DEMO_WINDOW) {
			ImPlot::ShowDemoWindow();
			ImGui::ShowDemoWindow(&SHOW_DEMO_WINDOW);
		}

		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

		if (ImGui::Begin("Options")) {
			ImGui::SliderFloat("Simulation speed", &SIMULATION_SPEED, 0.f, 100.f);

		}
		ImGui::End();
		ImGui::Begin("My Window");
		static ImPlotAxisFlags xflags = ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit;
		static ImPlotAxisFlags yflags = ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit;
		if (ImPlot::BeginPlot("My Plot", "time", "people", ImVec2(-1, 0), 0, xflags, yflags)) {

			ImPlot::PlotLine("Susceptible", susceptible.data(), susceptible.size(), CIRCLE_COUNT);
			ImPlot::PlotLine("Infected", infected.data(), infected.size(), CIRCLE_COUNT);
			ImPlot::PlotLine("Recovered", recovered.data(), recovered.size(), CIRCLE_COUNT);
			ImPlot::PlotLine("Dead", dead.data(), dead.size(), CIRCLE_COUNT);
			ImPlot::EndPlot();
		}
		ImGui::End();

		if (ImGui::Begin(
			"Viewport", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBackground)) {
			ImDrawList* drawList = ImGui::GetWindowDrawList();

			cage.update(scaled_current_time);
			susceptible.push_back(cage.susceptible);
			infected.push_back(cage.infected);
			recovered.push_back(cage.recovered);
			dead.push_back(cage.dead);

			for (const auto& circle : cage.getCircles()) {
				ImVec2 center = ImVec2(circle.center.x, circle.center.y);
				ImColor color = switchColorByDiseaseStage(circle.disease_stage);
				drawList->AddCircleFilled(center, circle.radius, color);
			}
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}