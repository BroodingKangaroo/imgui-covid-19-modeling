#pragma once
#include <random>
#include <stdexcept>
#include <vector>

#include "circle.h"
#include "util.h"
#include "random_generators.h"

class Cage {
	std::vector<Circle> circles;
	int population_size_{};
	int height_{};
	int width_{};
	glm::vec2 begin_{};
	float last_update_time_{};
public:
	int susceptible;
	int infected;
	int recovered;
	int dead;
	Cage(int population_size, int height, int width, glm::vec2 center) :
		population_size_(population_size),
		height_(height),
		width_(width),
		begin_(center),
		last_update_time_(0),
		susceptible(population_size) {}

	void populate() {
		circles.reserve(population_size_);
		std::uniform_int_distribution<int> width_distribution_(begin_.x, width_);
		std::uniform_int_distribution<int> height_distribution_(begin_.y, height_);
		Circle circle;
		for (int i = 0; i < population_size_; i++) {
			circle.direction.x = x_direction_distribution(generator);
			circle.direction.y = y_direction_distribution(generator);
			circle.center.x = width_distribution_(generator);
			circle.center.y = height_distribution_(generator);
			circles.push_back(circle);
		}
	}

	void populateInfected(int number_of_infected_to_populate) {
		if (number_of_infected_to_populate > population_size_ || number_of_infected_to_populate <= 0) {
			throw std::out_of_range("Number of infected to populate is invalid");
		}
		while (number_of_infected_to_populate--) {
			circles[number_of_infected_to_populate].disease_stage = DiseaseStages::INFECTED;
		}
		infected = number_of_infected_to_populate;
		susceptible -= number_of_infected_to_populate;
	}

	void update(const float current_time) {
		if (SIMULATION_SPEED != 0.0f) {
			moveCircles_(current_time - last_update_time_);
			changeDiseaseStageOverTime_(current_time);
			markIntersectionCircles_(current_time);
		}

		last_update_time_ = current_time;
	}

	std::vector<Circle> getCircles() const {
		return circles;
	}

	void markIntersectionCircles_(const float& current_time) {
		for (auto& covidCircle : circles) {
			if (covidCircle.disease_stage == DiseaseStages::INFECTED) {
				for (auto& circle : circles) {
					if (circle.disease_stage == DiseaseStages::SUSCEPTIBLE && intersect(covidCircle, circle)) {
						circle.disease_stage = DiseaseStages::INFECTED;
						circle.disease_stage_change_time = current_time;
						susceptible--;
						infected++;
					}
				}
			}
		}
	}

	void moveCircles_(const float& delta_time) {
		for (auto& circle : circles) {
			if (circle.disease_stage == DiseaseStages::DEAD) continue;
			glm::vec2 oldCenter = circle.center;
			glm::vec2 newCenter = circle.center + circle.direction * delta_time;
			circle.center = newCenter;
			glm::vec2* intersection = outsideViewport_(circle);
			if (intersection) {
				circle.center = oldCenter;
				reflectVector2(circle.direction, *intersection);
			}
			circle.center += circle.direction * delta_time;
		}
	}

	glm::vec2* outsideViewport_(const Circle& c) {
		if (c.center.x + c.radius > begin_.x + width_)
			return Intersection::RIGHT;
		if (c.center.y + c.radius > begin_.y + height_)
			return Intersection::TOP;
		if (c.center.x - c.radius < begin_.x)
			return Intersection::LEFT;
		if (c.center.y - c.radius < begin_.y)
			return Intersection::BOTTOM;
		return Intersection::NO_INTERSECTION;
	}

	void changeDiseaseStageOverTime_(const float& current_time) {
		for (auto& circle : circles) {
			float dTime = current_time - circle.disease_stage_change_time;
			if (circle.disease_stage == DiseaseStages::INFECTED && dTime >= RECOVERY_TIME) {
				if (death_distribution(generator) < DEATH_PROBABILITY) {
					circle.disease_stage = DiseaseStages::DEAD;
					dead++;
				} else {
					circle.disease_stage = DiseaseStages::RECOVERED;
					recovered++;
				}
				infected--;
			}
		}
	}
};