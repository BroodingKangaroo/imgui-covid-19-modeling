#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "cage.h"
#include "circle.h"
#include "settings.h"
#include "util.h"




int main(void)
{
	GLFWwindow* window = create_window("COVID-19 modeling");
	if(!window)	return -1;

	Cage cage(1000, VIEWPORT_HEIGHT, VIEWPORT_WIDTH, glm::vec2(0,0));
	cage.populate();
	cage.populate_infected(1);
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0, 0.0, 0.0, 0.0);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (SHOW_DEMO_WINDOW)
			ImGui::ShowDemoWindow(&SHOW_DEMO_WINDOW);

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

			cage.update(glfwGetTime());
			
			for (const auto& circle: cage.getCircles())
			{
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
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}