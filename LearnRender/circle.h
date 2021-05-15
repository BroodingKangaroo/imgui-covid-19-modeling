#pragma once
#include <vector>
#include <glm/vec2.hpp>

#include "random_generators.h"
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

bool outsideViewport(const Circle& c) {
	return c.center.x + c.radius > VIEWPORT_WIDTH ||
		c.center.y + c.radius > VIEWPORT_HEIGHT ||
		c.center.x - c.radius < 0 ||
		c.center.y - c.radius < 0;
}

void invertVector2(glm::vec2& v) {
	v *= -1.f;
}

void moveCircles(std::vector<Circle>& circles, const float& delta_time) {
	for (auto& circle : circles) {
		glm::vec2 oldCenter = circle.center;
		glm::vec2 newCenter = circle.center + circle.direction * delta_time * SIMULATION_SPEED;
		circle.center = newCenter;
		if (outsideViewport(circle)) {
			circle.center = oldCenter;
			invertVector2(circle.direction);
		}
		circle.center += circle.direction * delta_time * SIMULATION_SPEED;
	}
}

void markIntersectionCircles(std::vector<Circle>& circles, const float& current_time) {
	for (auto& covidCircle : circles) {
		if (covidCircle.disease_stage == DiseaseStages::INFECTED) {
			for (auto& circle : circles) {
				if (circle.disease_stage == DiseaseStages::SUSCEPTIBLE && intersect(covidCircle, circle)) {
					circle.disease_stage = DiseaseStages::INFECTED;
					circle.disease_stage_change_time = current_time;
				}
			}
		}
	}
}

void changeDiseaseStageOverTime(std::vector<Circle>& circles, const float& current_time) {
	for (auto& circle: circles) {
		float dTime = current_time - circle.disease_stage_change_time;
		if (circle.disease_stage == DiseaseStages::INFECTED && dTime >= RECOVERY_TIME) {
			if (death_distribution(generator) == 1)
				circle.disease_stage = DiseaseStages::DEAD;
			else
				circle.disease_stage = DiseaseStages::RECOVERED;
		}
	}
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