#ifndef Intersection_h
#define Intersection_h

#include "LinearFunction.h"
#include "MergeCandidate.h"

/*!
 * Decribes an interesetion point between two linear merge function
 */
class Intersection {
public:
    double alpha;
    LinearFunction next_function;
    MergeCandidate next_merge;

    Intersection() {}

    Intersection(double alpha, LinearFunction next_function, MergeCandidate next_merge) : alpha(alpha),
                                                                                          next_function(next_function),
                                                                                          next_merge(next_merge) {}
};

#endif /* Intersection_h */
