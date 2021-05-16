#pragma once
#include "imgui.h"

int VIEWPORT_WIDTH = 1280;
int VIEWPORT_HEIGHT = 720;

float SIMULATION_SPEED = 0.f;

ImColor SUSCEPTIBLE_COLOR = ImColor(255, 255, 0);
ImColor INFECTED_COLOR = ImColor(255, 0, 0);
ImColor DEAD_COLOR = ImColor(0, 0, 0);
ImColor RECOVERED_COLOR = ImColor(0, 255, 0);

int CIRCLE_COUNT = 100;

float RECOVERY_TIME = 100;

float DEATH_PROBABILITY = 0.3;

bool SHOW_DEMO_WINDOW = true;