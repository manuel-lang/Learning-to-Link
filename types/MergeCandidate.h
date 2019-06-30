#ifndef MergeCandidate_h
#define MergeCandidate_h

/*!
 * Represends a candiate for a merge given by two cluster indices.
 */
class MergeCandidate {
public:
    long cluster1;
    long cluster2;

    MergeCandidate() {}

    MergeCandidate(long cluster1, long cluster2) : cluster1(cluster1), cluster2(cluster2) {}
};

#endif /* MergeCandidate_h */
