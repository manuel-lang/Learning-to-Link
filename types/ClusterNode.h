#ifndef ClusterNode_h
#define ClusterNode_h

#include <vector>

/*!
 * Represents one node in the clustering tree that knows its left and right children and also the counts of now many nodes of each target label are included in its subtree.
 */
class ClusterNode {
public:
    ClusterNode *left;
    ClusterNode *right;
    bool has_children;
    std::vector<int> counts;

    ClusterNode(ClusterNode *left, ClusterNode *right, std::vector<int> counts, bool has_children) : left(left),
                                                                                                     right(right),
                                                                                                     counts(counts),
                                                                                                     has_children(
                                                                                                             has_children) {}
};

#endif /* ClusterNode_hpp */
