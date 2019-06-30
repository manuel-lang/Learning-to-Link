#ifndef InitOperations_h
#define InitOperations_h

#include "DistanceFunction.h"
#include "Helpers.h"
#include "State.h"

/**
  * Get the initial distances between all points - each point describes a cluster.
   * The vector is represented as a flattened n x n matrix with the clusterwise distances between i and j in n where
   * all redundant values are cancelled out.
 * @tparam T - the numeric feature type
 * @param feature_vectors - a vector of all feature vectors (i.e. points)
 * @param len - the amount of feature vectors
 * @return euclidean distances between all points
 */
template<typename T>
std::vector<double> getdists(const std::vector<std::vector<T> > &feature_vectors, size_t len) {
    std::vector<double> dists;
    for (auto i = 0; i < len; i = i + 1) {
        for (auto j = i + 1; j < len; j = j + 1) {
            dists.push_back(DistanceFunction::euclidean_dist(feature_vectors[i], feature_vectors[j]));
        }
    }
    return dists;
}

/**
 * Gets the initial nodes for a clustering, where each node represents one point
 * @tparam T - the numeric label type
 * @param concrete_labels - all labels
 * @param different_labels - all unique labels
 * @return all initial nodes
 */
template<typename T>
std::vector<ClusterNode *> getnodes(const std::vector<T> &concrete_labels, const std::vector<T> &different_labels) {
    std::vector<ClusterNode *> nodes;
    for (auto i = 0; i < concrete_labels.size(); i++) {
        auto pos = std::find(different_labels.begin(), different_labels.end(), concrete_labels[i]) -
                   different_labels.begin();
        std::vector<int> counts(different_labels.size(), 0);
        counts[pos] = 1;
        nodes.push_back(new ClusterNode(NULL, NULL, counts, false));
    }
    return nodes;
}

 /**
  * Get the initial SC_State from feature vectors and labels without redundant distance values
  * @tparam T - the numeric label type
  * @param state - the output initial state
  * @param feature_vectors - input feature vectors
  * @param concrete_labels - all labels
  * @param different_labels - all unique labels
  */
template<typename T>
void getinitstate(SC_State &state, const std::vector<std::vector<T> > &feature_vectors,
                  const std::vector<T> &concrete_labels, const std::vector<T> &different_labels) {
    std::vector<double> minmaxdists = getdists(feature_vectors, concrete_labels.size());
    std::vector<ClusterNode *> nodes = getnodes(concrete_labels, Helpers::getUniqueValues(concrete_labels));
    std::vector<long> active_indices;
    std::vector<short> cluster_sizes;
    active_indices.reserve(concrete_labels.size());
    for (auto i = 0; i < concrete_labels.size(); i++) {
        active_indices.push_back(i);
    }
    state = SC_State(0.0, 1.0, minmaxdists, minmaxdists, active_indices, nodes);
}

/**
 * Get the initial SA_State from feature vectors and labels without redundant distance values
 * @tparam T - the numeric label type
 * @param state - the output initial state
 * @param feature_vectors - input feature vectors
 * @param concrete_labels - all labels
 * @param different_labels - all unique labels
 */
template<typename T>
void getinitstate(SA_State &state, const std::vector<std::vector<T> > &feature_vectors,
                  const std::vector<T> &concrete_labels, const std::vector<T> &different_labels) {
    std::vector<double> minavgdists = getdists(feature_vectors, concrete_labels.size());
    std::vector<ClusterNode *> nodes = getnodes(concrete_labels, different_labels);
    std::vector<long> active_indices;
    std::vector<short> cluster_sizes;
    for (auto i = 0; i < concrete_labels.size(); i++) {
        active_indices.push_back(i);
        cluster_sizes.push_back(1);
    }
    state = SA_State(0.0, 1.0, minavgdists, minavgdists, active_indices, nodes, cluster_sizes);
}

/**
 * Get the initial AC_State from feature vectors and labels without redundant distance values
 * @tparam T - the numeric label type
 * @param state - the output initial state
 * @param feature_vectors - input feature vectors
 * @param concrete_labels - all labels
 * @param different_labels - all unique labels
 */
template<typename T>
void getinitstate(AC_State &state, const std::vector<std::vector<T> > &feature_vectors,
                  const std::vector<T> &concrete_labels, const std::vector<T> &different_labels) {
    std::vector<double> avgmaxdists = getdists(feature_vectors, concrete_labels.size());
    std::vector<ClusterNode *> nodes = getnodes(concrete_labels, different_labels);
    std::vector<long> active_indices;
    std::vector<short> cluster_sizes;
    for (auto i = 0; i < concrete_labels.size(); i++) {
        active_indices.push_back(i);
        cluster_sizes.push_back(1);
    }
    state = AC_State(0.0, 1.0, avgmaxdists, avgmaxdists, active_indices, nodes, cluster_sizes);
}


#endif /* InitOperations_h */
