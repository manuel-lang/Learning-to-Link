#ifndef State_h
#define State_h

#include "ClusterNode.h"

/*!
 * A state represents one possible clustering at any given time of the linkage based agglomerative hierarchical clustering algorithm. Each state is valid for a given range of the parameter alpha and thus represented by a lower boundary alpha_min and an upper boundary alpha_max. For each state, we store the distance matrices for both the lower and the upper distances. This dyamic programming approach improves the performance a lot over calculating the distances over and over again. Each state also contains the active_indices, that indicate which clusters were not merged yet. A vector of node represents the underlying cluster structure of a state.
 */
class State {
public:
    double alpha_min;
    double alpha_max;
    std::vector<double> lower_dists;
    std::vector<double> upper_dists;
    std::vector<long> active_indices;
    std::vector<ClusterNode *> nodes;

    State() {};

    State(double amin, double amax, std::vector<double> mind, std::vector<double> maxd, std::vector<long> ai,
          std::vector<ClusterNode *> n) : alpha_min(amin), alpha_max(amax), lower_dists(mind), upper_dists(maxd),
                                          active_indices(ai), nodes(n) {};

    bool operator==(const State &s1) {
        return s1.alpha_min == alpha_min && s1.alpha_max == alpha_max && s1.active_indices == active_indices;
    }
};

/*!
 * A SC_State is a certain subtype of a State that represents a state for interpolating between single and complete linkage.
 */
class SC_State : public State {
public:
    SC_State() {};

    SC_State(double amin, double amax, std::vector<double> mind, std::vector<double> maxd, std::vector<long> ai,
             std::vector<ClusterNode *> n) : State(amin, amax, mind, maxd, ai, n) {};
};

/*!
 * A SC_State is a certain subtype of a State that represents a state for interpolating between single and average linkage.
 */
class SA_State : public State {
public:
    std::vector<short> cluster_sizes;

    SA_State() {};

    SA_State(double amin, double amax, std::vector<double> mind, std::vector<double> avgd, std::vector<long> ai,
             std::vector<ClusterNode *> n, std::vector<short> cs) : State(amin, amax, mind, avgd, ai, n),
                                                                    cluster_sizes(cs) {};
};

/*!
 * A SC_State is a certain subtype of a State that represents a state for interpolating between average and complete linkage.
 */
class AC_State : public State {
public:
    std::vector<short> cluster_sizes;

    AC_State() {};

    AC_State(double amin, double amax, std::vector<double> avgd, std::vector<double> maxd, std::vector<long> ai,
             std::vector<ClusterNode *> n, std::vector<short> cs) : State(amin, amax, avgd, maxd, ai, n),
                                                                    cluster_sizes(cs) {};
};

#endif /* State_h */
