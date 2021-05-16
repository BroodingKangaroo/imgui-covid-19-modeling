#pragma once
#include <random>

std::default_random_engine generator;

std::uniform_real_distribution<double> x_direction_distribution(-1, 1);
std::uniform_real_distribution<double> y_direction_distribution(-1, 1);
std::uniform_real_distribution<double> death_distribution(0, 1);