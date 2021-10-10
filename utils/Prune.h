#ifndef Prune_h
#define Prune_h

#include <map>
#include <limits>

#include "stddef.h"

#include "CostFunction.h"

#include "ClusterNode.h"
#include "Pruning.h"

/**
 * Get the optimal prunings for a hierarchical clustering (i.e. pruning for majority distance)
 * @param node - the root node
 * @param max_k - the amount of target clusters
 * @return a map matching amount of clusters to optimal prunings
 */
inline std::map<int, Pruning> prune(ClusterNode const &node, size_t max_k)
{
    std::map<int, Pruning> opt_prunings;
    opt_prunings.insert( std::pair<int,Pruning>(1,Pruning(CostFunction::majority_cost(node))) );

    if(node.has_children)
    {
        std::map<int, Pruning> left_opt_prunings = prune(*node.left, max_k);
        std::map<int, Pruning> right_opt_prunings = prune(*node.right, max_k);

        for(auto k = 2; k <= max_k; k++)
        {
            Pruning best_k_pruning = Pruning(std::numeric_limits<double>::infinity());
            for(int left_k = 1; left_k <= k-1; left_k++)
            {
                int right_k = k - left_k;
                if(left_opt_prunings.count( left_k ) != 0 && right_opt_prunings.count( right_k ) != 0)
                {
                    Pruning left_opt = left_opt_prunings.find(left_k)->second;
                    Pruning right_opt = right_opt_prunings.find(right_k)->second;
                    Pruning pruning = Pruning(left_opt.cost + right_opt.cost);
                    if(pruning.cost < best_k_pruning.cost)
                    {
                        best_k_pruning = pruning;
                    }
                }
            }
            opt_prunings.insert( std::pair<int, Pruning>(k, best_k_pruning) );
        }
    }
    return opt_prunings;
}

/**
 * Get all possible prunings for a node into k clusters.
 * @tparam T - the type indicating the number of clusters
 * @param node - node containing all clusters
 * @param k - number of clusters
 * @return all possible prunings of the node into k clusters
 */
template <typename T>
std::vector<std::vector<ClusterNode> > all_prunings(ClusterNode const &node, T k) {
    if(k == 1)
    {
        std::vector<std::vector<ClusterNode> > ret;
        std::vector<ClusterNode> ret_d;
        ret_d.push_back(node);
        ret.push_back(ret_d);
        return ret;
    }
    else {
        std::vector<std::vector<ClusterNode> > prunings;
        if(node.has_children) {
            for(auto left_k = 1; left_k < k; left_k++) {
                auto right_k = k - left_k;
                std::vector<std::vector<ClusterNode> > left_prunings = all_prunings(*node.left, left_k);
                std::vector<std::vector<ClusterNode> > right_prunings = all_prunings(*node.right, right_k);

                if(!left_prunings.empty() && !right_prunings.empty())
                {
                    for(const std::vector<ClusterNode> &vec_l : left_prunings)
                    {
                        for(const std::vector<ClusterNode> &vec_r : right_prunings)
                        {
                            std::vector<ClusterNode> pruning;
                            for(const ClusterNode &n_l : vec_l)
                            {
                                pruning.push_back(n_l);
                            }
                            for(const ClusterNode &n_r : vec_r)
                            {
                                pruning.push_back(n_r);
                            }
                            prunings.push_back(pruning);
                        }
                    }
                }
            }
        }
        return prunings;
    }
}

/**
 * Find the best pruning (i.e. with the lowest hamming distance) of a node into k clusters
 * @tparam T - type indicating the number of clusters
 * @param node - contains all clusters
 * @param k - amount of target clusters
 * @return the optimal pruning (i.e. hamming cost) of a node into k clusters
 */
template <typename T>
Pruning best_pruning(ClusterNode const &node, T k) {
    std::vector<std::vector<ClusterNode> > prunings = all_prunings(node, k);
    std::vector<ClusterNode> best_pruning = prunings[0];
    double best_cost = std::numeric_limits<double>::infinity();
    double cost;
    for(const std::vector<ClusterNode> &nodes : prunings) {
        cost = CostFunction::hamming_cost(nodes);
        if(cost < best_cost) {
            best_pruning = nodes;
            best_cost = cost;
        }
    }
    return {best_cost};
}

#endif /* Prune_h */
