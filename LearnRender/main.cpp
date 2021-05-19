#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "cage.h"
#include "circle.h"
#include "settings.h"
#include "util.h"
#include "canvas.h"
#include "cage_mediator.h"

int main(void) {
	GLFWwindow* window = GLFWBeginRendering("COVID-19 modeling");
	if (!window) return -1;
	
	IMGUIBeginRendering(window);

	Canvas canvas(glm::vec2(0, 0), VIEWPORT_HEIGHT, VIEWPORT_WIDTH);

	
	canvas.addCage(Cage(10, 300, 300, glm::vec2(50, 200), "home"));
	canvas.populate("home");
	//canvas.populateInfected("home", 3, glfwGetTime());

	canvas.addCage(Cage(30, 300, 300, glm::vec2(500, 200), "middle"));
	canvas.populate("middle");
	canvas.populateInfected("middle", 10, glfwGetTime());

	canvas.addCage(Cage(10, 300, 300, glm::vec2(950, 200), "destination"));
	canvas.populate("destination");
	//canvas.populateInfected("destination", 1, glfwGetTime());

	CageMediator cage_mediator(&canvas);

	cage_mediator.addDestination("home", "destination", 5);


	TimeController time_controller;

	while (!glfwWindowShouldClose(window)) {
		time_controller.update(SIMULATION_SPEED);
		cage_mediator.update(time_controller.scaled_current_time);
		
		glClearColor(.5f, .5f, .5f, .5f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		/*if (SHOW_DEMO_WINDOW) {
			ImPlot::ShowDemoWindow();
			ImGui::ShowDemoWindow(&SHOW_DEMO_WINDOW);
		}*/

		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

		canvas.addUIControls(time_controller.scaled_current_time);
		
		canvas.drawData();

		if (ImGui::Begin(
			"Viewport", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBackground
		)) {

			canvas.update(time_controller.scaled_current_time);

			ImDrawList* drawList = ImGui::GetWindowDrawList();

			for (const auto& circle : canvas.getCirclesToDraw()) {
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