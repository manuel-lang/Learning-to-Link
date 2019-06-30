#include "AlphaLinkage.h"

#include "../utils/Clustering.h"

#include <chrono>
#include <iostream>
#include <stack>
#include <vector>

#include "CSVReader.h"
#include "Helpers.h"

#include "AlphaRange.h"
#include "State.h"

#include "../utils/Evaluation.h"
#include "../utils/InitOperations.h"

void AlphaLinkage::single_complete(const std::vector<std::string> &files, const std::string &output_file,
                                   const std::vector<double> &sublabels, int points_per_label, int batch_id,
                                   bool verbose, bool average, bool use_majority) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<AlphaRange> ranges;
    int file_id = 0;
    std::vector<double> cur_labels;
    for (const auto &file : files) {
        if (Helpers::hasEnding(file, ".csv")) {
            cur_labels = sublabels;
            file_id++;

            // read csv file and get labels and feature vectors
            std::vector<std::vector<double> > data = readcsv(file);
            std::cout << "Processing " << file << std::endl;
            std::vector<double> labels;
            std::vector<std::vector<double> > feature_vectors;
            Helpers::load_data(data, labels, feature_vectors, sublabels, points_per_label, batch_id);
            if (cur_labels.empty()) {
                cur_labels = Helpers::getUniqueValues(labels);
            }

            // init operations
            std::vector<SC_State> states;
            SC_State state;
            getinitstate(state, feature_vectors, labels, cur_labels);
            states.push_back(state);

            // calculate all intervals
            std::vector<AlphaRange> res = Clustering::getranges(states, output_file, labels.size(), cur_labels.size(),
                                                                verbose, average, use_majority);
            for (AlphaRange const &r : res) {
                ranges.push_back(r);
            }
        }
    }

    // average if wanted
    if (average) {
        std::vector<AlphaRange> output_costs = Evaluation::average_costs(ranges, file_id);
        std::sort(output_costs.begin(), output_costs.end(), Helpers::compareByAlphaMin);
        if (!output_file.empty()) {
            std::ofstream stream;
            stream.open(output_file);
            for (AlphaRange const &range : output_costs) {
                stream << range.min << "," << range.max << "," << range.cost << "\n";
            }
            stream.close();
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Finished after " << elapsed.count() << " seconds.\n";
}

void AlphaLinkage::single_average(const std::vector<std::string> &files, const std::string &output_file,
                                  const std::vector<double> &sublabels, int points_per_label, int batch_id,
                                  bool verbose, bool average, bool use_majority) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<AlphaRange> ranges;
    std::vector<double> cur_labels;
    int file_id = 0;
    for (const auto &file : files) {
        if (Helpers::hasEnding(file, ".csv")) {
            cur_labels = sublabels;
            file_id++;
            // read csv file and get labels and feature vectors
            std::vector<std::vector<double> > data = readcsv(file);
            std::cout << "Processing " << file << std::endl;
            std::vector<double> labels;
            std::vector<std::vector<double> > feature_vectors;
            Helpers::load_data(data, labels, feature_vectors, sublabels, points_per_label, batch_id);
            if (cur_labels.empty()) {
                cur_labels = Helpers::getUniqueValues(labels);
            }

            // init operations
            std::vector<SA_State> states;
            SA_State state;
            getinitstate(state, feature_vectors, labels, cur_labels);
            states.push_back(state);

            // calculate all intervals
            std::vector<AlphaRange> res = Clustering::getranges(states, output_file, labels.size(), cur_labels.size(),
                                                                verbose, average, use_majority);
            for (AlphaRange const &r : res) {
                ranges.push_back(r);
            }
        }
    }

    // average if wanted
    if (average) {
        std::vector<AlphaRange> output_costs = Evaluation::average_costs(ranges, file_id);
        std::sort(output_costs.begin(), output_costs.end(), Helpers::compareByAlphaMin);
        if (!output_file.empty()) {
            std::ofstream stream;
            stream.open(output_file);
            for (AlphaRange const &range : output_costs) {
                stream << range.min << "," << range.max << "," << range.cost << "\n";
            }
            stream.close();
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Finished after " << elapsed.count() << " seconds.\n";;
}

void AlphaLinkage::average_complete(const std::vector<std::string> &files, const std::string &output_file,
                                    const std::vector<double> &sublabels, int points_per_label, int batch_id,
                                    bool verbose, bool average, bool use_majority) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<AlphaRange> ranges;
    std::vector<double> cur_labels;
    int file_id = 0;
    for (const auto &file : files) {
        if (Helpers::hasEnding(file, ".csv")) {
            cur_labels = sublabels;
            file_id++;
            // read csv file and get labels and feature vectors
            std::vector<std::vector<double> > data = readcsv(file);
            std::cout << "Processing " << file << std::endl;
            std::vector<double> labels;
            std::vector<std::vector<double> > feature_vectors;
            Helpers::load_data(data, labels, feature_vectors, sublabels, points_per_label, batch_id);
            if (sublabels.empty()) {
                cur_labels = Helpers::getUniqueValues(labels);
            }

            // init operations
            std::vector<AC_State> states;
            AC_State state;
            getinitstate(state, feature_vectors, labels, cur_labels);
            states.push_back(state);

            // calculate all intervals
            std::vector<AlphaRange> res = Clustering::getranges(states, output_file, labels.size(), cur_labels.size(),
                                                                verbose, average, use_majority);
            for (AlphaRange const &r : res) {
                ranges.push_back(r);
            }
        }
    }

    // average if wanted
    if (average) {
        std::vector<AlphaRange> output_costs = Evaluation::average_costs(ranges, file_id);
        std::sort(output_costs.begin(), output_costs.end(), Helpers::compareByAlphaMin);
        if (!output_file.empty()) {
            std::ofstream stream;
            stream.open(output_file);
            for (AlphaRange const &range : output_costs) {
                stream << range.min << "," << range.max << "," << range.cost << "\n";
            }
            stream.close();
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Finished after " << elapsed.count() << " seconds.\n";
}

void AlphaLinkage::single_complete_folder(const std::string &input_folder, const std::string &output_file,
                                          const std::vector<double> &sublabels, int points_per_label, int batch_id,
                                          bool verbose, bool average, bool use_majority) {
    std::vector<std::string> files = Helpers::get_files_in_folder(input_folder);
    single_complete(files, output_file, sublabels, points_per_label, batch_id, verbose, average, use_majority);
}

void AlphaLinkage::single_average_folder(const std::string &input_folder, const std::string &output_file,
                                         const std::vector<double> &sublabels, int points_per_label, int batch_id,
                                         bool verbose, bool average, bool use_majority) {
    std::vector<std::string> files = Helpers::get_files_in_folder(input_folder);
    single_average(files, output_file, sublabels, points_per_label, batch_id, verbose, average, use_majority);
}

void AlphaLinkage::average_complete_folder(const std::string &input_folder, const std::string &output_file,
                                           const std::vector<double> &sublabels, int points_per_label, int batch_id,
                                           bool verbose, bool average, bool use_majority) {
    std::vector<std::string> files = Helpers::get_files_in_folder(input_folder);
    average_complete(files, output_file, sublabels, points_per_label, batch_id, verbose, average, use_majority);
}
