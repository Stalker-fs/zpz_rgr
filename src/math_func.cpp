#include <cmath>
#include <boost/math/distributions/students_t.hpp>
#include <iostream>

bool check_delay_outliers(const std::vector<unsigned int>* delays) {
    int sum = 0;
    for (const unsigned int x : *delays) {
        sum += x;
    }

    std::cout << "SUM: " << sum << std::endl;

    for (int i = 0; i < delays->size(); i++) {
        unsigned int sum_yi = sum - (*delays)[i];
        std::cout << "SUM - i: " << sum_yi << std::endl;
        double M = sum_yi / delays->size();
        std::cout << "M: " << M << std::endl;

        double sum_s = 0;
        for (const unsigned int x : *delays) {
            sum_s += std::pow(x - M, 2);
        }

        sum_s -= std::pow((*delays)[i] - M, 2);

        double S = std::sqrt(sum_s / (delays->size() - 1));
        std::cout << "S: " << S << std::endl;

        double t_p = std::abs(((*delays)[i] - M) / S);
        std::cout << "t_p: " << t_p << std::endl;

        boost::math::students_t dist(delays->size() - 1);
        double t_tb = boost::math::quantile(boost::math::complement(dist, 0.05 / 2));
        std::cout << "t_tp: " << t_tb << std::endl;
        std::cout << "---------------------------" << std::endl;

        if (t_p > t_tb) {
            return false;
        }
    }

    return true;
}