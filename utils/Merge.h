#ifndef merge_h
#define merge_h

#define float_inf std::numeric_limits<float>::infinity()

#include "Helpers.h"
#include "State.h"
#include "LinearFunction.h"
#include "MergeCandidate.h"

#include <limits>

/**
 *
 * Calculate the pairwise sum over two vectors.
 * @tparam T - numeric feature type
 * @param a - input vector a
 * @param b - input vector b
 * @return vector of pairwise sums
 */
template<typename T>
std::vector<T> operator+(const std::vector<T> &a, const std::vector<T> &b) {
    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::plus<T>());
    return result;
}

/**
 * Get the clusters and the resulting distance function that get merged for a certain alpha value with the distance
 * being (1-alpha) * lower_dist + alpha * upper_dist.
 * @param lower_dists
 * @param upper_dists
 * @param active_indices
 * @param alpha
 * @param width
 * @return the next linear function distance(alpha) and the next merge clusters i and j
 */
std::pair<LinearFunction, MergeCandidate> find_merge_candidates(std::vector<double> const &lower_dists,
                                                                std::vector<double> const &upper_dists,
                                                                std::vector<long> const &active_indices, double alpha,
                                                                size_t width) {
    LinearFunction lf;
    MergeCandidate indices;
    double mindist = std::numeric_limits<double>::infinity();
    double dist;
    long i1, i2;
    for (auto i = 0; i < active_indices.size(); i++) {
        i1 = Helpers::get_reduced_matrix_outter_index(width, active_indices[i]);
        for (auto j = i + 1; j < active_indices.size(); j++) {
            i2 = i1 + active_indices[j];
            dist = (1 - alpha) * lower_dists[i2] + alpha * upper_dists[i2];
            if (dist < mindist) {
                lf = LinearFunction(upper_dists[i2] - lower_dists[i2], lower_dists[i2]);
                indices = MergeCandidate(active_indices[i], active_indices[j]);
                mindist = dist;
            } else if (dist == mindist) {
                LinearFunction lf_new = LinearFunction(upper_dists[i2] - lower_dists[i2], lower_dists[i2]);
                if (lf_new.a < lf.a) {
                    indices = MergeCandidate(active_indices[i], active_indices[j]);
                    mindist = dist;
                    lf = lf_new;
                }
            }
        }
    }
    return {lf, indices};
}

/**
 * Update the single linkage distances for merging clusters i and j where the distance don't include redundant values
 * @param dists - single linkage distances
 * @param active_indices - instaces of cluster that were not merged yet
 * @param i - first merged cluster
 * @param j - second merged cluster
 * @param width - number of points (width of pairwise distance matrix)
 */
void
merge_min_dists(std::vector<double> &dists, std::vector<long> const &active_indices, long i, long j, size_t width) {
    long k, l;
    for (auto active_index : active_indices) {
        k = Helpers::get_reduced_matrix_index(width, std::min(i, active_index), std::max(i, active_index));
        l = Helpers::get_reduced_matrix_index(width, std::min(j, active_index), std::max(j, active_index));
        if (i != active_index) {
            dists[k] = std::min(dists[k], dists[l]);
        }
        if (j != active_index) {
            dists[l] = float_inf;
        }
    }
}

/**
 * Update the average linkage distances for merging clusters i and j where the distance don't include redundant values
 * @param dists - average linkage distances
 * @param cluster_sizes - the sized of all clusters
 * @param active_indices - instaces of cluster that were not merged yet
 * @param i - first merged cluster
 * @param j - second merged cluster
 * @param width - number of points (width of pairwise distance matrix)
 */
