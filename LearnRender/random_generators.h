#pragma once
#include <random>

std::default_random_engine generator;
std::uniform_int_distribution<int> direction_distribution(-1, 1);
std::uniform_int_distribution<int> death_distribution(0, 1);