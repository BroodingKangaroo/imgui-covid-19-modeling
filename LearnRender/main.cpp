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

	
	canvas.addCage(Cage(20, 300, 300, glm::vec2(50, 150), "home"));
	canvas.populate("home");
	//canvas.populateInfected("home", 3, glfwGetTime());

	canvas.addCage(Cage(30, 300, 300, glm::vec2(500, 150), "middle1"));
	canvas.populate("middle1");
	canvas.populateInfected("middle1", 5, glfwGetTime());

	canvas.addCage(Cage(200, 400, 400, glm::vec2(50, 550), "middle2"));
	canvas.populate("middle2");
	//canvas.populateInfected("middle2", 5, glfwGetTime());

	canvas.addCage(Cage(40, 300, 300, glm::vec2(550, 550), "middle3"));
	canvas.populate("middle3");

	canvas.addCage(Cage(100, 300, 300, glm::vec2(950, 150), "destination"));
	canvas.populate("destination");
	//canvas.populateInfected("destination", 1, glfwGetTime());

	CageMediator cage_mediator(&canvas);

	cage_mediator.addDestination("middle2", "middle3", 10);
	cage_mediator.addDestination("middle3", "destination", 7);
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

			for (const auto& [name, cage] : canvas.getCages()) {
				ImVec2 left = ImVec2(cage.getCoordinates().top_left_corner.x - 1, cage.getCoordinates().top_left_corner.y - 1);
				ImVec2 right = ImVec2(left.x + cage.getCoordinates().width + 3, left.y + cage.getCoordinates().height + 3);
				drawList->AddRect(left, right, BORDER_COLOR, 1, ImDrawFlags(), 2);
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