#include <cmath>
#include <boost/math/distributions/students_t.hpp>
#include <iostream>

#include "../include/math_func.h"

bool check_delay_outliers2(const std::vector<unsigned int>* delays) {
    boost::math::students_t dist(delays->size() - 2);
    double t_tb = boost::math::quantile(boost::math::complement(dist, 0.05 / 2));

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
    int sum = 0;
    for (const unsigned int x : *delays) {
        sum += x;
    }

    sum -= element;
    return sum / (double)(delays->size()-1);
    // M
}

// leave-one-out variance
double loov(const std::vector<unsigned int>* delays, int element, double M) {
    double sum_s = 0;
    for (const unsigned int x : *delays) {
        sum_s += std::pow(x - M, 2);
    }
    sum_s -= std::pow(element - M, 2);
    return sum_s / (double)(delays->size() - 2);
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
double sv(const std::vector<unsigned int>* delays) {
    double M = AVG(delays);

    double sum = 0;
    for (const unsigned int x : *delays) {
        sum += std::pow(x - M, 2);
    }

    return sum / (delays->size() - 1);
    // S2
}

