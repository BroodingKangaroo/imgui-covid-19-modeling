#pragma once
#include <random>


float gen_random_float_number(float min_value, float max_value) {
    static std::default_random_engine e;
    std::uniform_real_distribution<> dis(min_value, max_value);
    return dis(e);
}



int gen_random_integer_number(int min_value, int max_value) {
    static std::default_random_engine e;
    std::uniform_int_distribution<> dis(min_value, max_value);
    return dis(e);
}