// Implementation of Clarkson's algorithm

#include "AffineFunction.h"
#include "Polytope.h"
#include "Clarkson.h"
#include "gurobi_c++.h"
#include <unordered_set>
#include <vector>

GRBEnv Clarkson::env = GRBEnv();
void Clarkson::init_env() {
    env.set(GRB_IntParam_LogToConsole, 0);
};


Polytope *Clarkson::non_redundant_polytope(Polytope &boundary)
{
    unsigned int d = boundary.d;
    std::vector<int> *non_redundant = non_redundant_indices(boundary);
    std::vector<AffineFunction> new_constraints;
    for(int i : *non_redundant) {
        new_constraints.push_back(boundary.constraints[i]);
    }
    return new Polytope(new_constraints, d);
}

std::vector<int> *Clarkson::non_redundant_indices(Polytope &boundary)
{
    std::unordered_set<int> remaining;
    std::vector<int> *nonredundant = new std::vector<int>();

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
            boundary, *z, *nonredundant, test_index
        );

        // Sometimes we get lucky and find a non-redundant constraint;
        if(!res.is_redundant) {
            nonredundant->push_back(res.index);
        }

        remaining.erase(res.index);
    }
    
    return nonredundant;
};

Clarkson::redundancy_result Clarkson::single_redundancy_check(
    Polytope &boundary, std::vector<double> &starting_point,
    std::vector<int> &indices, int test_index)
{
    // Set up LP for checking single redundancy
    GRBModel model = GRBModel(Clarkson::env);

    unsigned int d = boundary.d;

    GRBVar *vars = model.addVars(d, GRB_CONTINUOUS);
    for(int i = 0; i < d; i++) {
        vars[i].set(GRB_DoubleAttr_LB, -GRB_INFINITY);
        vars[i].set(GRB_DoubleAttr_UB, GRB_INFINITY);
    }

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

std::vector<double>* Clarkson::interior_point(Polytope &boundary) {
    // The high-level idea behind finding an interior point is to add a
    // new variable, C (boundary_distance), which represents the overall distance that the solution
    // is from the hyperplane constraints. By maximizing C, we find a point
    // that is inside the boundary polytope and as far away from the boundaries
    // as possible; as long as C > 0 in the optimum solution (which happens whenever
    // the region is full-dimensional), we find an interior point.

    try {
        std::vector<double>* point = new std::vector<double>();
        GRBModel model = GRBModel(Clarkson::env);

        GRBVar boundary_distance = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "C");

        unsigned int d = boundary.d;
        unsigned int m = boundary.constraints.size();

        GRBVar *vars = model.addVars(d, GRB_CONTINUOUS);
        for(int i = 0; i < d; i++) {
            vars[i].set(GRB_DoubleAttr_LB, -GRB_INFINITY);
            vars[i].set(GRB_DoubleAttr_UB, GRB_INFINITY);
        }
        
        for(int i = 0; i < m; i++) {
            model.addConstr(boundary.constraints[i].to_grb_expression(vars, d) >= boundary_distance);
        }

        model.setObjective(GRBLinExpr(boundary_distance), GRB_MAXIMIZE);
        model.optimize();

        for(int i = 0; i < d; i++) {
            point->push_back(vars[i].get(GRB_DoubleAttr_X));
        }
        
        return point;
    } catch(GRBException e) {
        printf("Error code: %d\n", e.getErrorCode());
        printf("Error message: %s\n", e.getMessage());
        exit(1);
    }
};

double Clarkson::single_ray_shoot(
    Polytope &boundary, std::vector<double> &starting_point,
    std::vector<double> &direction, int index) {

    AffineFunction constraint = boundary.constraints[index];
    double direction_eval = constraint.linear_eval(direction);

    if(direction_eval == 0.0) {
        return GRB_INFINITY;
    }

    return -constraint.eval(starting_point) / direction_eval;
};

int Clarkson::ray_shoot(
    Polytope &boundary, std::vector<double> &starting_point,
    std::vector<double> &direction) {

    unsigned int d = boundary.d;
    unsigned int m = boundary.constraints.size();
    int best_index = -1;
    double best_distance = 0.0;
    for(int i = 0; i < m; i++) {
        double current_distance = single_ray_shoot(boundary, starting_point, direction, i);
        if(current_distance >= 0.0 && (best_index == -1 || current_distance < best_distance)) {
            best_index = i;
            best_distance = current_distance;
        }
    }
    return best_index;
};
