#include "./utils/AlphaLinkage.h"
#include "./utils/Evaluation.h"
#include "./utils/Helpers.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static void show_usage(std::string name) {
    std::cerr << "Usage: " << name << " <option(s)> SOURCES"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << "\t-e,--experiment \t\tSpecify the folder path\n"
              << "\t-f,--folder \t\tSpecify the folder path\n"
              << "\t-i,--input \t\tSpecify the files path\n"
              << "\t-l,--labels \t\tSpecify the specific labels as CSV input, e.g. 0,5,9\n"
              << "\t-p,--points \t\tSpecify how many points of each class are used (will result in num_classes * points_per_class points overall)\n"
              << "\t-v,--verbose \t\tShow entire logs"
              << std::endl;
}

std::vector<double> split_labels(const std::string &s, char delimiter) {
    std::vector<double> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(std::stod(token));
    }
    return tokens;
}

int main(int argc, char *argv[]) {
    Evaluation::evaluate_folder("/Users/Manuel/CMU/AlphaLinkage/AlphaLinkage/logs/MNIST-AWS-AC-6-batches", "/Users/Manuel/CMU/AlphaLinkage/AlphaLinkage/logs/MNIST-AWS-AC-6-batches/avg.txt");
    return 0;
    bool average = true;
    bool use_folder = false;
    bool use_files = false;
    int points_per_class = 0;
    int batch_id = 0;
    bool use_majority = false;
    std::string mode = "AC";
    std::string folder;
    std::string output;
    std::vector<std::string> files = {};
    std::vector<double> labels = {};
    bool verbose = false;
    double alpha = -1;

    // requires at least an input and an input
    if (argc < 3) {
        show_usage(argv[0]);
        return 0;
    }

    // parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // help
        if (arg == "-h" || arg == "--help") {
            show_usage(argv[0]);
            return 0;
        }

        // batch id
        else if (arg == "-b" || arg == "--batch") {
            if (i + 1 < argc) {
                i++;
                batch_id = std::stoi(argv[i]);
            } else {
                std::cerr << "--batch option requires one argument." << std::endl;
                return 0;
            }
        }

        // input folder
        else if (arg == "-f" || arg == "--folder") {
            use_folder = true;
            if (i + 1 < argc) {
                i++;
                folder = argv[i];
            } else {
                std::cerr << "--folder option requires one argument." << std::endl;
                return 0;
            }
        }

        // input file
        else if (arg == "-i" || arg == "--input") {
            use_files = true;
            if (i + 1 < argc) {
                i++;
                files.emplace_back(argv[i]);
            } else {
                std::cerr << "--input option requires one argument." << std::endl;
                return 0;
            }
        }

        // job index
        else if (arg == "-j" || arg == "--job") {
            if (i + 1 < argc) {
                i++;
                labels = Helpers::mnist_id_to_labels(std::stoi(argv[i]));
            } else {
                std::cerr << "--job option requires one argument." << std::endl;
                return 0;
            }
        }

        // csv list of sublabels
        else if (arg == "-l" || arg == "--labels") {
            if (i + 1 < argc) {
                i++;
                labels = split_labels(argv[i], ',');
            } else {
                std::cerr << "--labels option requires one argument." << std::endl;
                return 0;
            }
        }

        // use majority distance
        else if (arg == "-m" || arg == "--majority") {
            use_majority = true;
        }

        // skip averaging
        else if (arg == "-n" || arg == "--noaverage") {
            average = false;
        }

        // output file
        else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                i++;
                output = argv[i];
            } else {
                std::cerr << "--output option requires one argument." << std::endl;
                return 0;
            }
        }

        // points per class
        else if (arg == "-p" || arg == "--points") {
            if (i + 1 < argc) {
                i++;
                points_per_class = std::stoi(argv[i]);
            } else {
                std::cerr << "--points option requires one argument." << std::endl;
                return 0;
            }
        }

        // interpolate between average and complete linkage
        else if (arg == "-ac" || arg == "--averagecomplete") {
            mode = "AC";
        }

        // interpolate between single and average linkage
        else if (arg == "-sa" || arg == "--singleaverage") {
            mode = "SA";
        }

        // interpolate between single and complete linkagee
        else if (arg == "-sc" || arg == "--singlecomplete") {
            mode = "SC";
        }

        // verbose output
        else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        }
    }

    // launch experiments for entire directories
    if (use_folder) {
        if (mode == "AC") {
            AlphaLinkage::average_complete_folder(folder, output, labels, points_per_class, batch_id, verbose, average,
                                                  use_majority);
        } else if (mode == "SA") {
            AlphaLinkage::single_average_folder(folder, output, labels, points_per_class, batch_id, verbose, average,
                                                use_majority);
        } else if (mode == "SC") {
            AlphaLinkage::single_complete_folder(folder, output, labels, points_per_class, batch_id, verbose, average,
                                                 use_majority);
        }

    }

    // launch experiments for individual files
    else if (use_files) {
        if (files.size() == 1) {
            average = false;
        }
        if (mode == "AC") {
            AlphaLinkage::average_complete(files, output, labels, points_per_class, batch_id, verbose, average,
                                           use_majority);
        } else if (mode == "SA") {
            AlphaLinkage::single_average(files, output, labels, points_per_class, batch_id, verbose, average,
                                         use_majority);
        } else if (mode == "SC") {
            AlphaLinkage::single_complete(files, output, labels, points_per_class, batch_id, verbose, average,
                                          use_majority);
        }
    }
    return 0;
}
