#pragma once
#include <stdexcept>
#include <vector>

#include "circle.h"

class Cage {
	std::vector<Circle> circles;
	int population_size_{};
	int height_{};
	int width_{};
	glm::vec2 begin_{};
	float last_update_time_;
public:
	Cage(int population_size, int height, int width, glm::vec2 center) :
		population_size_(population_size),
		height_(height),
		width_(width),
		begin_(center),
		last_update_time_(0) {}

	void populate() {
		circles.reserve(population_size_);
		std::uniform_int_distribution<int> width_distribution_(begin_.x, width_);
		std::uniform_int_distribution<int> height_distribution_(begin_.y, height_);
		Circle circle;
		for (int i = 0; i < population_size_; i++) 	{
			circle.direction.x = direction_distribution(generator);
			circle.direction.y = direction_distribution(generator);

			circle.center.x = width_distribution_(generator);
			circle.center.y = height_distribution_(generator);
			circles.push_back(circle);
		}
	}

	void populateInfected(int number) {
		if (number > population_size_ || number <= 0) {
			throw std::out_of_range("Number of infected to populate is invalid");
		}
		while(number--) {
			circles[number].disease_stage = DiseaseStages::INFECTED;
		}
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
					}
				}
			}
		}
	}

	void moveCircles_(const float& delta_time) {
		for (auto& circle : circles) {
			glm::vec2 oldCenter = circle.center;
			glm::vec2 newCenter = circle.center + circle.direction * delta_time;
			circle.center = newCenter;
			if (outsideViewport_(circle)) {
				circle.center = oldCenter;
				invertVector2(circle.direction);
			}
			circle.center += circle.direction * delta_time;
		}
	}

	bool outsideViewport_(const Circle& c) {
		return c.center.x + c.radius > begin_.x + width_ ||
			c.center.y + c.radius > begin_.y + height_ ||
			c.center.x - c.radius < begin_.x ||
			c.center.y - c.radius < begin_.y;
	}

	void changeDiseaseStageOverTime_(const float& current_time) {
		for (auto& circle : circles) {
			float dTime = current_time - circle.disease_stage_change_time;
			if (circle.disease_stage == DiseaseStages::INFECTED && dTime >= RECOVERY_TIME) {
				if (death_distribution(generator) == 1)
					circle.disease_stage = DiseaseStages::DEAD;
				else
					circle.disease_stage = DiseaseStages::RECOVERED;
			}
		}
	}
};
