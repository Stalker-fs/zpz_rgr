#include <boost/math/distributions/students_t.hpp>
#include <boost/math/distributions/fisher_f.hpp>
#include <map>

#include "../include/math_utils.h"
#include "../include/config.h"

#ifdef DEBUG
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"
#endif

bool fisher(std::vector<float>& S2_array_1, float S2_inp_1, int size_1, int size_2) {
    float max = 0;
    for (const float x : S2_array_1) {
        max = std::max(max, x);
    }

    boost::math::fisher_f dist(size_1 - 1, size_2 - 1);
    float f_tb = boost::math::quantile(boost::math::complement(dist, 0.05));

    float f_p = max / S2_inp_1;

    #ifdef DEBUG
    std::cout << "\n<---Fisher's test--->" << std::endl;
    std::cout << ((f_p > f_tb) ? RED "Failure" RESET : GREEN "Success" RESET) << " T_p: " << f_p << ((f_p > f_tb) ? " > " : " <= ") << "T_tb: " << f_tb << std::endl;
    #endif

    if (f_p > f_tb) {
        return false;
    }

    return true;
}

bool check_delay_outliers2(const std::vector<unsigned int>* delays) {
    boost::math::students_t dist(delays->size() - 2);
    float t_tb = boost::math::quantile(boost::math::complement(dist, 0.025 / 2));

    for (const unsigned int x : *delays) {
        float M = expected_value(delays, x);        
        float S2 = loov(delays, x, M);
        float S = std::sqrt(S2);

        float t_p = t_value(x, M, S);

        if (t_p > t_tb) {
            return false;
        }
    }
    return true;
}

void search_user(std::vector<unsigned int>& a_delays, Config &conf, std::pair<std::string, float>& winner, std::pair<float, float>& M_S2_pair, float p) {
    boost::math::students_t dist(a_delays.size() - 1);
    float t_tb = boost::math::quantile(boost::math::complement(dist, (1 - p) / 2));

    float M = AVG(&a_delays);
    float S2 = sv(&a_delays, M);

    M_S2_pair = {M, S2};

    std::vector<std::string> u_list;
    conf.get_user_list(u_list);

    for (std::string &u : u_list) {
        #ifdef DEBUG
        std::cout << std::string(100, '#') << std::endl;
        std::cout << "User: " << u << std::endl;
        #endif

        std::vector<float> S2_array;
        conf.get_S2(u, S2_array);

        if (!fisher(S2_array, S2, conf.get_phrase_len(u), a_delays.size())) {
            continue;
        }

        float r = 0;
        std::map<float, float> param;
        conf.get_M_S2(u, param);

        #ifdef DEBUG
        std::cout << "\n<---Student's t-test--->" << std::endl;
        std::cout << std::left
        << std::setw(10) << "status"
        << std::setw(10) << "T_p"
        << std::setw(5) << ""
        << std::setw(10) << "T_tb"
        << std::setw(10) << "M_etalon"
        << std::setw(10) << "M_input"
        << std::setw(15) << "S^2_etalon"
        << std::setw(20) << "S^2_input"
        << std::endl;
        std::cout << std::string(100, '-') << std::endl;
        #endif
        
        for (const auto& [M_e, S2_e] : param) {
            float S_general = S(S2, S2_e, a_delays.size(), conf.get_phrase_len(u));
            float t_p_ = t_value2(M_e, M, S_general, a_delays.size(), conf.get_phrase_len(u));
            if (t_p_ <= t_tb) {
                r++;
            }

            #ifdef DEBUG
            std::cout << std::left
            << std::setw(20) << ((t_p_ <= t_tb) ? GREEN "Success" RESET : RED "Failure" RESET)
            << std::setw(10) << t_p_
            << std::setw(5) << ((t_p_ <= t_tb) ? "<=" : ">")
            << std::setw(10) << t_tb
            << std::setw(10) << M_e
            << std::setw(10) << M
            << std::setw(15) << S2_e
            << std::setw(20) << S2
            << std::endl;
            #endif
        }

        float P = r / param.size();

        if (P > winner.second) {
            winner = {u, P};
        }
    }
}

void calc_variance(std::vector<std::vector<unsigned int>*>& delays, std::map<float, float>& S2) {
    for (std::vector<unsigned int>* x : delays) {
        float M = AVG(x);
        S2[M] = sv(x, M);
    }
}