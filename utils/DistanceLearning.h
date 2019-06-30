#include <utility>

/**
 * DistanceLearning.h
 */

#ifndef DISTANCE_LEARNING_H
#define DISTANCE_LEARNING_H

#include "../types/AlphaRange.h"
#include "../types/ClusterNode.h"
#include "../data_reader/CSVReader.h"

#include <functional>
#include <vector>

namespace distance_learning {

// Type alias for distance matrices
    typedef std::vector<std::vector<double>> DistanceMatrix;

// Represents a node in the cluster tree. The label of each leaf node is the
// index of the data point it represents. The label of internal nodes is -1.
//
// I didn't use the type defined in types/ClusterNode.h since I only wanted to
// store the index of the point at each leaf of the cluster.
    class ClusterTree {
    public:
        ClusterTree *left;
        ClusterTree *right;
        bool is_leaf;
        // The label for this node. For leaves this is the index of the data point
        // and for internal nodes it is a hash of the two subtrees.
        size_t label;

        // Returns the vector of point indices contained at the leaves of this tree.
        std::vector<size_t> get_points() const;

        // Returns a ClusterNode pointer that includes a count of how many times each
        // target cluster appears in each subtree. This is used by the rest of the
        // code for finding the best prunings and evaluating errors.
        ClusterNode *convert_to_cluster_node(int k,
                                             std::vector<int> target_clusters) const;

        // Constructs a leaf of the cluster tree
        ClusterTree(size_t label);

        // Constructs an internal node of the cluster tree
        ClusterTree(ClusterTree *left, ClusterTree *right);
    };

// Represents what type of linkage we are running.
    enum LinkageType {
        SINGLE_LINKAGE, COMPLETE_LINKAGE
    };

// Represents a pair of point indices
    struct PointPair {
        size_t a;
        size_t b;
    };

// Represents a pair of point indices together with the distance between them
    struct PointsWithDistance {
        PointPair points;
        double distance;
    };

// Represents a cluster merge. Also stores the pair of points defining the
// distance between the merged clusters (i.e., the closest pair for single
// linkage, the farthest pair for complete linkage).
    struct ClusterMerge {
        size_t cluster1;
        size_t cluster2;
        PointsWithDistance points;
    };

// Represents the state of either single or complete linkage while running on a
// dataset of points in the distance function learning setting.
    class DistanceLearningState {
    public:
        // Whether to use single or complete linkage.
        LinkageType linkage_type;
        // Pointers to the two distance matrices
        DistanceMatrix *d0;
        DistanceMatrix *d1;
        // The current set of clusters, each represented by a cluster tree.
        std::vector<ClusterTree *> clusters;

        // The number of clusters in the current state
        size_t num_clusters() const { return clusters.size(); }

        // The number of points in the clustering instance
        size_t num_points() const { return d0->size(); }

        // Calculates the beta distance between points a and b
        double beta_dist(double beta, const size_t a, const size_t b) const {
            return (1 - beta) * (*d0)[a][b] + beta * (*d1)[a][b];
        }

        double beta_dist(double beta, const PointPair &pair) const {
            return this->beta_dist(beta, pair.a, pair.b);
        }

        // Returns a vector of the point indices that belong to cluster_ix.
        std::vector<size_t> get_points(size_t cluster_ix) const {
            return clusters[cluster_ix]->get_points();
        }

        // Returns a new DistanceLearningState obtained from the current one by
        // performing the given merge.
        DistanceLearningState do_merge(ClusterMerge m) const {
            std::vector<ClusterTree *> new_clusters;
            // Copy all old clusters except for the two that were merged.
            for (size_t i = 0; i < num_clusters(); i++) {
                if (i != m.cluster1 && i != m.cluster2) {
                    new_clusters.push_back(clusters[i]);
                }
            }
            // Add the new merged cluster
            ClusterTree *new_node =
                    new ClusterTree(clusters[m.cluster1], clusters[m.cluster2]);
            new_clusters.push_back(new_node);
            return DistanceLearningState(d0, d1, linkage_type, new_clusters);
        }

        DistanceLearningState(DistanceMatrix *d0, DistanceMatrix *d1,
                              LinkageType type)
                : linkage_type(type), d0(d0), d1(d1) {
            for (size_t i = 0; i < num_points(); i++) {
                clusters.push_back(new ClusterTree(i));
            }
        }

        DistanceLearningState(DistanceMatrix *d0, DistanceMatrix *d1,
                              LinkageType type, std::vector<ClusterTree *> clusters)
                : linkage_type(type), d0(d0), d1(d1), clusters(std::move(clusters)) {}
    };

// Represents a cluster tree together with the parameter interval where that
// tree is constructed.
    struct TreeWithInterval {
        ClusterTree *tree;
        double lb;
        double ub;
    };

// Enumerates all possible cluster trees that can be achieved by the given
// linkage type for any convex combination of the distance metrics d0 and d1.
// For each cluster tree, it calls the provided handler function with a
// TreeWithInterval instance with the cluster tree together with a parameter
// interval where that tree is produced.
    void exhaustive_distance_learn(LinkageType linkage_type, DistanceMatrix &d0,
                                   DistanceMatrix &d1,
                                   std::function<void(TreeWithInterval)> handler);

// The same as exhaustive_distance_learn except returns a vector of all
// possible outcomes.
    std::vector<TreeWithInterval>
    collect_exhaustive_distance_learn(LinkageType linkage_type, DistanceMatrix &d0,
                                      DistanceMatrix &d1);

// Runs single or complete linkage using the d_beta distance metric.
    ClusterTree *cluster_data(LinkageType linkage_type, double beta,
                              DistanceMatrix &d0, DistanceMatrix &d1);

} // namespace distance_learning

#endif
