#pragma once

#include <unordered_map>
#include <string>

#include "cage.h"
#include "util.h"

class Canvas {
	int number_of_cages_{};
	WindowCoordinates coordinates_;
	std::unordered_map<const char*, Cage> cages;
	GraphValues graph_values_;
public:
	Canvas(glm::vec2 top_left_corner, int height, int width) : coordinates_(top_left_corner, height, width) {}

	void addCage(Cage cage) {
		cages[cage.name] = cage;
	}

	std::unordered_map<const char*, Cage>& getCages() {
		return cages;
	}

	Cage& operator[] (const char* name) {
		return cages[name];
	}

	void populate(const char* name) {
		cages[name].populate();
	}

	void populateInfected(const char* name, int number_of_infected_to_populate, float time) {
		cages[name].populateInfected(number_of_infected_to_populate, time);
	}

	void update(float scaled_current_time) {
		float susceptible_total = 0, infected_total = 0, recovered_total = 0, dead_total = 0;
		for (auto& cage : cages) {
			cage.second.update(scaled_current_time);
			if (SIMULATION_SPEED) {
				susceptible_total += cage.second.susceptible;
				infected_total += cage.second.infected;
				recovered_total += cage.second.recovered;
				dead_total += cage.second.dead;
			}
		}
		if (SIMULATION_SPEED)
			graph_values_.update(susceptible_total, infected_total, recovered_total, dead_total, scaled_current_time);
	}

	std::vector<Circle> getCirclesToDraw() {
		std::vector<Circle>circles;
		for (auto& entry : cages) {
			for (Circle& circle : entry.second.getCircles()) {
				circles.emplace_back(circle);
			}
		}
		return circles;
	}

	void drawData() {
		graph_values_.drawData();
	}

	void addUIControls(float scaled_current_time) {
		ImGui::SetNextWindowSize(ImVec2(400, 300));
		ImGui::SetNextWindowPos(ImVec2(VIEWPORT_WIDTH - 400, 0));
		bool p_open = true;
		if (ImGui::Begin("Configuration",  &p_open)) {
			if (ImGui::CollapsingHeader("Cage configuration")) {
				for (auto& [cage_name, cage] : cages) {
					if (ImGui::TreeNode(cage_name)) {
						if (ImGui::Button("Repopulate")) {
							cage.repopulate();
						}
						ImGui::SameLine();
						if (ImGui::Button("Populated 1 infected")) {
							cage.populateInfected(1, scaled_current_time);
						}
						ImGui::TreePop();
					}
				}
			}
			ImGui::SliderFloat("Simulation speed", &SIMULATION_SPEED, 0.f, 100.f);
		}
		ImGui::End();
	}
};
