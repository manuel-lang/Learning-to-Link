#include "assert.h"
#include <vector>
#include "AffineFunction.h"


#ifndef Polytope_h
#define Polytope_h

using namespace std;

/*
 * Represents a d-dimensional closed polytope as a list of half-space
 * constraints. Each half-space constraint is given as <a, x> + b >= 0,
 * i.e. an affine function that must be non-negative.
 */
class Polytope {
public:
    vector<AffineFunction> constraints;
    unsigned int d; // the dimension this polytope lives in

    Polytope(vector<AffineFunction> const &constraints, unsigned int d) {
        // Check that every affine function has the right dimension
        for(auto halfspace : constraints) {
            assert(halfspace.a.size() == d);
        }
        this->d = d;
        this->constraints = constraints;
    }
};

#endif /* Polytope_h */
