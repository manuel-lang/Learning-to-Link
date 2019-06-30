#include "DistanceLearning.h"

#include <cmath>
#include <functional>
#include <limits>
#include <utility>

#include <iostream>

using namespace std;

namespace distance_learning {

//////////////////////////////
// ----- Cluster Tree ----- //
//////////////////////////////

void get_tree_points(const ClusterTree *tree, vector<size_t> *points) {
  if (tree->is_leaf) {
    points->push_back(tree->label);
  } else {
    get_tree_points(tree->left, points);
    get_tree_points(tree->right, points);
  }
}

vector<size_t> ClusterTree::get_points() const {
  vector<size_t> points;
  get_tree_points(this, &points);
  return points;
}

ClusterNode *
ClusterTree::convert_to_cluster_node(int k,
                                     std::vector<int> target_clusters) const {
  if (is_leaf) {
    ClusterNode *node = new ClusterNode(nullptr, nullptr, vector<int>(), false);
    for (int i = 0; i < k; i++)
      node->counts.push_back(i == target_clusters[label] ? 1 : 0);
    return node;
  } else {
    ClusterNode *left_cn = left->convert_to_cluster_node(k, target_clusters);
    ClusterNode *right_cn = right->convert_to_cluster_node(k, target_clusters);
    ClusterNode *node = new ClusterNode(left_cn, right_cn, vector<int>(), true);
    for (int i = 0; i < k; i++)
      node->counts.push_back(left_cn->counts[i] + right_cn->counts[i]);
    return node;
  }
}

ClusterTree::ClusterTree(size_t label)
    : left(nullptr), right(nullptr), is_leaf(true), label(label) {}

ClusterTree::ClusterTree(ClusterTree *left, ClusterTree *right)
    : left(left), right(right), is_leaf(false), label(-1) {}

//////////////////////////////////////////////////
// ----- Exhaustive Search Implementation ----- //
//////////////////////////////////////////////////

// Given two distance matrices d0 and d1, a mixing parameter beta, and two sets
// A and B of point indices, returns the closest pair of points between the sets
// A and B according to the d_beta metric. The closest pair of points and the
// distance between them are returned through the last 3 arguments.
PointsWithDistance find_closest_points(const DistanceLearningState &dls,
                                       double beta,
                                       vector<size_t> &cluster1_pts,
                                       vector<size_t> &cluster2_pts) {
  PointPair best_pair;
  double best_dist = numeric_limits<double>::infinity();
  for (auto a : cluster1_pts) {
    for (auto b : cluster2_pts) {
      double dist = dls.beta_dist(beta, a, b);
      if (dist < best_dist) {
        best_pair.a = a;
        best_pair.b = b;
        best_dist = dist;
      }
    }
  }
  return PointsWithDistance{best_pair, best_dist};
}

// The same as find_closest_points, except instead it returns the most distant
// pair of points between A and B according to the d_beta metric.
PointsWithDistance find_farthest_points(const DistanceLearningState &dls,
                                        double beta,
                                        vector<size_t> &cluster1_pts,
                                        vector<size_t> &cluster2_pts) {
  PointPair best_pair;
  double best_dist = -1;
  for (auto a : cluster1_pts) {
    for (auto b : cluster2_pts) {
      double dist = dls.beta_dist(beta, a, b);
      if (dist > best_dist) {
        best_pair.a = a;
        best_pair.b = b;
        best_dist = dist;
      }
    }
  }
  return PointsWithDistance{best_pair, best_dist};
}

// Finds the pair of clusters that would be merged from the given
// DistanceLearningState for the parameter value beta. Also returns the pair of
// points that determined the distance between the merged clusters (the closest
// pair of points for single linkage and the farthest pair for complete
// linkage).
ClusterMerge find_merge(const DistanceLearningState &dls, double beta) {
  size_t best_c1, best_c2;
  PointsWithDistance best_points{0, 0, numeric_limits<double>::infinity()};
  for (size_t c1 = 0; c1 < dls.num_clusters(); c1++) {
    vector<size_t> c1_pts = dls.get_points(c1);
    for (size_t c2 = c1 + 1; c2 < dls.num_clusters(); c2++) {
      vector<size_t> c2_pts = dls.get_points(c2);

      // Find either the closest or farthest pair of points between c1 and c2
      // depending on the linkage type.
      PointsWithDistance points;
      switch (dls.linkage_type) {
      case SINGLE_LINKAGE:
        points = find_closest_points(dls, beta, c1_pts, c2_pts);
        break;
      case COMPLETE_LINKAGE:
        points = find_farthest_points(dls, beta, c1_pts, c2_pts);
        break;
      }

      // If the pair of points are closer than the best pair of clusters so far
      // then update.
      if (points.distance < best_points.distance) {
        best_c1 = c1;
        best_c2 = c2;
        best_points = points;
      }
    }
  }
  return ClusterMerge{best_c1, best_c2, best_points};
}

// Represents the solution to a linear equation of one variable. We do not
// distinguish between zero solutions and infinitely many solutions, since these
// two cases don't matter for our setting.
struct LinearSolution {
  bool unique;
  double value;
};

// Returns the solution to the linear equation d_beta(p1) = d_beta(p2) as a
// function of beta.
LinearSolution find_critical_parameter(const DistanceLearningState &dis,
                                       const PointPair &p1,
                                       const PointPair &p2) {
  double delta0 = dis.beta_dist(0, p2) - dis.beta_dist(0, p1);
  double delta1 = dis.beta_dist(1, p2) - dis.beta_dist(1, p1);
  if (fabs(delta0 - delta1) < 1e-10) {
    return LinearSolution{false, 0};
  } else {
    return LinearSolution{true, delta0 / (delta0 - delta1)};
  }
}

// Returns the largest parameter value beta_bound such the pair of points
// certifying the merge chosen by the algorithm is the same for all parameters
// beta' in [beta, beta_bound).
double find_beta_bound(const DistanceLearningState &dis, double beta,
                       const ClusterMerge &merge) {
  PointPair merge_pair = merge.points.points;
  double current_bound = 1.0;
  for (size_t c1 = 0; c1 < dis.num_clusters(); c1++) {
    vector<size_t> c1_pts = dis.get_points(c1);
    for (size_t c2 = c1 + 1; c2 < dis.num_clusters(); c2++) {
      vector<size_t> c2_pts = dis.get_points(c2);
      for (auto p1 : c1_pts) {
        for (auto p2 : c2_pts) {
          LinearSolution s =
              find_critical_parameter(dis, merge_pair, PointPair{p1, p2});
          if (s.unique && s.value > beta && s.value < current_bound) {
            current_bound = s.value;
          }
        }
      }
    }
  }
  return current_bound;
}

void exhaustive_distance_learn(const DistanceLearningState &dls,
                               double min_beta, double max_beta,
                               const function<void(TreeWithInterval)> &handler) {
  // base case: only one cluster, the algorithm has finished
  if (dls.num_clusters() == 1) {
    handler(TreeWithInterval{dls.clusters[0], min_beta, max_beta});
  }
  // recursive case: find each merge and recurse on them from left to right.
  else {
    double beta = min_beta;
    ClusterMerge merge = find_merge(dls, beta);
    while (beta < max_beta) {

      // Find the largest value of beta so that the same merge is performed in
      // [beta, next_beta]
      ClusterMerge next_merge = merge;
      double next_beta = beta;
      while (next_beta < max_beta && next_merge.cluster1 == merge.cluster1 &&
             next_merge.cluster2 == merge.cluster2) {
        next_beta =
            min(max_beta, find_beta_bound(dls, next_beta, next_merge) + 1e-10);
        next_merge = find_merge(dls, next_beta);
      }

      DistanceLearningState child_dls = dls.do_merge(merge);
      exhaustive_distance_learn(child_dls, beta, next_beta, handler);
      beta = next_beta;
      merge = next_merge;
    }
  }
}

void exhaustive_distance_learn(LinkageType linkage_type, DistanceMatrix &d0,
                               DistanceMatrix &d1,
                               function<void(TreeWithInterval)> handler) {
  DistanceLearningState state(&d0, &d1, linkage_type);
  exhaustive_distance_learn(state, 0, 1, handler);
}

std::vector<TreeWithInterval>
collect_exhaustive_distance_learn(LinkageType linkage_type, DistanceMatrix &d0,
                                  DistanceMatrix &d1) {
  vector<TreeWithInterval> solutions;
  exhaustive_distance_learn(
      linkage_type, d0, d1,
      [&solutions](TreeWithInterval t) { solutions.push_back(t); });
  return solutions;
}

ClusterTree *cluster_data(LinkageType linkage_type, double beta,
                          DistanceMatrix &d0, DistanceMatrix &d1) {
  DistanceLearningState state(&d0, &d1, linkage_type);
  while (state.num_clusters() > 1) {
    ClusterMerge m = find_merge(state, beta);
    state = move(state.do_merge(m));
  }
  return state.clusters[0];
}

} // namespace distance_learning
