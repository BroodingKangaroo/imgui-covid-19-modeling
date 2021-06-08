#pragma once

#include <vector>
#include <fstream>
#include <filesystem>

#include "cage.h"
#include "circle.h"
#include "canvas.h"
#include "settings.h"


struct Flow
{
	std::string source;
	std::string destination;
	int amount;

	Flow() = default;
	
	Flow(std::string source_, std::string destination_, int amount_) : source(source_), destination(destination_), amount(amount_) {}
};

/**
 *	Control the behavior of circles that move between cages.
 *	Circle moves to a destination cage and hangs out there for the TIME_TO_REST_IN_CAGE.
 *	When the time runs out circle goes back to home cage and rest there for the same time.
 *	The class also processes encounters of circles with cages that might be on the their way to destination cage or home cage.
 **/
class CageMediator {
	Canvas* canvas_;
	std::vector<std::list<Circle>::iterator> moving_circles;
	std::vector<Flow>flows_;

public:
	CageMediator(Canvas* canvas) : canvas_(canvas) {}

	void addMovingCircle(std::list<Circle>::iterator& circle_iterator) {
		moving_circles.emplace_back(circle_iterator);
	}

	void update(const float& current_time) {
		for (size_t i = 0; i < moving_circles.size(); i++) {
			for (auto& [cage_name, cage] : canvas_->getCages()) {

				// If true - circle is in the cage that is not its home cage and circle is not resting there.
				// There are two ways how it can be possible:
				// 1. circle goes to a destination cage through another cage
				// 2. circle has come to a destination cage
				if (cage.surrounds(moving_circles[i]->center) && (cage_name != moving_circles[i]->current_cage)) {

					// substitute the iterator with the new one that is working within the new cage
					auto old_iterator = moving_circles[i];
					moving_circles[i] = cage.addCircle(old_iterator);
					(*canvas_)[moving_circles[i]->current_cage].removeCircle(old_iterator);
					moving_circles[i]->current_cage = cage_name;

					if ( // circle has come to a destination cage
						moving_circles[i]->circle_moving_state == CircleMovingState::MOVING_TO_DESTINATION_CAGE
						&& cage_name == moving_circles[i]->destination_cage
						&& moving_circles[i]->arrived_in < 0
						) {
						moving_circles[i]->circle_moving_state = CircleMovingState::RESTING;
						moving_circles[i]->arrived_in = current_time;
					} else if ( // circle has come to the home cage
						moving_circles[i]->circle_moving_state == CircleMovingState::MOVING_TO_HOME_CAGE
						&& cage_name == moving_circles[i]->home_cage
						&& moving_circles[i]->arrived_in < 0
						) {
						moving_circles[i]->circle_moving_state = CircleMovingState::RESTING;
						moving_circles[i]->arrived_in = current_time;
					}

					// otherwise the cage should be passed without stopping
				}
			}

			// if true - time to get out of the cage
			float time_to_rest_in_cage = gen_random_float_number(TIME_TO_REST_IN_CAGE_MIN, TIME_TO_REST_IN_CAGE_MAX);
			if (moving_circles[i]->circle_moving_state == CircleMovingState::RESTING
				&& moving_circles[i]->arrived_in >= 0
				&& current_time - moving_circles[i]->arrived_in >= time_to_rest_in_cage
				) {
				moving_circles[i]->arrived_in = -1;
				moving_circles[i]->circle_moving_state =
					moving_circles[i]->current_cage == moving_circles[i]->home_cage
					? CircleMovingState::MOVING_TO_DESTINATION_CAGE
					: CircleMovingState::MOVING_TO_HOME_CAGE;
			}
			if (moving_circles[i]->circle_moving_state == CircleMovingState::MOVING_TO_DESTINATION_CAGE) {
				moving_circles[i]->direction = calculateCircleDirectionByCageName_(moving_circles[i]->center, moving_circles[i]->destination_cage);
			} else if (moving_circles[i]->circle_moving_state == CircleMovingState::MOVING_TO_HOME_CAGE) {
				moving_circles[i]->direction = calculateCircleDirectionByCageName_(moving_circles[i]->center, moving_circles[i]->home_cage);
			}
		}
	}

	void addDestination(Flow flow) {
		flows_.push_back(Flow(flow.source, flow.destination, flow.amount));
		std::vector<std::list<Circle>::iterator> iterators = (*canvas_)[flow.source].addDestination(flow.destination, flow.amount);
		for (auto& iterator : iterators) {
			addMovingCircle(iterator);
		}
	}

	std::string save(std::string file_name = "") const {
		file_name = file_name + "-" + getTimesStamp();
		std::filesystem::create_directory(DIRECTORY_FOR_SAVES);
		std::ofstream out(DIRECTORY_FOR_SAVES + "/" + file_name, std::ios::out | std::ios::app);
		
		out << canvas_->getCages().size() << "\n";
		for (const auto& [name, cage] : canvas_->getCages()) {
			out << name << "\n";
			out << cage.getCoordinates().top_left_corner.x << " " << cage.getCoordinates().top_left_corner.y << " " << cage.getCoordinates().width << " " << cage.getCoordinates().height << "\n";
			out << cage.getPopulationSize() << "\n";
		}
		out << flows_.size() << "\n";
		for (const auto& flow : flows_) {
			out << flow.source << " " << flow.destination << " " << flow.amount << "\n";
		}
		out.close();
		return file_name;
	}

	void clearData() {
		flows_.clear();
		moving_circles.clear();
		canvas_->clear_data();
	}
	
	void load(const std::string& file_name) {
		clearData();
		SIMULATION_SPEED = 0;
		std::ifstream in(file_name);
		int cages_number, flows_number;
		in >> cages_number;
		while(cages_number--) {
			int population_size;
			std::string cage_name;
			Coordinates cage_coordinates;
			in >> cage_name >> cage_coordinates.top_left_corner.x >> cage_coordinates.top_left_corner.y >> cage_coordinates.width >> cage_coordinates.height;
			in >> population_size;
			canvas_->addCage(Cage(population_size, cage_coordinates, cage_name));
		}
		in >> flows_number;
		if (flows_number) {
			for (auto& [name, cage] : canvas_->getCages()) {
				cage.repopulate();
			}
		}
		while (flows_number--) {
			Flow flow;
			in >> flow.source >> flow.destination >> flow.amount;
			flows_.push_back(flow);
			addDestination(flow);
		}
	}

private:
	glm::vec2 calculateCircleDirectionByCageName_(glm::vec2 circle_center, std::string& cage_name) const {
		Coordinates cage_coordinates = (*canvas_)[cage_name].getCoordinates();
		float cage_center_x = cage_coordinates.top_left_corner.x + cage_coordinates.width / 2. - circle_center.x;
		float cage_center_y = cage_coordinates.top_left_corner.y + cage_coordinates.height / 2. - circle_center.y;
		float max_coordinate = std::max(abs(cage_center_x), abs(cage_center_y));
		return glm::vec2(cage_center_x / max_coordinate, cage_center_y / max_coordinate);
	}
};