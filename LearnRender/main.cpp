#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "cage.h"
#include "circle.h"
#include "settings.h"
#include "util.h"

bool continue_drawing = true;
std::vector<float>susceptible;
std::vector<float>infected;
std::vector<float>recovered;
std::vector<float>dead;
std::vector<float>time;

int main(void) {
	GLFWwindow* window = create_window("COVID-19 modeling");
	if (!window)	return -1;

	Cage cage(300, 500, 500, glm::vec2(50, 50));
	cage.populate();
	cage.populateInfected(1, glfwGetTime());

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

		if (ImGui::Begin("Options", nullptr)) {

			if (ImGui::Button("Repopulate")) {
				cage.repopulate();
			}
			ImGui::SameLine();
			if (ImGui::Button("Populated 1 infected")) {
				cage.populateInfected(1, scaled_current_time);
			}
			ImGui::SameLine();
			ImGui::SliderFloat("Simulation speed", &SIMULATION_SPEED, 0.f, 100.f);
		}
		ImGui::End();
		ImGui::Begin("My Window");
		ImGui::Checkbox("Continue drawing", &continue_drawing);
		static ImPlotAxisFlags xflags = ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit;
		static ImPlotAxisFlags yflags = ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit;
		if (ImPlot::BeginPlot("My Plot", "time", "people", ImVec2(-1, 0), 0, xflags, yflags)) {

			ImPlot::PlotLine("Susceptible", time.data(), susceptible.data(), susceptible.size());
			ImPlot::PlotLine("Infected", time.data(), infected.data(), infected.size());
			ImPlot::PlotLine("Recovered", time.data(), recovered.data(), recovered.size());
			ImPlot::PlotLine("Dead", time.data(), dead.data(), dead.size());
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
			if (continue_drawing && SIMULATION_SPEED) {
				susceptible.push_back(cage.susceptible);
				infected.push_back(cage.infected);
				recovered.push_back(cage.recovered);
				dead.push_back(cage.dead);
				time.push_back(scaled_current_time);
			}

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