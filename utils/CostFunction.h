#ifndef CostFunction_h
#define CostFunction_h

#include "ClusterNode.h"
#include <vector>

namespace CostFunction {

    /**
     * Calculates the majority cost based on the optimal resulting cluster tree.
     * @param node - the optimal root node containing all clusters
     * @return majority cost of a root node pruned into k clusters
     */
    double majority_cost(ClusterNode const &node);

    /**
     * Calculates the hamming cost based on all resulting cluster trees.
     * @param nodes - all root nodes containg  all clusters
     * @return hamming cost of optimal root node pruned into k clusters
     */
    double hamming_cost(std::vector<ClusterNode> nodes);
};

#endif /* CostFunction_h */
