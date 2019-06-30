#ifndef Helpers_h
#define Helpers_h

#include <algorithm>
#include <dirent.h>
#include <functional>
#include <string>
#include <vector>

#include "../types/AlphaRange.h"

namespace Helpers {

    /**
     * Compares the start values of two ranges.
     * @param a - alpha range
     * @param b - alpha range
     * @return a comparator if one range's start value is smaller than another's
     */
    bool compareByAlphaMin(AlphaRange const &a, AlphaRange const &b);

    /**
     * Checks if  a string has a specific ending.
     * @param fullString - input string
     * @param ending - desired ending
     * @return if the string has the desired ending
     */
    bool hasEnding(std::string const &fullString, std::string const &ending);

    /**
     * Calculates the pairwise distance vector index of clusters i and j.
     * @param width - width of distance matrix before it was flattened
     * @param i - index of cluster 1
     * @param j - index of cluster 2
     * @return the index of the distance between clusters i and j in the pairwise distance vector
     */
    long get_reduced_matrix_index(unsigned long width, int i, int j);

    /**
     * Calculates the outer index (i.e. start of pairwise distances containing i) of a matrix in a flattened pairwise
     * distance vector (after removing duplicates and zeroes)
     * @param width - the width of the original pairwise distance matrix (i.e. num of points)
     * @param i - cluster i
     * @return the outter index
     */
    long get_reduced_matrix_outter_index(unsigned long width, long i);

    /**
     * Gets all unique values in a vector.
     * @tparam T - feature type.
     * @param vec - the input vector.
     * @return all unique values in the vector
     */
    template<typename T>
    std::vector<T> getUniqueValues(std::vector<T> vec) {
        std::sort(vec.begin(), vec.end());
        vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
        return vec;
    }

    /**
     * Calculates the used MNIST labels based on an experiment id.
     * @param experiment_id - the ID of the experiment
     * @return the corresponding labels
     */
    std::vector<double> mnist_id_to_labels(int experiment_id);

    /**
     * Loads all feature vectors and labels from a nested data vector depending on given target labels, amount of
     * points per class and the batch id.
     * @param data - all data points
     * @param labels - output labels
     * @param feature_vectors - output feature vectors
     * @param sublabels - all target labels
     * @param points_per_label - amount of points for each class
     * @param batch_id - n-th batch for each class (NOTE: starting at n = 0)
     */
    void load_data(std::vector<std::vector<double>> const &data,
                   std::vector<double> &labels,
                   std::vector<std::vector<double>> &feature_vectors,
                   const std::vector<double> &sublabels, int points_per_label,
                   int batch_id);

    /**
     * Gets all file names in a given folder.
     * @param input_folder - input directory
     * @return all file names
     */
    std::vector<std::string> get_files_in_folder(const std::string &input_folder);
} // namespace Helpers
#endif /* Helpers_h */
