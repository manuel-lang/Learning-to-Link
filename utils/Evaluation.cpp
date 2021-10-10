#include "Evaluation.h"

#include <dirent.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <limits>

#include "stddef.h"

#include "Helpers.h"

/*!
 * Combine a vector of AlphaRanges (that can be unstructured and include the same ranges multiple times) to unique
 * ranges with averaged costs.
 */
std::vector<AlphaRange> Evaluation::average_costs(std::vector<AlphaRange> const &costs, double amount) {
    std::vector<AlphaRange> out_costs;
    out_costs.emplace_back(0.0, 1.0, 0.0);
    for (AlphaRange in : costs) {
        in.cost /= amount;
        std::vector<AlphaRange> temp(out_costs);
        for (size_t i = 0; i < out_costs.size(); i++) {
            auto out = out_costs[i];
            if (in.max <= out.min || in.min >= out.max) continue;
            else if (in.min <= out.min) {
                if (in.max >= out.max) {
                    temp[i] = AlphaRange(out.min, out.max, out.cost + in.cost);
                } else if (in.max < out.max) {
                    temp.emplace_back(in.max, out.max, out.cost);
                    temp[i] = AlphaRange(out.min, in.max, out.cost + in.cost);
                }
            } else if (in.min > out.min) {
                if (in.max >= out.max) {
                    temp.emplace_back(out.min, in.min, out.cost);
                    temp[i] = AlphaRange(in.min, out.max, out.cost + in.cost);
                } else if (in.max < out.max) {
                    temp.emplace_back(out.min, in.min, out.cost);
                    temp.emplace_back(in.max, out.max, out.cost);
                    temp[i] = AlphaRange(in.min, in.max, out.cost + in.cost);
                }
            }
        }
        out_costs = temp;
    }
    return out_costs;
}

/*!
 * Combines all subsequent ranges that have the same cost and outputs the resulting ranges.
 */
std::vector<AlphaRange> Evaluation::compress_regions(std::vector<AlphaRange> const &ranges) {
    AlphaRange cur_range = ranges[0];
    std::vector<AlphaRange> out_ranges;
    for (auto i = 1; i < ranges.size(); i++) {
        if (ranges[i].cost == cur_range.cost) {
            cur_range = AlphaRange(cur_range.min, ranges[i].max, cur_range.cost);
        } else {
            out_ranges.push_back(cur_range);
            cur_range = ranges[i];
        }
    }
    // Push the last range as it is not pushed yet
    out_ranges.push_back(cur_range);
    return out_ranges;
}

/*!
 *  Evaluate all experiment results in one folder with the given ending and stores the result that is averaged over all
 *  files in the given output path
 */
void Evaluation::evaluate_folder(std::string input_folder, std::string output_file, std::string ending) {
    DIR *dpdf;
    struct dirent *epdf;
    dpdf = opendir(input_folder.c_str());
    std::vector<AlphaRange> ranges;

    // used for normalization
    int files_count = 0;

    if (dpdf != nullptr) {
        while ((epdf = readdir(dpdf))) {
            if (Helpers::hasEnding(epdf->d_name, ending)) {
                std::vector<AlphaRange> temp;
                std::ifstream infile(input_folder + '/' + epdf->d_name);
                double a_min, a_max, cost;
                char sep;
                while ((infile >> a_min >> sep >> a_max >> sep >> cost) && (sep == ',')) {
                    temp.emplace_back(a_min, a_max, cost);
                }
                std::sort(temp.begin(), temp.end(), Helpers::compareByAlphaMin);
                std::cout << epdf->d_name << std::endl;
                std::vector<AlphaRange> comp = compress_regions(temp);
                for (AlphaRange const &r : comp) {
                    ranges.push_back(r);
                }
                files_count++;
            }
        }
    }
    std::sort(ranges.begin(), ranges.end(), Helpers::compareByAlphaMin);
    std::vector<AlphaRange> averaged = average_costs(ranges, files_count);
    std::sort(averaged.begin(), averaged.end(), Helpers::compareByAlphaMin);
    std::ofstream file;
    file.open(output_file);
    for (AlphaRange const &out : averaged) {
        file << out.min << "," << out.max << "," << out.cost << "\n";
    }
}

/*!
 * Find the cost for a given value of alpha in a given list of ranges. Returns if a valid range was found.
 */
bool Evaluation::find_cost_in_ranges(double alpha, std::vector<AlphaRange> ranges, double &ret_cost) {
    for (AlphaRange const &r : ranges) {
        if (r.min <= alpha && r.max > alpha) {
            ret_cost = r.cost;
            return true;
        }
    }
    return false;
}

