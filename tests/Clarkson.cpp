#include "../lib/Clarkson.h"
#include "../lib/Polytope.h"
#include "Clarkson.h"
#include <vector>
#include <printf.h>

// Check for simple non-redundancy

AffineFunction of_list(double input[]) {
    std::vector<double> coeffs = std::vector<double>();

    coeffs.push_back(input[0]);
    coeffs.push_back(input[1]);

    return AffineFunction(coeffs, input[2]);
};

void simple_polygon_test() {
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
        constraints.push_back(of_list(coeffs[i]));
    }

    Polytope boundary = Polytope(constraints, 2);
    std::vector<int> *non_redundant = Clarkson::non_redundant_indices(boundary);

    for(int i : *non_redundant) {
        printf("%d\n", i);
    }
};

void ClarksonTest::run() {
    simple_polygon_test();
}
