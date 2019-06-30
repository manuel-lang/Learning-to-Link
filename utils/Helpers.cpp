//
// Created by Manuel Lang on 12/18/18.
//

#include "Helpers.h"

/*!
 * Check if the minimum alpha value if a range is smaller than the one of another range in order to sort AlphaRanges.
 */
bool Helpers::compareByAlphaMin(AlphaRange const &a, AlphaRange const &b) {
    return a.min < b.min;
}

/*!
 * Check if a string has a specific ending.
 */
bool Helpers::hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

long Helpers::get_reduced_matrix_index(unsigned long width, int i, int j) {
    return (width * (width - 1)) / 2 - ((width - i) * (width - i - 1)) / 2 + j - i - 1;
}

long Helpers::get_reduced_matrix_outter_index(unsigned long width, long i) {
    return (width * (width - 1)) / 2 - ((width - i) * (width - i - 1)) / 2 - i - 1;
}

void Helpers::load_data(std::vector<std::vector<double> > const &data, std::vector<double> &labels,
                        std::vector<std::vector<double> > &feature_vectors, const std::vector<double> &sublabels,
                        int points_per_label, int batch_id) {
    std::vector<unsigned int> label_counts;
    if (!sublabels.empty()) {
        std::vector<unsigned int> counts(sublabels.size());
        label_counts = counts;
    }

    std::vector<double> distinct_labels;
    for (auto i = 0; i < data.size(); i++) {
        if (sublabels.empty()) // if no specific labels are defined
        {
            if (points_per_label == 0) // if no specific labels are defined and all points for each label are wanted
            {
                labels.push_back(data[i].front());
                std::vector<double> features(data[i].begin() + 1, data[i].end());
                feature_vectors.push_back(features);
            } else // if no specific labels are defined and only limited points for each label are wanted
            {
                auto pos = std::find(distinct_labels.begin(), distinct_labels.end(), data[i].front()) -
                           distinct_labels.begin();
                if (pos < distinct_labels.size()) // if previous points of the same label already appeared
                {
                    if (label_counts[pos] < points_per_label * (1 + batch_id)) {
                        if (points_per_label * batch_id <= label_counts[pos]) {
                            labels.push_back(data[i].front());
                            std::vector<double> features(data[i].begin() + 1, data[i].end());
                            feature_vectors.push_back(features);
                        }
                        label_counts[pos]++;
                    }
                } else {
                    distinct_labels.push_back(data[i].front());
                    label_counts.push_back(1);
                    labels.push_back(data[i].front());
                    std::vector<double> features(data[i].begin() + 1, data[i].end());
                    feature_vectors.push_back(features);
                }
            }
        } else // if specific labels are defined
        {
            auto pos = std::find(sublabels.begin(), sublabels.end(), data[i].front()) - sublabels.begin();
            if (pos < sublabels.size()) {
                if (points_per_label == 0 || label_counts[pos] < points_per_label * (1 + batch_id)) {
                    if (points_per_label == 0 || label_counts[pos] >= points_per_label * batch_id) {
                        labels.push_back(data[i].front());
                        std::vector<double> features(data[i].begin() + 1, data[i].end());
                        feature_vectors.push_back(features);
                    }
                    label_counts[pos]++;
                }
            }
        }
    }
}

/*!
 * Gets a list of the names of all files in a given folder with the given ending (e.g. .csv)
 */
std::vector<std::string> Helpers::get_files_in_folder(const std::string &input_folder) {
    DIR *dpdf;
    struct dirent *epdf;
    dpdf = opendir(input_folder.c_str());
    std::vector<AlphaRange> opts;
    std::vector<std::string> files;
    if (dpdf != nullptr) {
        while ((epdf = readdir(dpdf))) {
            files.push_back(input_folder + "/" + epdf->d_name);
        }
    }
    return files;
}

std::vector<double> Helpers::mnist_id_to_labels(int experiment_id) {
    int counter = 0;
    for (auto i = 0; i < 10; i++) {
        for (auto j = i + 1; j < 10; j++) {
            for (auto k = j + 1; k < 10; k++) {
                for (auto l = k + 1; l < 10; l++) {
                    for (auto m = l + 1; m < 10; m++) {
                        if (counter == experiment_id) {
                            return std::vector<double>{static_cast<double>(i), static_cast<double>(j),
                                                       static_cast<double>(k), static_cast<double>(l),
                                                       static_cast<double>(m)};
                        }
                        counter++;
                    }
                }
            }
        }
    }
    return {};
}
