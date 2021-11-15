// Implementation of Clarkson's algorithm

#include "AffineFunction.h"
#include "Polytope.h"
#include "Clarkson.h"
#include "gurobi_c++.h"
#include <unordered_set>
#include <vector>

Polytope *Clarkson::eliminate_redundancy(Polytope &boundary)
{
    std::unordered_set<int> remaining;
    std::vector<int> nonredundant;

    int num_constraints = boundary.constraints.size();
    unsigned int d = boundary.d;

    for(int i = 0; i < num_constraints; i++) {
        remaining.insert(i);
    }

    // Before redundancy checking, we need an interior point of the polytope
    std::vector<double> *z = Clarkson::interior_point(boundary);

    while(!remaining.empty()) {
        int test_index = *remaining.begin();

        Clarkson::redundancy_result res = Clarkson::single_redundancy_check(
            boundary, *z, nonredundant, test_index
        );

        // Sometimes we get lucky and find a non-redundant constraint;
        if(!res.is_redundant) {
            nonredundant.push_back(res.index);
        }

        remaining.erase(res.index);
    }

    // We have the nonredundant constraints; just have to add them to a new polytope
    std::vector<AffineFunction> new_constraints;
    for(int i : nonredundant) {
        new_constraints.push_back(boundary.constraints[i]);
    }

    return new Polytope(new_constraints, d);
};

Clarkson::redundancy_result Clarkson::single_redundancy_check(
    Polytope &boundary, std::vector<double> &starting_point,
    std::vector<int> &indices, int test_index)
{
    // Set up LP for checking single redundancy
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);

    unsigned int d = boundary.d;

    GRBVar *vars = model.addVars(d, GRB_CONTINUOUS);

    for(int i : indices) {
        model.addConstr(boundary.constraints[i].to_grb_expression(vars, d) >= 0);
    }

    GRBLinExpr test_expr = boundary.constraints[test_index].to_grb_expression(vars, d);

    // The trick: add this constraint slightly looser than it actually needs to be. If
    // the resulting polytope grows (i.e. minimizing this expression reaches sub-zero),
    // then test_index is non-redundant.
    model.addConstr(test_expr >= -1);
    model.setObjective(test_expr, GRB_MINIMIZE);

    model.optimize();

    Clarkson::redundancy_result result;
    if(model.getObjective().getValue() < 0) {
        // A non-redundant constraint is out there -- now have to ray shoot to figure out which
        // one is the tightest.
        result.is_redundant = false;

        std::vector<double> shoot_direction;
        for(int i = 0; i < d; i++) {
            shoot_direction.push_back(vars[i].get(GRB_DoubleAttr_X) - starting_point[i]);
        }

        result.index = ray_shoot(boundary, starting_point, shoot_direction);
    } else {
        // Redundant
        result.is_redundant = true;
        result.index = test_index;
    }

    return result;
};

// TODO: Implement these
std::vector<double>* Clarkson::interior_point(Polytope &boundary) {
    std::vector<double>* point = new std::vector<double>();
    return point;
};

int Clarkson::ray_shoot(
    Polytope &boundary, std::vector<double> &starting_point,
    std::vector<double> &direction) {
    return 0;
};
