#pragma once

#include "canvas.h"
#include "cage_mediator.h"

enum class UserInputMessage {
	WRONG_POPULATION_SIZE, EMPTY_NAME, REPEATED_NAME, INVALID_COORDINATES, OVERLAPPING, SUCCESS, INITIAL, DUPLICATED_NAME, FLOW_BIGGER_THAN_CAPABILITY
};

class UIControls {
	Canvas* canvas_;
	CageMediator* cage_mediator_;
	inline static UserInputMessage add_cage_state_ = UserInputMessage::INITIAL;
	inline static UserInputMessage add_flow_state_ = UserInputMessage::INITIAL;

public:

	UIControls(Canvas& canvas, CageMediator& cage_mediator) : canvas_(&canvas), cage_mediator_(&cage_mediator) {}

	void update(float scaled_current_time) {
		if (ImGui::Begin("Configuration")) {
			ImGui::SliderFloat("Simulation speed", &SIMULATION_SPEED, 0.f, 100.f);
			if (ImGui::CollapsingHeader("Cage configuration")) {
				manageCageControls(scaled_current_time);
				manageAddCageButton();
				manageAddFlowButton();
				manageSaveLoadButtons();
			}
			ImGui::End();
		}

		canvas_->getGraphData().drawData();

		if (SHOW_DEMO_WINDOW) {
			//ImPlot::ShowDemoWindow();
			ImGui::ShowDemoWindow(&SHOW_DEMO_WINDOW);
		}
	}

private:

	void manageSaveLoadButtons() {
		if (ImGui::Button("Save")) {
			cage_mediator_->save();
		}
	}
	
	void manageAddFlowButton() {
		if (ImGui::TreeNode("Add flow")) {
			static char source_cage_name[128] = "";
			static char destination_cage_name[128] = "";
			static int number_of_moving_circles = 0;
			ImGui::PushItemWidth(100);
			ImGui::InputText("Input name of source cage", source_cage_name, IM_ARRAYSIZE(source_cage_name));
			ImGui::InputText("Input name of destination", destination_cage_name, IM_ARRAYSIZE(destination_cage_name));
			ImGui::InputInt("Input number of moving circles", &number_of_moving_circles);
			ImGui::PopItemWidth();
			if (ImGui::Button("Add flow!")) {
				if (!std::strlen(source_cage_name) || !std::strlen(destination_cage_name)) {
					add_flow_state_ = UserInputMessage::EMPTY_NAME;
				} else if (std::strcmp(source_cage_name, destination_cage_name) == 0) {
					add_flow_state_ = UserInputMessage::DUPLICATED_NAME;
				} else {
					add_flow_state_ = UserInputMessage::SUCCESS;
					cage_mediator_->addDestination(Flow(std::string(source_cage_name), std::string(destination_cage_name), number_of_moving_circles));
				}
			}
			chooseUserInputMessage(add_flow_state_);
			ImGui::TreePop();
		}
	}
	
	void manageCageControls(float scaled_current_time) {
		for (auto& [cage_name, cage] : canvas_->getCages()) {
			if (ImGui::TreeNode(cage_name.c_str())) {
				if (ImGui::Button("Repopulate")) {
					cage.repopulate();
				}
				ImGui::SameLine();
				if (ImGui::Button("Populate 1 infected")) {
					cage.populateInfected(1, scaled_current_time);
				}
				ImGui::TreePop();
			}
		}
	}

	void manageAddCageButton() {
		if (ImGui::TreeNode("Add cage")) {
			static char cage_name[128] = "";
			static int left_corner[2] = { 50, 50 }, size[2] = { 300, 300 }, population_size = 150;
			
			ImGui::PushItemWidth(100);
			ImGui::InputText("Input cage name", cage_name, IM_ARRAYSIZE(cage_name));
			ImGui::InputInt2("Input x and y coordinates of the left top corner", left_corner);
			ImGui::InputInt("Input width and height", size);
			ImGui::InputInt("Input population size", &population_size);
			ImGui::PopItemWidth();
			
			if (ImGui::Button("Add cage!")) {
				if (population_size > 1000) {
					add_cage_state_ = UserInputMessage::WRONG_POPULATION_SIZE;
				} else if (!std::strlen(cage_name)) {
					add_cage_state_ = UserInputMessage::EMPTY_NAME;
				} else if (!canvas_->isCageNameRepeats(cage_name)) {
					add_cage_state_ = UserInputMessage::REPEATED_NAME;
				} else if (!canvas_->isCoordinatesValid(left_corner, size)) {
					add_cage_state_ = UserInputMessage::INVALID_COORDINATES;
				} else if (!canvas_->isOverlapCages(left_corner, size)) {
					add_cage_state_ = UserInputMessage::OVERLAPPING;
				} else {
					add_cage_state_ = UserInputMessage::SUCCESS;
					canvas_->addCage(Cage(population_size, Coordinates(glm::vec2(left_corner[0], left_corner[1]), size[1], size[0]), cage_name));
				}
			}

			chooseUserInputMessage(add_cage_state_);

			ImGui::TreePop();
		}
	}

	void chooseUserInputMessage(UserInputMessage& state) {
		switch (state) {
		case UserInputMessage::INVALID_COORDINATES:
			ImGui::TextColored(RED_COLOR, "Please check input parameters. \nCage must be inside ViewPort."); break;
		case UserInputMessage::WRONG_POPULATION_SIZE:
			ImGui::TextColored(RED_COLOR, "Please check input parameters. \nPopulation size is not allowed to exceed 1000."); break;
		case UserInputMessage::OVERLAPPING:
			ImGui::TextColored(RED_COLOR, "Please check input parameters. \nCages are not allowed to overlap each other."); break;
		case UserInputMessage::EMPTY_NAME:
			ImGui::TextColored(RED_COLOR, "Please check input parameters. \nThe cage name is not allowed to be empty."); break;
		case UserInputMessage::REPEATED_NAME:
			ImGui::TextColored(RED_COLOR, "Please check input parameters. \nCage with such name already exists."); break;
		case UserInputMessage::DUPLICATED_NAME:
			ImGui::TextColored(RED_COLOR, "Please check input parameters. \nNames of the cages should not be equal."); break;
		case UserInputMessage::SUCCESS:
			ImGui::TextColored(GREEN_COLOR, "Done."); break;
		case UserInputMessage::INITIAL: break; default: break;
		}
	}
};