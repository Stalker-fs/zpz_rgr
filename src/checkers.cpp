#include <boost/math/distributions/students_t.hpp>
#include <boost/math/distributions/fisher_f.hpp>
#include <iostream>

bool fisher(std::vector<double>& S2_array_1, double S2_inp_1, int size_1, int size_2) {
    double max = 0;
    for (const double x : S2_array_1) {
        max = std::max(max, x);
    }

    boost::math::fisher_f dist(size_1 - 1, size_2 - 1);
    double f_tb = boost::math::quantile(boost::math::complement(dist, 0.05));

    double f_p = max / S2_inp_1;

    //std::cout << "f_p: " << f_p << " f_tb: " << f_tb << std::endl;
    if (f_p > f_tb) {
        return false;
    }

    return true;
}
