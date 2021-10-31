#include <vector>
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