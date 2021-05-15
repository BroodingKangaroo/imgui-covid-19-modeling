#pragma once
#include <glm/vec2.hpp>

#include "settings.h"

enum class DiseaseStages {
	SUSCEPTIBLE, INFECTED, RECOVERED, DEAD
};

class Circle {
public:
	glm::vec2 center = glm::vec2(10.f, 10.f);
	glm::vec2 direction = glm::vec2(0.f, 0.f);
	ImColor color;

	float radius = 3.f;
	DiseaseStages disease_stage = DiseaseStages::SUSCEPTIBLE;
	float disease_stage_change_time = 0;
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