/*!
 * Get all ranges of a given input file.
 */
std::vector<AlphaRange> Evaluation::get_ranges(std::string input_folder, std::string file_path) {
    std::vector<AlphaRange> ranges;
    std::ifstream infile(file_path);
    double a_min, a_max, cost;
    char sep;
    while ((infile >> a_min >> sep >> a_max >> sep >> cost) && (sep == ',')) {
        ranges.emplace_back(a_min, a_max, cost);
    }
    return ranges;
}

/*!
 * Get the optimal values for all experimental results stored in the given file_paths.
 */
std::vector<AlphaRange> Evaluation::get_opts(std::string input_folder, std::vector<std::string> file_paths) {
    std::vector<AlphaRange> opts;
    for (const std::string &path : file_paths) {
        if (Helpers::hasEnding(path, ".txt")) {
            AlphaRange opt = AlphaRange(0, 0, std::numeric_limits<double>::infinity());
            std::ifstream infile(path);
            double a_min, a_max, cost;
            char sep;
            while ((infile >> a_min >> sep >> a_max >> sep >> cost) && (sep == ',')) {
                if (cost < opt.cost) {
                    opt = AlphaRange(a_min, a_max, cost);
                }
            }
            opts.push_back(opt);
        }
    }
    return opts;
}

/*!
 * Get the top k optimal values for experimental results where each value must be the optimal value for at least one
 * experiment. This algorithm is based on the assumption that the optimal values for k are all also part of the optimal
 * values for k+1.
 */
void Evaluation::greedy_top_k(const std::string &input_folder, int max_k, const std::string &output_file) {
    std::vector<std::string> file_paths = Helpers::get_files_in_folder(input_folder);
    std::vector<AlphaRange> opt_vals = get_opts(input_folder, file_paths);
    std::vector<std::vector<AlphaRange> > all_ranges;
    std::vector<std::pair<double, double> > top_k;

    // read compressed regions for all experiments
    for (const std::string &path : file_paths) {
        if (Helpers::hasEnding(path, ".txt")) {
            all_ranges.push_back(compress_regions(get_ranges(input_folder, path)));
        }
    }

    // find best parameters
    for (unsigned int k = 1; k <= max_k; k++) {
        std::pair<double, double> top = std::pair<double, double>(0.0, std::numeric_limits<double>::infinity());
        for (const AlphaRange &opt : opt_vals) {
            double overall_cost = 0.0;
            for (const auto &experiment_result : all_ranges) {
                // sum up costs for all experiments given AlphaRange opt and previously calculated optimal parameters
                double cost = 0.0;
                bool check = find_cost_in_ranges((opt.min + opt.max) / 2.0, experiment_result, cost);
                for (const auto &top_range : top_k) {
                    double cost_temp = 0.0;
                    find_cost_in_ranges(std::get<0>(top_range), experiment_result, cost_temp);
                    cost = std::min(cost, cost_temp);
                }
                overall_cost += cost;
            }

            // normalize cost and compare to optimum
            overall_cost /= (double) all_ranges.size();
            if (overall_cost < std::get<1>(top)) {
                top = std::pair<double, double>((opt.min + opt.max) / 2.0, overall_cost);
            }
        }
        top_k.push_back(top);
    }

    // export results "<alpha>,<cost>"
    std::ofstream myfile;
    myfile.open(output_file);
    for (auto const &out : top_k) {
        myfile << std::get<0>(out) << "," << std::get<1>(out) << "\n";
    }
}

void Evaluation::write_compressed_files(std::string input_folder, std::string output_folder) {
    std::vector<std::string> file_paths = Helpers::get_files_in_folder(input_folder);
    for (const std::string &path : file_paths) {
        std::vector<AlphaRange> ranges;
        std::ifstream infile(input_folder + '/' + path);
        double a_min, a_max, cost;
        char sep;
        while ((infile >> a_min >> sep >> a_max >> sep >> cost) && (sep == ',')) {
            ranges.emplace_back(a_min, a_max, cost);
        }
        std::sort(ranges.begin(), ranges.end(), Helpers::compareByAlphaMin);
        std::vector<AlphaRange> comp = compress_regions(ranges);
        std::ofstream myfile;
        myfile.open(output_folder + '/' + path);
        for (AlphaRange const &out : comp) {
            myfile << out.min << "," << out.max << "," << out.cost << "\n";
        }
    }
}