void merge_avg_dists(std::vector<double> &dists, std::vector<short> const &cluster_sizes,
                     std::vector<long> const &active_indices,
                     long i, long j, size_t width) {
    long k, l;
    for (auto active_index : active_indices) {
        k = Helpers::get_reduced_matrix_index(width, std::min(i, active_index), std::max(i, active_index));
        l = Helpers::get_reduced_matrix_index(width, std::min(j, active_index), std::max(j, active_index));
        if (i != active_index) {
            dists[k] =
                    (cluster_sizes[i] * dists[k] + cluster_sizes[j] * dists[l]) / (cluster_sizes[i] + cluster_sizes[j]);
        }
        if (i != active_index && j != active_index) {
            dists[l] = float_inf;
        }
    }
}

/**
 * Update the complete linkage distances for merging clusters i and j where the distance don't include redundant values
 * @param dists - complete linkage distances
 * @param active_indices - instaces of cluster that were not merged yet
 * @param i - first merged cluster
 * @param j - second merged cluster
 * @param width - number of points (width of pairwise distance matrix)
 */
void
merge_max_dists(std::vector<double> &dists, std::vector<long> const &active_indices, long i, long j, size_t width) {
    long k, l;
    for (auto active_index : active_indices) {
        k = Helpers::get_reduced_matrix_index(width, std::min(i, active_index), std::max(i, active_index));
        l = Helpers::get_reduced_matrix_index(width, std::min(j, active_index), std::max(j, active_index));
        if (i != active_index) {
            dists[k] = std::max(dists[k], dists[l]);
        }
        if (i != active_index && j != active_index) {
            dists[l] = float_inf;
        }
    }
}

/**
 * Merge clusters i and j when interpolating between single and complete linkage
 * @param st - current state
 * @param i - first merged cluster
 * @param j - second merged cluster
 * @param width - number of points (width of pairwise distance matrix)
 */
void merge_clusters(SC_State &st, long i, long j, size_t width) {
    merge_min_dists(st.lower_dists, st.active_indices, i, j, width);
    merge_max_dists(st.upper_dists, st.active_indices, i, j, width);
    st.active_indices.erase(std::remove(st.active_indices.begin(), st.active_indices.end(), j),
                            st.active_indices.end());
    st.nodes[i] = new ClusterNode(st.nodes[i], st.nodes[j], st.nodes[i]->counts + st.nodes[j]->counts, true);
}

/**
 * Merge clusters i and j when interpolating between single and average linkage
 * @param st - current state
 * @param i - first merged cluster
 * @param j - second merged cluster
 * @param width - number of points (width of pairwise distance matrix)
 */
void merge_clusters(SA_State &st, long i, long j, size_t width) {
    merge_min_dists(st.lower_dists, st.active_indices, i, j, width);
    merge_avg_dists(st.upper_dists, st.cluster_sizes, st.active_indices, i, j, width);
    st.active_indices.erase(std::remove(st.active_indices.begin(), st.active_indices.end(), j),
                            st.active_indices.end());
    st.cluster_sizes[i] = st.cluster_sizes[i] + st.cluster_sizes[j];
    st.nodes[i] = new ClusterNode(st.nodes[i], st.nodes[j], st.nodes[i]->counts + st.nodes[j]->counts, true);
}

/**
 * Merge clusters i and j when interpolating between average and complete linkage
 * @param st - current state
 * @param i - first merged cluster
 * @param j - second merged cluster
 * @param width - number of points (width of pairwise distance matrix)
 */
void merge_clusters(AC_State &st, long i, long j, size_t width) {
    merge_avg_dists(st.lower_dists, st.cluster_sizes, st.active_indices, i, j, width);
    merge_max_dists(st.upper_dists, st.active_indices, i, j, width);
    st.active_indices.erase(std::remove(st.active_indices.begin(), st.active_indices.end(), j),
                            st.active_indices.end());
    st.cluster_sizes[i] = st.cluster_sizes[i] + st.cluster_sizes[j];
    st.nodes[i] = new ClusterNode(st.nodes[i], st.nodes[j], st.nodes[i]->counts + st.nodes[j]->counts, true);
}

#endif /* merge_h */
