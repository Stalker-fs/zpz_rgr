#include <gtest/gtest.h>
#include "../src/math_func.cpp"

struct DelayCase {
    std::vector<unsigned int> delays;
    bool expected;
};
TEST(MathFuncTest, CheckDelayOutliers) {
    std::vector<DelayCase> cases = {
        {{10, 9, 11, 12, 9, 10}, true},
        {{100, 102, 95, 500}, false},
        {{1, 1, 1, 1, 1, 1, 1, 1}, true},
        {{10, 100, 10, 100}, true},
        {{10, 11, 9, 10, 11, 10}, true},
        {{10, 11, 11, 100}, false},
        {{5, 6, 7, 8, 100}, false},
        {{1000, 1001, 999, 1002, 1001}, true},
        {{1000, 1001, 999, 1002, 5000}, false},
        {{50, 49, 51, 48, 52, 47}, true},
        {{50, 49, 51, 48, 52, 150}, false},
        {{2, 5, 1, 12}, false},
        {{2, 2, 2, 2, 2, 99}, false},
        {{25, 26, 24, 23, 200, 90, 1, 65}, false},
        {{25, 26, 24, 23, 25}, true},
        {{0, 1, 0, 1, 1}, true},
        {{0, 1, 0, 1, 0, 5}, false},
        {{15, 16, 14, 15, 16}, true},
        {{15, 16, 15, 80}, false}
    };

    for (size_t i = 0; i < cases.size(); ++i) {
        const auto& tc = cases[i];
        EXPECT_EQ(check_delay_outliers2(&tc.delays), tc.expected) << "Failed on test case: " << i;
    }
}
