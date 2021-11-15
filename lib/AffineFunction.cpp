#include <vector>
#include "assert.h"
#include "gurobi_c++.h"
#include "AffineFunction.h"
#include "DebugAssert.h"

GRBLinExpr AffineFunction::to_grb_expression(GRBVar *vars, unsigned int d) {
    dbg_assert(d == a.size());

    GRBLinExpr expr;
    expr.addTerms(a.data(), vars, d);
    expr += b;

    return expr;
};


double AffineFunction::linear_eval(std::vector<double> &x) {
    dbg_assert(x.size() == a.size());

    double result = 0.0;
    unsigned int d = a.size();

    for(int i = 0; i < d; i++) {
        result += a[i] * x[i];
    }
    return result;
};

double AffineFunction::eval(std::vector<double> &x) {
    return linear_eval(x) + b;
};

