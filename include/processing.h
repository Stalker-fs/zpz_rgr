bool fisher(std::vector<float>& S2_array_1, float S2_inp_1, int size_1, int size_2);
void search_user(std::vector<unsigned int>& a_delays, Config &conf, std::pair<std::string, float>& winner);
bool check_delay_outliers2(const std::vector<unsigned int>* delays);
void calc_variance(std::vector<std::vector<unsigned int>*>& delays, std::map<float, float>& S2);
