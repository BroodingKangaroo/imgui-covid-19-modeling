#pragma once

#include <unordered_map>
#include <string>

#include "cage.h"
#include "util.h"

class Canvas {
	int number_of_cages_{};
	Coordinates coordinates_;
	std::unordered_map<std::string, Cage> cages;
	GraphData graph_values_;
public:
	Canvas(glm::vec2 top_left_corner, int height, int width) : coordinates_(top_left_corner, height, width) {}

	void addCage(Cage cage) {
		cages[cage.name] = cage;
	}

	std::unordered_map<std::string, Cage>& getCages() {
		return cages;
	}

	Cage& operator[] (std::string& name) {
		return cages[std::string(name)];
	}

	void populate(const char* name) {
		cages[std::string(name)].populate();
	}

	void populateInfected(const char* name, int number_of_infected_to_populate, float time) {
		cages[std::string(name)].populateInfected(number_of_infected_to_populate, time);
	}

	void update(float scaled_current_time) {
		float susceptible_total = 0, infected_total = 0, recovered_total = 0, dead_total = 0;
		for (auto& [name, cage] : cages) {
			cage.update(scaled_current_time);
			if (SIMULATION_SPEED) {
				susceptible_total += cage.susceptible;
				infected_total += cage.infected;
				recovered_total += cage.recovered;
				dead_total += cage.dead;
			}
		}
		if (SIMULATION_SPEED)
			graph_values_.update(susceptible_total, infected_total, recovered_total, dead_total, scaled_current_time);
	}

	GraphData& getGraphData() {
		return graph_values_;
	}

	bool isCoordinatesValid(int* left_corner, int* size) {
		if (left_corner[0] <= 0 || left_corner[1] <= 0 || size[0] <= 0 || size[1] <= 0 || size[0] + left_corner[0] > VIEWPORT_WIDTH || size[1] + left_corner[1] > VIEWPORT_HEIGHT) {
			return false;
		}
		return true;
	}

	bool isOverlapCages(int* left_corner, int* size) {
		for (const auto& [cage_name, cage] : cages) {
			auto cage_coordinates = cage.getCoordinates();
			if (isOverlap(
				cage.getCoordinates(),
				Coordinates(
					glm::vec2(left_corner[0], left_corner[1]),
					left_corner[0] + size[0],
					left_corner[1] + size[1])
			)) {
				return false;
			}
		}
		return true;
	}
	
	void drawCircles(ImDrawList* drawList) {
		for (auto& [name, cage] : cages) {
			for (Circle& circle : cage.getCircles()) {
				ImVec2 center = ImVec2(circle.center.x, circle.center.y);
				ImColor color = switchColorByDiseaseStage(circle.disease_stage);
				drawList->AddCircleFilled(center, circle.radius, color);
			}
		}
	}

	void drawCages(ImDrawList* drawList) {
		for (const auto& [name, cage] : cages) {
			const auto cage_coordinates = cage.getCoordinates();
			ImVec2 left = ImVec2(cage_coordinates.top_left_corner.x - 1, cage_coordinates.top_left_corner.y - 1);
			ImVec2 right = ImVec2(left.x + cage_coordinates.width + 3, left.y + cage_coordinates.height + 3);
			drawList->AddRect(left, right, BORDER_COLOR, 1, ImDrawFlags(), 2);
			drawList->AddText(
				ImGui::GetFont(),
				CAGE_FONT_SIZE,
				ImVec2(left.x + cage_coordinates.width / 2. - CAGE_FONT_SIZE / 2. * cage.name.length() / 2., left.y - 25),
				CAGE_NAME_COLOR, 
				cage.name.c_str()
			);
		}
	}

	bool isCageNameRepeats(char* cage_name) {
		for (auto& [name, cage] : cages) {
			if (name == std::string(cage_name)) {
				return false;
			}
		}
		return true;
	}
};
