#include "../lib/Clarkson.h"
#include "../lib/Polytope.h"
#include "../lib/AffineFunction.h"
#include "ClarksonTest.h"
#include "TestHelp.h"
#include <vector>
#include <algorithm>
#include <printf.h>

// Check for simple non-redundancy

AffineFunction ClarksonTest::of_list(double input[]) {
    std::vector<double> coeffs = std::vector<double>();

    coeffs.push_back(input[0]);
    coeffs.push_back(input[1]);

    return AffineFunction(coeffs, input[2]);
};

void ClarksonTest::simple_polygon_test() {
    double coeffs[6][3] = {
        {1, 1, 0},
        {1, 1, 1},
        {-1, 0, -2},
        {-2, -1, 4},
        {0, -1, 4},
        {-1, -1, 1}
    };

    std::vector<AffineFunction> constraints;
    for(int i = 0; i < 6; i++) {
        constraints.push_back(ClarksonTest::of_list(coeffs[i]));
    }

    Polytope boundary = Polytope(constraints, 2);
    std::vector<int> non_redundant = *Clarkson::non_redundant_indices(boundary);
    std::vector<int> expected = std::vector<int>({ 0, 2, 4, 5});

    TestHelp::assert_same_elements(non_redundant, expected);
};

void ClarksonTest::triangle_many_redundant_test() {
    double coeffs[18][3] = {
        {-1, 1, 0}, // non-redundant
        {0, -1, 5},
        {0, -1, 5.3},
        {0, -1, 11},
        {0, -1, 114.5},
        {-1, -1, 10},
        {-1, -1, 200},
        {5, -1, -5}, // non-redundant
        {-1, -1, 34.4},
        {-1, -1, 54},
        {0, 1, 4},
        {0, 2, 4},
        {1, 0, 44},
        {0, -1, 4}, // non-redundant
        {1, 0, 0.4},
        {1, 0, 1544.2},
        {43.2, 0, 13},
        {-1, 0, 5},
    };

    std::vector<AffineFunction> constraints;
    for(int i = 0; i < 18; i++) {
        constraints.push_back(ClarksonTest::of_list(coeffs[i]));
    }

    Polytope boundary = Polytope(constraints, 2);
    std::vector<int> non_redundant = *Clarkson::non_redundant_indices(boundary);
    std::vector<int> expected = std::vector<int>({ 0, 7, 13});

    TestHelp::assert_same_elements(non_redundant, expected);
};

void ClarksonTest::run() {
    Clarkson::init_env();
    printf("Running Clarkson's tests...\n");
    ClarksonTest::simple_polygon_test();
    ClarksonTest::triangle_many_redundant_test();
}
