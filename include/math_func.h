bool check_delay_outliers2(const std::vector<unsigned int>* delays);
double expected_value(const std::vector<unsigned int>* delays, int element);
double loov(const std::vector<unsigned int>* delays, int element, double M);
double loov(const std::vector<unsigned int>* delays, int element);
double t_value(int element, double M, double S);
double AVG(const std::vector<unsigned int>* delays);
double sv(const std::vector<unsigned int>* delays, const double M);
long double S(double S2_1, double S2_2, int n);
long double t_value2(double M1, double M2, long double S_general, int n);
