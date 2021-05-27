#pragma once
#include <glm/vec2.hpp>

#include "settings.h"

enum class DiseaseStages {
	SUSCEPTIBLE, INFECTED, RECOVERED, DEAD
};

enum class CircleMovingState {
	RESTING, MOVING_TO_HOME_CAGE, MOVING_TO_DESTINATION_CAGE
};

struct Circle {
	glm::vec2 center = glm::vec2(10.f, 10.f);
	glm::vec2 direction = glm::vec2(0.f, 0.f);
	float radius = 3.f;

	int id;
	CircleMovingState circle_moving_state = CircleMovingState::RESTING;
	DiseaseStages disease_stage = DiseaseStages::SUSCEPTIBLE;
	float disease_stage_change_time = 0;
	std::string home_cage;
	std::string destination_cage;
	std::string current_cage;
	float arrived_in;
	float recovery_time;

	Circle() {
		static int id_ = 0;
		id = ++id_;
	}

	bool operator==(const Circle& rhs) const {
		return rhs.id == id;
	}
};

bool intersect(const Circle& c1, const Circle& c2) {
	glm::vec2 diff = c2.center - c1.center;
	return diff.x * diff.x + diff.y * diff.y <= 4 * c1.radius * c1.radius;
}

ImColor switchColorByDiseaseStage(DiseaseStages disease_stage) {
	switch (disease_stage) {
	case DiseaseStages::SUSCEPTIBLE:
		return SUSCEPTIBLE_COLOR;
	case DiseaseStages::INFECTED:
		return INFECTED_COLOR;
	case DiseaseStages::RECOVERED:
		return RECOVERED_COLOR;
	case DiseaseStages::DEAD:
		return DEAD_COLOR;
	}
}