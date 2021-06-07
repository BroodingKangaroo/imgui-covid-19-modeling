#pragma once
#include <string>

#include "imgui.h"

int VIEWPORT_WIDTH = 1680;
int VIEWPORT_HEIGHT = 1020;

float SIMULATION_SPEED = 0.0;

ImColor SUSCEPTIBLE_COLOR = ImColor(255, 255, 0);
ImColor INFECTED_COLOR = ImColor(255, 0, 0);
ImColor DEAD_COLOR = ImColor(0, 0, 0);
ImColor RECOVERED_COLOR = ImColor(0, 255, 0);

ImColor GREEN_COLOR = ImColor(0, 255, 0);
ImColor RED_COLOR = ImColor(255, 0, 0);


int CAGE_FONT_SIZE = 20;
ImColor BORDER_COLOR = ImColor(180, 180, 180);
ImColor CAGE_NAME_COLOR = ImColor(200, 200, 20);

int CIRCLE_COUNT = 100;

float RECOVERY_TIME_MIN = 300;
float RECOVERY_TIME_MAX = 4000;

float DEATH_PROBABILITY = 0.2;

float INFECTION_PROBABILITY = 0.07;

bool SHOW_DEMO_WINDOW = false;

float TIME_TO_REST_IN_CAGE = 1000;

std::string DIRECTORY_FOR_SAVES = "saves";