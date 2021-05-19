#pragma once

#include <vector>

#include "cage.h"
#include "circle.h"
#include "canvas.h"
#include "settings.h"

/**
 *	Control the behavior of circles that move between cages.
 *	Circle moves to a destination cage and hangs out there for the TIME_TO_REST_IN_CAGE.
 *	When the time runs out it goes back to home cage and rest there for the same time.
 *	The function also processes encounters with cages that might be on the way to destination cage.
 **/
class CageMediator {
	Canvas* canvas_;
	std::vector<std::list<Circle>::iterator> moving_circles;

public:
	CageMediator(Canvas* canvas) : canvas_(canvas) {}

	void addMovingCircle(std::list<Circle>::iterator& circle_iterator) {
		moving_circles.emplace_back(circle_iterator);
	}

	void update(const float& current_time) {
		for (int i = 0; i < moving_circles.size(); i++) {
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
					moving_circles[i]->current_cage = const_cast<char*>(cage_name);

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
			if (moving_circles[i]->circle_moving_state == CircleMovingState::RESTING
				&& moving_circles[i]->arrived_in >= 0
				&& current_time - moving_circles[i]->arrived_in >= TIME_TO_REST_IN_CAGE
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

	void addDestination(const char* home_cage_name, const char* destination_cage_name, int amount_of_circles) {
		std::vector<std::list<Circle>::iterator> iterators = (*canvas_)[home_cage_name].addDestination(const_cast<char*>(destination_cage_name), amount_of_circles);
		for (auto& iterator : iterators) {
			addMovingCircle(iterator);
		}
	}

private:
	glm::vec2 calculateCircleDirectionByCageName_(glm::vec2 circle_center, const char* cage_name) const {
		WindowCoordinates cage_coordinates = (*canvas_)[cage_name].getCoordinates();
		float cage_center_x = cage_coordinates.top_left_corner.x + cage_coordinates.width / 2. - circle_center.x;
		float cage_center_y = cage_coordinates.top_left_corner.y + cage_coordinates.height / 2. - circle_center.y;
		float max_coordinate = std::max(abs(cage_center_x), abs(cage_center_y));
		return glm::vec2(cage_center_x / max_coordinate, cage_center_y / max_coordinate);
	}
};