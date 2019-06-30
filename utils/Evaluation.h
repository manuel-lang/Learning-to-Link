#ifndef Evaluation_h
#define Evaluation_h

#include <string>
#include <tuple>
#include <vector>

#include "../types/AlphaRange.h"

namespace Evaluation {

    /**
     * Combines all ranges to unique intervals in range [0,1] and averages costs for each interval.
     * @param costs - all input ranges
     * @param amount - number indicating how many experiments are included in the costs vectors.
     * @return averaged costs in [0,1]
     */
    std::vector<AlphaRange> average_costs(std::vector<AlphaRange> const &costs, double amount);

    /**
     * Compressed a vector of intervals by concatenating following intervals with same cost.
     * @param ranges all ranges
     * @return compressed vector of ranges
     */
    std::vector<AlphaRange> compress_regions(std::vector<AlphaRange> const &ranges);

    /**
     * Average all files in a given folder and export to given file.
     * @param input_folder - input directory
     * @param output_file - output file
     * @param ending - file ending of results  (default .txt)
     */
    void evaluate_folder(std::string input_folder, std::string output_file, std::string ending = ".txt");

    /**
     * Calculates greedy parameter advising (top k values) and exports to given file.
     * @param input_folder - input directory
     * @param max_k - maximum number of parameters
     * @param output_file - output file
     */
    void greedy_top_k(const std::string &input_folder, int max_k, const std::string &output_file);

    /**
     * Compress all files in a folder and export them to a different folder.
     * @param input_folder - input directory
     * @param output_folder - output directory
     */
    void write_compressed_files(std::string input_folder, std::string output_folder);

    /**
     * Finds cost for value alpha in a vector of ranges and writes it to ret_cost.
     * @param alpha - target value alpha
     * @param ranges - all ranges
     * @param ret_cost - found cost for alpha
     * @return if cost was found for alpha
     */
    bool find_cost_in_ranges(double alpha, std::vector<AlphaRange> ranges, double &ret_cost);

    /**
     * Gets all ranges in a given input file.
     * @param input_folder - input directory
     * @param file_path  - input file.
     * @return all found ranges.
     */
    std::vector<AlphaRange> get_ranges(std::string input_folder, std::string file_path);

    /**
     * Finds the optimal ranges for all given input files
     * @param input_folder - input directory
     * @param file_paths - all input files
     * @return a vector with all optimal ranges
     */
    std::vector<AlphaRange> get_opts(std::string input_folder, std::vector<std::string> file_paths);
};

#endif /* Evaluation_h */
