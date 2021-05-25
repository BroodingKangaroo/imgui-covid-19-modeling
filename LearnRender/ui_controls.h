#pragma once

#include "canvas.h"
#include "cage_mediator.h"

enum class CageAddState {
	WRONG_POPULATION_SIZE, EMPTY_NAME, REPEATED_NAME, INVALID_COORDINATES, OVERLAPPING, SUCCESS, EMPTY
};

class UIControls {
	Canvas* canvas_;
	CageMediator* cage_mediator_;
	inline static CageAddState cage_add_state_ = CageAddState::EMPTY;

public:

	UIControls(Canvas& canvas, CageMediator& cage_mediator) : canvas_(&canvas), cage_mediator_(&cage_mediator) {}

	void update(float scaled_current_time) {
		if (ImGui::Begin("Configuration")) {
			ImGui::SliderFloat("Simulation speed", &SIMULATION_SPEED, 0.f, 100.f);
			if (ImGui::CollapsingHeader("Cage configuration")) {
				manageCageControls(scaled_current_time);
				manageAddCageButton();
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

	void manageCageControls(float scaled_current_time) {
		for (auto& [cage_name, cage] : canvas_->getCages()) {
			if (ImGui::TreeNode(cage_name)) {
				if (ImGui::Button("Repopulate")) {
					cage.repopulate();
				}
				ImGui::SameLine();
				if (ImGui::Button("Populated 1 infected")) {
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
					cage_add_state_ = CageAddState::WRONG_POPULATION_SIZE;
				} else if (!std::strlen(cage_name)) {
					cage_add_state_ = CageAddState::EMPTY_NAME;
				} else if (!canvas_->isCageNameRepeats(cage_name)) {
					cage_add_state_ = CageAddState::REPEATED_NAME;
				} else if (!canvas_->isCoordinatesValid(left_corner, size)) {
					cage_add_state_ = CageAddState::INVALID_COORDINATES;
				} else if (!canvas_->isOverlapCages(left_corner, size)) {
					cage_add_state_ = CageAddState::OVERLAPPING;
				} else {
					cage_add_state_ = CageAddState::SUCCESS;
					canvas_->addCage(Cage(population_size, size[1], size[0], glm::vec2(left_corner[0], left_corner[1]), cage_name));
				}
			}

			chooseAddCageMessage();

			ImGui::TreePop();
		}
	}

	void chooseAddCageMessage() {
		switch (cage_add_state_) {
		case CageAddState::INVALID_COORDINATES:
			ImGui::TextColored(RED_COLOR, "Please check input parameters. \nCage must be inside ViewPort."); break;
		case CageAddState::WRONG_POPULATION_SIZE:
			ImGui::TextColored(RED_COLOR, "Please check input parameters. \nPopulation size is not allowed to exceed 1000."); break;
		case CageAddState::OVERLAPPING:
			ImGui::TextColored(RED_COLOR, "Please check input parameters. \nCages are not allowed to overlap each other."); break;
		case CageAddState::EMPTY_NAME:
			ImGui::TextColored(RED_COLOR, "Please check input parameters. \nThe cage name is not allowed to be empty."); break;
		case CageAddState::REPEATED_NAME:
			ImGui::TextColored(RED_COLOR, "Please check input parameters. \nCage with such name already exists."); break;
		case CageAddState::SUCCESS:
			ImGui::TextColored(GREEN_COLOR, "Cage was successfully created."); break;
		case CageAddState::EMPTY: break; default: break;
		}
	}
};