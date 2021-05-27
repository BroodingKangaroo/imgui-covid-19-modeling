#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "settings.h"
#include "util.h"
#include "canvas.h"
#include "cage_mediator.h"
#include "ui_controls.h"


int main(void) {
	GLFWwindow* window = GLFWBeginRendering("COVID-19 modeling");
	if (!window) return -1;
	
	IMGUIBeginRendering(window);

	Canvas canvas(glm::vec2(0, 0), VIEWPORT_HEIGHT, VIEWPORT_WIDTH);

	CageMediator cage_mediator(&canvas);
	
	UIControls ui_controls(canvas, cage_mediator);

	TimeController time_controller;
	

	while (!glfwWindowShouldClose(window)) {
		time_controller.update(SIMULATION_SPEED);
		cage_mediator.update(time_controller.scaled_current_time);
		
		glClearColor(.5f, .5f, .5f, .5f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ui_controls.update(time_controller.scaled_current_time);

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

			canvas.drawCircles(drawList);

			canvas.drawCages(drawList);
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