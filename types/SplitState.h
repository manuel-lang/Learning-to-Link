#ifndef SplitState_h
#define SplitState_h

#include <stdio.h>
#include <vector>

#include "MergeCandidate.h"

/*!
 * Represents an interval from alpha_min to alpha_max where a certain merge candidate is preferred
 */
class SplitState {
public:
    MergeCandidate merge_candidate;
    double alpha_min;
    double alpha_max;

    SplitState(MergeCandidate merge_candidate, double alpha_min, double alpha_max) : merge_candidate(merge_candidate),
                                                                                     alpha_min(alpha_min),
                                                                                     alpha_max(alpha_max) {}
};

#endif /* SplitState_h */

