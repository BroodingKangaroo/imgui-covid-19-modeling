#pragma once
#include <list>
#include <random>
#include <stdexcept>
#include <vector>

#include "circle.h"
#include "util.h"
#include "random_generators.h"

class Cage {
	std::list<Circle> circles{};
	int population_size_{};
	WindowCoordinates coordinates_{};
	float last_update_time_{};
public:
	const char* name{};
	int susceptible{};
	int infected{};
	int recovered{};
	int dead{};

	Cage() {}

	Cage(int population_size, int height, int width, glm::vec2 top_left_corner, const char* name_) :
		population_size_(population_size),
		coordinates_(top_left_corner, height, width),
		name(name_),
		susceptible(population_size) {}

	void populate() {
		Circle circle;
		for (int i = 0; i < population_size_; i++) {
			circle.direction.x = gen_random_float_number(-1.0f, 1.0f);
			circle.direction.y = gen_random_float_number(-1.0f, 1.0f);
			circle.center.x = gen_random_integer_number(coordinates_.top_left_corner.x, coordinates_.top_left_corner.x + coordinates_.width);
			circle.center.y = gen_random_integer_number(coordinates_.top_left_corner.y, coordinates_.top_left_corner.y + coordinates_.height);
			circle.home_cage = const_cast<char*>(name);
			circle.current_cage = circle.home_cage;
			circles.push_back(circle);
		}
	}

	void repopulate() {
		circles.clear();
		susceptible = population_size_;
		infected = 0;
		recovered = 0;
		dead = 0;
		populate();
	}

	void populateInfected(int number_of_infected_to_populate, float infection_time) {
		if (number_of_infected_to_populate > population_size_ || number_of_infected_to_populate <= 0) {
			throw std::out_of_range("Number of infected to populate is invalid");
		}
		auto circle = circles.begin();
		while (number_of_infected_to_populate--) {
			circle->recovery_time = gen_random_integer_number(RECOVERY_TIME_MIN, RECOVERY_TIME_MAX);
			
			circle->disease_stage = DiseaseStages::INFECTED;
			circle->disease_stage_change_time = infection_time;
			circle = ++circle;
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

	std::list<Circle> getCircles() const {
		return circles;
	}

	void removeCircle(const std::list<Circle>::iterator& circle_iterator) {
		circles.erase(circle_iterator);
	}

	void markIntersectionCircles_(const float& current_time) {
		std::uniform_real_distribution<float> infect_distribution(0, 1);
		for (auto& covidCircle : circles) {
			if (covidCircle.disease_stage == DiseaseStages::INFECTED) {
				for (auto& circle : circles) {
					if (circle.disease_stage == DiseaseStages::SUSCEPTIBLE && intersect(covidCircle, circle) && gen_random_float_number(0, 1) < INFECTION_PROBABILITY) {
						circle.disease_stage = DiseaseStages::INFECTED;
						
						circle.recovery_time = gen_random_integer_number(RECOVERY_TIME_MIN, RECOVERY_TIME_MAX);
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
			if (circle.circle_moving_state==CircleMovingState::RESTING && intersection) {
				circle.center = oldCenter;
				reflectVector2(circle.direction, *intersection);
			}
			circle.center += circle.direction * delta_time;
		}
	}

	glm::vec2* outsideViewport_(const Circle& c) {
		if (c.center.x > coordinates_.top_left_corner.x + coordinates_.width)
			return Intersection::RIGHT;
		if (c.center.y > coordinates_.top_left_corner.y + coordinates_.height)
			return Intersection::TOP;
		if (c.center.x < coordinates_.top_left_corner.x)
			return Intersection::LEFT;
		if (c.center.y < coordinates_.top_left_corner.y)
			return Intersection::BOTTOM;
		return Intersection::NO_INTERSECTION;
	}

	void changeDiseaseStageOverTime_(const float& current_time) {
		for (auto& circle : circles) {
			float dTime = current_time - circle.disease_stage_change_time;
			if (circle.disease_stage == DiseaseStages::INFECTED && dTime >= circle.recovery_time) {
				if (gen_random_float_number(0, 1) < DEATH_PROBABILITY) {
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

	bool surrounds(glm::vec2 center) {
		if (center.x > coordinates_.top_left_corner.x
			&& center.x < coordinates_.top_left_corner.x + coordinates_.width
			&& center.y > coordinates_.top_left_corner.y
			&& center.y < coordinates_.top_left_corner.y + coordinates_.height) {
			return true;
		}
		return false;
	}

	std::list<Circle>::iterator addCircle(std::list<Circle>::iterator& value) {
		circles.emplace_back(*value);
		return std::prev(circles.end());
	}

	WindowCoordinates getCoordinates() const {
		return coordinates_;
	}

	std::vector<std::list<Circle>::iterator> addDestination(char* destination_cage_name, int amount_of_circles) {
		std::vector<std::list<Circle>::iterator> iterators;
		auto circle_iterator = circles.begin();
		while(circle_iterator != circles.end() && amount_of_circles-- > 0) {
			circle_iterator->destination_cage = destination_cage_name;
			circle_iterator->circle_moving_state = CircleMovingState::MOVING_TO_DESTINATION_CAGE;
			iterators.emplace_back(circle_iterator);
			++circle_iterator;
		}
		return iterators;
	}
};