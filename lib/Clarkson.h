///////////////////////////////////////////////////////////////////////////////
// Clarkson.h: Header file for the class Clarksons 
//
// This implements Clarkson's algorithm for eliminating redundant constraints
// from a polytope. If there are s nonredundant constraints, d total variables, and m
// total constraints, this runs in time O(m * LP(d, s)), where LP(d, s) is the time
// required to solve a linear program with d dimensions and s constraints.
//

#ifndef Clarkson_h
#define Clarkson_h

#include "AffineFunction.h"
#include "Polytope.h"
#include "gurobi_c++.h"


class Clarkson
{
public: 
    Clarkson();
    ~Clarkson();
    Polytope *eliminate_redundancy(Polytope &boundary);
    
private:
    // When a redundancy check is performed, it returns either a redundant constraint index
    // or a provably non-redundant constraint index.
    struct redundancy_result {
        bool is_redundant;
        int index;
    };

    GRBEnv env = GRBEnv();

    // Returns an interior point of the polytope
    std::vector<double> *interior_point(Polytope &boundary);

    // Checks whether constraint k is redundant in the LP given by constraint set indices and test_index
    redundancy_result single_redundancy_check(
        Polytope &boundary, std::vector<double> &starting_point,
        std::vector<int> &indices, int test_index);

    // Returns an essential index
    int ray_shoot(
        Polytope &boundary, std::vector<double> &starting_point,
        std::vector<double> &direction);
};

#endif /* Clarkson_h */
