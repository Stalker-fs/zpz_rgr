#include <cmath>
#include <iostream>
#include <vector>

#include "../include/math_utils.h"

float expected_value(const std::vector<unsigned int>* delays, int element) {
    float sum = 0;
    for (const unsigned int x : *delays) {
        sum += x;
    }

    sum -= element;
    return sum / (delays->size()-1);
    // M
}

// leave-one-out variance
float loov(const std::vector<unsigned int>* delays, int element, float M) {
    float sum_s = 0;
    for (const unsigned int x : *delays) {
        sum_s += std::pow(x - M, 2);
    }
    sum_s -= std::pow(element - M, 2);
    return sum_s / (delays->size() - 2);
    // S2
}

float loov(const std::vector<unsigned int>* delays, int element) {
    float M = expected_value(delays, element);
    return loov(delays, element, M);
}

float t_value(int element, float M, float S) {
    return std::abs((element - M) / S);
    // t_p
}

float AVG(const std::vector<unsigned int>* delays) {
    float sum = 0;
    for (const unsigned int x : *delays) {
        sum += x;
    }
    return sum / delays->size();
}

// sample variance
float sv(const std::vector<unsigned int>* delays, const float M) {
    float sum = 0;
    for (const unsigned int x : *delays) {
        sum += std::pow(x - M, 2);
    }

    return sum / (delays->size() - 1);
    // S2
}

float S(float S2_1, float S2_2, int n_1, int n_2) {
    float max = std::max(S2_1, S2_2);
    S2_1 /= max;
    S2_2 /= max;

    // n1 + n2 - 2; why -1?
    return std::sqrt((std::pow(S2_1, 2) * (n_1 - 1) + std::pow(S2_2, 2) * (n_2 - 1)) / (n_1 + n_2 - 1));
}

float t_value2(float M1, float M2, float S_general, int n) {
    float max = std::max(M1, M2);

    M1 /= max;
    M2 /= max;

    return std::abs(M1 - M2) / (S_general * std::sqrt(2.0 / n));
}