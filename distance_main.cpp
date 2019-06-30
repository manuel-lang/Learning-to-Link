#include "data_reader/CSVReader.h"
#include "types/ClusterNode.h"
#include "utils/DistanceFunction.h"
#include "utils/DistanceLearning.h"
#include "utils/Helpers.h"
#include "utils/Prune.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace distance_learning;

// Represents the error type that we compute
enum ErrorType {
    HAMMING, MAJORITY
};

// Represents what type of data is stored in each file.
enum FileType {
    EUCLIDEAN, COSINE, DISTANCES
};

FileType parse_file_type(char c) {
    switch (c) {
        case 'd':
            return DISTANCES;
        case 'c':
            return COSINE;
        default:
            return EUCLIDEAN;
    }
}

struct LabeledData {
    vector<int> labels;
    vector<vector<double>> points;
};

// Given the output of readcsv, extracts the first column as the label and the
// remaining columns as feature vectors.
LabeledData split_labels(const vector<vector<double>> &data) {
    LabeledData labeled_data;
    int count = 0;
    for (auto row : data) {
        labeled_data.labels.push_back(row[0]);
        labeled_data.points.push_back(vector<double>());
        for (size_t i = 1; i < row.size(); i++) {
            labeled_data.points[labeled_data.points.size() - 1].push_back(row[i]);
        }
    }
    return labeled_data;
}

// Loads a dataset from file and returns it.
LabeledData load_data_from_csv(string path) {
    vector<vector<double>> data = readcsv(path);
    vector<double> labels;
    vector<vector<double>> feature_vectors;
    vector<double> sublabels = {};
    Helpers::load_data(data, labels, feature_vectors, sublabels, 0, 0);
    LabeledData labeledData;
    labeledData.labels = labels;
    labeledData.points = feature_vectors;
    cout << labeledData.points.siz() << endl;
    return labeledData;
}

// Given a vector of labels (arbitrary integers) and a vector of the unique
// labels, maps the labels to take values in 0, ..., unique_labels.size() - 1
void adjust_labels(vector<int> &labels, const vector<int> &unique_labels) {
    for (int &label : labels) {
        for (size_t j = 0; j < unique_labels.size(); j++) {
            if (label == unique_labels[j]) {
                label = j;
                break;
            }
        }
    }
}

// Computes the ell-2 distance matrix for a vector of feature vectors.
DistanceMatrix make_distances(const vector<vector<double>> &features,
                              FileType file_type) {
    int num_pts = features.size();
    DistanceMatrix d;
    for (int i = 0; i < num_pts; i++) {
        vector<double> dists;
        for (int j = 0; j < num_pts; j++) {
            double d;
            switch (file_type) {
                case COSINE:
                    d = DistanceFunction::cosine_dist(features[i], features[j]);
                    break;
                default:
                    d = DistanceFunction::euclidean_dist(features[i], features[j]);
            }
            dists.push_back(d);
        }
        d.push_back(dists);
    }
    return d;
}

// Rescales all distances in the distance matrix d so that the maximum distance
// is 1.
void normalize_distances(DistanceMatrix &d) {
    double max_dist = -1;
    for (const auto &row : d) {
        for (auto value : row) {
            max_dist = max(max_dist, value);
        }
    }
    if (max_dist >= 1e-10) {
        for (auto &i : d) {
            for (double &j : i) {
                j /= max_dist;
            }
        }
    }
}

int main(int argv, char *args[]) {
    // All toggleable parameters are specified in the first argument, each using a
    // single character.
    string param_string(args[1]);
    ErrorType error_type = param_string[0] == 'm' ? MAJORITY : HAMMING;
    LinkageType linkage_type =
            param_string[1] == 's' ? SINGLE_LINKAGE : COMPLETE_LINKAGE;
    FileType file0_type = parse_file_type(param_string[2]);
    FileType file1_type = parse_file_type(param_string[3]);

    // Parse first feature file and load data
    string data0_filename(args[2]);
    LabeledData data0 = load_data_from_csv(data0_filename);
    DistanceMatrix d0;
    if (file0_type == DISTANCES) {
        d0 = data0.points;
    } else {
        d0 = make_distances(data0.points, file0_type);
    }
    normalize_distances(d0);

    // Parse second feature file and load data
    string data1_filename(args[3]);
    LabeledData data1 = load_data_from_csv(data1_filename);
    DistanceMatrix d1;
    if (file1_type == DISTANCES) {
        d1 = data1.points;
    } else {
        d1 = make_distances(data1.points, file1_type);
    }
    normalize_distances(d1);

    vector<int> labels = data0.labels;
    vector<int> unique_labels = Helpers::getUniqueValues(labels);
    adjust_labels(labels, unique_labels);
    int k = unique_labels.size();

    // Print out error for each execution tree possible
    exhaustive_distance_learn(
            linkage_type, d0, d1,
            // Handler that prints the error on each parameter interval.
            [&labels, k, error_type](TreeWithInterval sol) {
                // Convert the cluster tree into an instance of ClusterNode
                ClusterNode *cn = sol.tree->convert_to_cluster_node(k, labels);
                // Calculate the error for the best pruning of this tree
                double error;
                switch (error_type) {
                    case MAJORITY:
                        error = prune(*cn, k)[k].cost / labels.size();
                        break;
                    case HAMMING:
                        error = best_pruning(*cn, k).cost / labels.size();
                        break;
                }
                // Print result to stdout
                cout << sol.lb << ", " << sol.ub << ", " << error << endl;
            });
}
