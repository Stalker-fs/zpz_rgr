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
double sv(const std::vector<unsigned int>* delays, const double M) {
    //double M = AVG(delays);

    double sum = 0;
    for (const unsigned int x : *delays) {
        sum += std::pow(x - M, 2);
    }

    return sum / (delays->size() - 1);
    // S2
}

long double S(double S2_1, double S2_2, int n) {
    S2_1 = S2_1 / (double)std::max(S2_1, S2_2);
    S2_2 = S2_2 / (double)std::max(S2_1, S2_2);
    //std::cout << "F: " << S2_1 << " S: " << S2_2 << std::endl;
    long double y = std::pow(S2_1, 2) + std::pow(S2_2, 2);
    //std::cout << "Y: " << y << std::endl;
    long double x = (y * (n - 1)) / (2*n - 2);
    //std::cout << "X: " << x << std::endl;
    return std::sqrt(x);
}

long double t_value2(double M1, double M2, long double S_general, int n) {
    if (M1 == M2) {
        std::cout << "^----------------^" << std::endl;
    }
    double max = std::max(M1, M2);
    M1 = M1 / max;
    M2 = M2 / max;
    double y = std::sqrt(2.0 / n);
    //std::cout << "YYY: " << y << std::endl; 
    long double x = S_general * y;
    //std::cout << "X1: " << S_general << std::endl;

    long double res = std::abs(M1 - M2) / (double)(x);
    if (res == 0) {
        std::cout << "M1: " << M1 << " M2: " << M2 << " x: " <<  x << std::endl;
    }
    return res;
}