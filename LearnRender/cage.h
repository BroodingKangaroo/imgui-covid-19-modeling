#pragma once
#include <stdexcept>
#include <vector>

#include "circle.h"

class Cage {
	std::vector<Circle> circles;
	int height_{};
	int width_{};
	int population_size_{};
	glm::vec2 center_{};
	float last_update_time_;
public:
	Cage(int population_size, int height, int width, glm::vec2 center) :
		population_size_(population_size),
		height_(height),
		width_(width),
		center_(center),
		last_update_time_(0) {}

	void populate() {
		circles.reserve(population_size_);
		Circle circle;
		for (int i = 0; i < population_size_; i++) 	{
			circle.direction.x = direction_distribution(generator);
			circle.direction.y = direction_distribution(generator);

			circle.center.x = width_distribution(generator);
			circle.center.y = height_distribution(generator);
			circles.push_back(circle);
		}
	}

	void populate_infected(int number) {
		if (number > population_size_ || number <= 0) {
			throw std::out_of_range("Number of infected to populate is invalid");
		}
		while(number--) {
			circles[number].disease_stage = DiseaseStages::INFECTED;
		}
	}

	void update(const float current_time) {
		moveCircles(circles, current_time - last_update_time_);
		changeDiseaseStageOverTime(circles, current_time);
		markIntersectionCircles(circles, current_time);
		last_update_time_ = current_time;
	}

	std::vector<Circle> getCircles() {
		return circles;
	}
};
