#include <vector>
#include "gurobi_c++.h"

#ifndef AffineFunction_h
#define AffineFunction_h

using namespace std;

/*
 * Represents an affine function of the form <a, x> + b for d-vectors a, x and
 * a number b.
 */
class AffineFunction {
public:
    vector<double> a;
    double b;

    AffineFunction(vector<double> &a, double b) : a(a), b(b) {};

    GRBLinExpr to_grb_expression(GRBVar *vars, unsigned int d);
};

#endif /* AffineFunction_h */
