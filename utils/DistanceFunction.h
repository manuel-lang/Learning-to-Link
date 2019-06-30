//
//  DistanceFunction.h
//  AlphaLinkage
//
//  Created by Manuel Lang on 26.09.18.
//  Copyright © 2018 Manuel Lang. All rights reserved.
//

#ifndef DistanceFunction_h
#define DistanceFunction_h

#include <cmath>
#include <vector>

namespace DistanceFunction {

    /**
     * Computes the euclidean distance (L2 norm) between two vectors.
     * @tparam T - numeric feature type
     * @param a - vector of points
     * @param b - vector of points
     * @return Euclidean distance between two numeric point vectors.
     */
    template<typename T>
    inline double euclidean_dist(std::vector<T> const &a,
                                 std::vector<T> const &b) {
        T d = 0.0;
        for (auto dim = 0; dim < a.size(); dim++) {
            d += pow(fabs(a[dim] - b[dim]), 2);
        }
        return sqrt(d);
    }

    /**
     * Computes the cosine distance (i.e., the angle between) two vectors.
     * @tparam T - numeric feature type
     * @param a - vector of points
     * @param b - vector of points
     * @return Cosine distance between two numeric point vectors
     */
    template<typename T>
    double cosine_dist(const std::vector<T> &a, const std::vector<T> &b) {
        T inner_product = 0;
        T a_norm = 0;
        T b_norm = 0;
        for (size_t i = 0; i < a.size(); i++) {
            inner_product += a[i] * b[i];
            a_norm += a[i] * a[i];
            b_norm += b[i] * b[i];
        }
        a_norm = sqrt(a_norm);
        b_norm = sqrt(b_norm);
        return acos(inner_product / a_norm / b_norm);
    }

    /**
     * Computes the euclidean distance for nested images (e.g. RGB)
     * @tparam T - numeric feature type
     * @param a - nested vector of points
     * @param b - nested vector of points
     * @return Euclidean distance for two nested point vectors
     */
    template<typename T>
    T euclidean_dist_rgb(std::vector<std::vector<T>> const &a,
                         std::vector<std::vector<T>> const &b) {
        T d = 0.0;
        for (auto dim = 0; dim < a.size(); dim++) {
            for (auto channel = 0; channel < 3; channel++) {
                d += pow(a[dim][channel] - b[dim][channel], 2);
            }
        }
        return sqrt(d);
    }

}; // namespace DistanceFunction

#endif /* DistanceFunction_h */
