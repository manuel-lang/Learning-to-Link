#include "CostFunction.h"

#include <algorithm>
#include <numeric>
#include "../lib/Hungarian.h"


/*!
 * Majority cost compares the maximum count to all counts for a given node. We use it to assign a cluster to the best
 * matching target cluster.
 */
double CostFunction::majority_cost(ClusterNode const &node) {
    return std::accumulate(node.counts.begin(), node.counts.end(), 0) -
           *std::max_element(node.counts.begin(), node.counts.end());
}

/*!
 * Hamming cost calculates the costs with an optimal matching between a generated and a target clustering using the
 * hungarian algorithm (https://en.wikipedia.org/wiki/Hungarian_algorithm).
 */
double CostFunction::hamming_cost(std::vector<ClusterNode> nodes) {
    std::vector<std::vector<double> > cost_matrix;
    for (ClusterNode node : nodes) {
        std::vector<double> cur_cost;
        int sum = std::accumulate(node.counts.begin(), node.counts.end(), 0);
        for (auto count : node.counts) {
            cur_cost.push_back((double) (sum - count));
        }
        cost_matrix.push_back(cur_cost);
    }
    std::vector<int> assignment;
    HungarianAlgorithm HungAlgo;
    return HungAlgo.Solve(cost_matrix, assignment);
}
