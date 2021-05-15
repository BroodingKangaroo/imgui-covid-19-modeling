#pragma once
#include <random>

#include "settings.h"

std::default_random_engine generator;
std::uniform_int_distribution<int> width_distribution(0, VIEWPORT_WIDTH);
std::uniform_int_distribution<int> height_distribution(0, VIEWPORT_HEIGHT);
std::uniform_int_distribution<int> direction_distribution(-1, 1);
std::uniform_int_distribution<int> death_distribution(0, 1);