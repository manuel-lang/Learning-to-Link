#ifndef Clustering_h
#define Clustering_h

#include "AlphaRange.h"
#include "Instersection.h"
#include "State.h"
#include "SplitState.h"

#include "Merge.h"
#include "Prune.h"

#include <iostream>
#include <stack>
#include <string>
#include <fstream>

namespace Clustering {

    /**
      * Calculates the nearest interestion of a given linear function distance(alpha) based on the pairwise distances
      * of all clusters.
      * @param lower_dists - a distance vector that stores the pairwise distance for alpha = 0
      * @param upper_dists - a distance vector that stores the pairwise distances for alpha = 1
      * @param active_indices - all cluster indices that have not been merged
      * @param lf_in - the input linear function distance(alpha) which is used to calculate the next intersection
      * @param alpha_start - the current value of alpha
      * @param alpha_end - the maximum value of alpha in the given interval
      * @param width - the width of the distance matrix, used to calculate indices in the flattened matrix that is given
      * as vector
      * @return the position of the nearest intersection together with the resulting linear function and the resulting
      * merge clusters
      */
    Intersection calculate_nearest_intersection(std::vector<double> const &lower_dists,
                                                std::vector<double> const &upper_dists,
                                                std::vector<long> const &active_indices,
                                                LinearFunction lf_in,
                                                double alpha_start, double alpha_end, size_t width) {
        LinearFunction lf_out, lf_opt;
        auto alpha_min = alpha_end;
        long i1, i2;
        MergeCandidate indices;
        double intersection;
        for (auto i = 0; i < active_indices.size(); i++) {
            i1 = Helpers::get_reduced_matrix_outter_index(width, active_indices[i]);
            for (auto j = i + 1; j < active_indices.size(); j++) {
                i2 = i1 + active_indices[j];
                lf_out = LinearFunction(upper_dists[i2] - lower_dists[i2], lower_dists[i2]);
                intersection = lf_in.calculate_interaction_with(lf_out);
                if (intersection < alpha_min && intersection > alpha_start) {
                    indices = MergeCandidate(active_indices[i], active_indices[j]);
                    alpha_min = intersection;
                    lf_opt = lf_out;
                }
            }
        }
        return {alpha_min, lf_opt, indices};
    }

    /**
     * Calculates all children nodes for a parent node.
     * @param min - the search space's lower alpha bound
     * @param max - the search space's upper alpha bound
     * @param lower_dists - the pairwise distances for alpha = 0
     * @param upper_dists - the pairwise distances for alpha = 1
     * @param active_indices - the incdices of all clusters that were not merged yet
     * @param size - the size of the pairwise distance matrix that was flattened
     * @param states - the parent state that will be overwritten by the children states
     */
    void getsplitstates(double min, double max, std::vector<double> const &lower_dists,
                        std::vector<double> const &upper_dists,
                        std::vector<long> const &active_indices, size_t size,
                        std::vector<SplitState> &states) {
        std::pair<LinearFunction, MergeCandidate> lower = find_merge_candidates(lower_dists, upper_dists,
                                                                                active_indices, min, size);
        std::pair<LinearFunction, MergeCandidate> upper = find_merge_candidates(lower_dists, upper_dists,
                                                                                active_indices, max, size);
        if (std::get<1>(lower).cluster1 != std::get<1>(upper).cluster1 ||
            std::get<1>(lower).cluster2 != std::get<1>(upper).cluster2) {
            Intersection is;
            auto alpha = min;
            while (alpha < max) {
                is = calculate_nearest_intersection(lower_dists, upper_dists, active_indices, std::get<0>(lower), alpha,
                                                    max, size);
                states.emplace_back(std::get<1>(lower), alpha, is.alpha);
                lower = std::pair<LinearFunction, MergeCandidate>(is.next_function, is.next_merge);
                alpha = is.alpha;
            }
        } else {
            states.emplace_back(std::get<1>(lower), min, max);
        }
    }

    /**
     * Finds all intervals by interpolating depending on the given input state and returns them as a vector<AlphaRange>.
     * @param states - a vector of states containing the input state
     * @param output_file - the file the ranges are written into
     * @param labels_size - the amount of different classes
     * @param maxlabel - the highest class number
     * @param verbose - output directly to console
     * @param average - calculate average over multiple files
     * @param use_majority - use majority cost instead of hamming cost
     * @return a vector with all ranges that contain an interval [a_min, a_max] and a loss value for each interval
     */
    template<typename S>
    std::vector<AlphaRange>
    getranges(std::vector<S> states, std::string output_file, unsigned long labels_size,
              unsigned long maxlabel, bool verbose, bool average, bool use_majority) {
        std::vector<AlphaRange> ranges;
        std::ofstream myfile;
        if (!output_file.empty()) {
            myfile.open(output_file);
        }
        while (!states.empty()) {

            // leaf node
            if (states[0].active_indices.size() == 1) {
                double cost;

                // calculate majority cost
                if (use_majority) {
                    std::map<int, Pruning> pruning_strats = prune(*states[0].nodes[*states[0].active_indices.begin()],
                                                                  maxlabel);
                    cost = pruning_strats.find(maxlabel)->second.cost / (double) labels_size;
                }

                    // calculate hamming cost
                else {
                    cost = best_pruning(*states[0].nodes[*states[0].active_indices.begin()], maxlabel).cost /
                           (double) labels_size;
                }

                // store range
                if (average) {
                    ranges.emplace_back(states[0].alpha_min, states[0].alpha_max, cost);
                }

                    // output range to file
                else if (!output_file.empty()) {
                    myfile << states[0].alpha_min << "," << states[0].alpha_max << "," << cost << "\n";
                }

                // output range to console
                if (verbose) {
                    std::cout << states[0].alpha_min << "," << states[0].alpha_max << "," << cost << std::endl;
                }
                states.erase(std::remove(states.begin(), states.end(), states[0]), states.end());
            }

                // nodes that are no leafs are getting processed further
            else {

                // take first element from the tree of executions and calculate resulting children
                // (a node can result in 1, 2 or more children)
                std::vector<SplitState> splitstates;
                getsplitstates(states[0].alpha_min, states[0].alpha_max, states[0].lower_dists,
                               states[0].upper_dists, states[0].active_indices, labels_size, splitstates);
                for (auto j = 0; j < splitstates.size() - 1; ++j) {
                    S temp = states[j];
                    merge_clusters(temp, splitstates[j].merge_candidate.cluster1,
                                   splitstates[j].merge_candidate.cluster2,
                                   labels_size);
                    temp.alpha_min = splitstates[j].alpha_min;
                    temp.alpha_max = splitstates[j].alpha_max;
                    states.insert(states.begin() + j, temp);
                }
                // overwrite the parent node with the last child for better performance
                merge_clusters(states[splitstates.size() - 1],
                               splitstates[splitstates.size() - 1].merge_candidate.cluster1,
                               splitstates[splitstates.size() - 1].merge_candidate.cluster2, labels_size);
                states[splitstates.size() - 1].alpha_min = splitstates[splitstates.size() - 1].alpha_min;
                states[splitstates.size() - 1].alpha_max = splitstates[splitstates.size() - 1].alpha_max;
            }
        }
        myfile.close();
        return ranges;
    }
};

#endif /* Clustering_h */
