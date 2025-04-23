#include <cmath>
#include <boost/math/distributions/students_t.hpp>
#include <iostream>

bool check_delay_outliers(const std::vector<unsigned int>* delays) {
    int sum = 0;
    for (const unsigned int x : *delays) {
        sum += x;
    }

    for (const unsigned int delay : *delays) {
        unsigned int sum_yi = sum - delay;
        double M = sum_yi / (double)delays->size();                         // expected value

        double sum_s = 0;
        for (const unsigned int x : *delays) {
            sum_s += std::pow(x - M, 2);
        }
        sum_s -= std::pow(delay - M, 2);
        double S = std::sqrt(sum_s / (double)(delays->size() - 1));         // standard deviation

        double t_p = std::abs((delay - M) / S);                             // Student's t-value
        boost::math::students_t dist(delays->size() - 1);
        double t_tb = boost::math::quantile(boost::math::complement(dist, 0.05 / 2));

        if (t_p > t_tb) {
            return false;
        }
    }

    return true;
}