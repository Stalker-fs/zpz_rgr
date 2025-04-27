#include <cmath>
#include <boost/math/distributions/students_t.hpp>
#include <iostream>

#include "../include/math_func.h"

bool check_delay_outliers2(const std::vector<unsigned int>* delays) {
    boost::math::students_t dist(delays->size() - 2);
    double t_tb = boost::math::quantile(boost::math::complement(dist, 0.025 / 2));

    for (const unsigned int x : *delays) {
        double M = expected_value(delays, x);        
        double S2 = loov(delays, x, M);
        double S = std::sqrt(S2);

        double t_p = t_value(x, M, S);

        if (t_p > t_tb) {
            return false;
        }
    }
    return true;
}

double expected_value(const std::vector<unsigned int>* delays, int element) {
    double sum = 0;
    for (const unsigned int x : *delays) {
        sum += x;
    }

    sum -= element;
    return sum / (delays->size()-1);
    // M
}

// leave-one-out variance
double loov(const std::vector<unsigned int>* delays, int element, double M) {
    double sum_s = 0;
    for (const unsigned int x : *delays) {
        sum_s += std::pow(x - M, 2);
    }
    sum_s -= std::pow(element - M, 2);
    return sum_s / (delays->size() - 2);
    // S2
}

double loov(const std::vector<unsigned int>* delays, int element) {
    double M = expected_value(delays, element);
    return loov(delays, element, M);
}

double t_value(int element, double M, double S) {
    return std::abs((element - M) / S);
    // t_p
}

double AVG(const std::vector<unsigned int>* delays) {
    double sum = 0;
    for (const unsigned int x : *delays) {
        sum += x;
    }
    return sum / delays->size();
}

// sample variance
double sv(const std::vector<unsigned int>* delays, const double M) {
    double sum = 0;
    for (const unsigned int x : *delays) {
        sum += std::pow(x - M, 2);
    }

    return sum / (delays->size() - 1);
    // S2
}

double S(double S2_1, double S2_2, int n_1, int n_2) {
    double max = std::max(S2_1, S2_2);
    S2_1 /= max;
    S2_2 /= max;

    // n1 + n2 - 2; why -1?
    return std::sqrt((std::pow(S2_1, 2) * (n_1 - 1) + std::pow(S2_2, 2) * (n_2 - 1)) / (n_1 + n_2 - 1));
}

double t_value2(double M1, double M2, double S_general, int n) {
    double max = std::max(M1, M2);

    M1 /= max;
    M2 /= max;

    return std::abs(M1 - M2) / (S_general * std::sqrt(2.0 / n));
}