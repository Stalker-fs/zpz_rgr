float expected_value(const std::vector<unsigned int>* delays, int element);
float loov(const std::vector<unsigned int>* delays, int element, float M);
float loov(const std::vector<unsigned int>* delays, int element);
float t_value(int element, float M, float S);
float AVG(const std::vector<unsigned int>* delays);
float sv(const std::vector<unsigned int>* delays, const float M);
float S(float S2_1, float S2_2, int n_1, int n_2);
float t_value2(float M1, float M2, float S_general, int n1, int n2);
