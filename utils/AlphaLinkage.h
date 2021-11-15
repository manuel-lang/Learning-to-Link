#ifndef AlphaLinkage_h
#define AlphaLinkage_h

#include <string>
#include <vector>

namespace AlphaLinkage {
    
    /**
     * Outputs all intervals and the according costs for the given input files into the given output file by interpolating
     * between single and complete linkage.
     * @param files - all to be evaluated input files
     * @param output_file - the file where the results will be written to
     * @param sublabels - the classes of interest
     * @param points_per_label - how many points of each class are used
     * @param batch_id - indicates which batch gets used (i.e. first, second, etc. sample of N points of each class)
     * @param verbose - output results to console
     * @param average - average over multiple files
     * @param use_majority - use majority cost instead of hamming cost
     */
    void single_complete(const std::vector<std::string> &files, const std::string &output_file,
                         const std::vector<double> &sublabels, int points_per_label, int batch_id, bool verbose,
                         bool average, bool use_majority);

    /**
     * Outputs all intervals and the according costs for the given input files into the given output file by interpolating
     * between single and average linkage.
     * @param files - all to be evaluated input files
     * @param output_file - the file where the results will be written to
     * @param sublabels - the classes of interest
     * @param points_per_label - how many points of each class are used
     * @param batch_id - indicates which batch gets used (i.e. first, second, etc. sample of N points of each class)
     * @param verbose - output results to console
     * @param average - average over multiple files
     * @param use_majority - use majority cost instead of hamming cost
     */
    void single_average(const std::vector<std::string> &files, const std::string &output_file,
                        const std::vector<double> &sublabels, int points_per_label, int batch_id, bool verbose,
                        bool average, bool use_majority);

    /**
     * Outputs all intervals and the according costs for the given input files into the given output file by interpolating
     * between average and complete linkage.
     * @param files - all to be evaluated input files
     * @param output_file - the file where the results will be written to
     * @param sublabels - the classes of interest
     * @param points_per_label - how many points of each class are used
     * @param batch_id - indicates which batch gets used (i.e. first, second, etc. sample of N points of each class)
     * @param verbose - output results to console
     * @param average - average over multiple files
     * @param use_majority - use majority cost instead of hamming cost
     */
    void average_complete(const std::vector<std::string> &files, const std::string &output_file,
                          const std::vector<double> &sublabels, int points_per_label, int batch_id, bool verbose,
                          bool average, bool use_majority);

    /**
     * Evaluates all data files (.csv) in a given folder by interpolating between single and complete linkage and outputs
     * all intervals with their corresponding costs to the given output file.
     * @param input_folder - the  input directory
     * @param output_file - the file where the results will be written to
     * @param sublabels - the classes of interest
     * @param points_per_label - how many points of each class are used
     * @param batch_id - indicates which batch gets used (i.e. first, second, etc. sample of N points of each class)
     * @param verbose - output results to console
     * @param average - average over multiple files
     * @param use_majority - use majority cost instead of hamming cost
     */
    void single_complete_folder(const std::string &input_folder, const std::string &output_file,
                                const std::vector<double> &sublabels, int points_per_label, int batch_id, bool verbose,
                                bool average, bool use_majority);

    /**
     * Evaluates all data files (.csv) in a given folder by interpolating between single and average linkage and outputs
     * all intervals with their corresponding costs to the given output file.
     * @param input_folder - the  input directory
     * @param output_file - the file where the results will be written to
     * @param sublabels - the classes of interest
     * @param points_per_label - how many points of each class are used
     * @param batch_id - indicates which batch gets used (i.e. first, second, etc. sample of N points of each class)
     * @param verbose - output results to console
     * @param average - average over multiple files
     * @param use_majority - use majority cost instead of hamming cost
     */
    void single_average_folder(const std::string &input_folder, const std::string &output_file,
                               const std::vector<double> &sublabels, int points_per_label, int batch_id, bool verbose,
                               bool average, bool use_majority);

    /**
     * Evaluates all data files (.csv) in a given folder by interpolating between average and complete linkage and outputs
     * all intervals with their corresponding costs to the given output file.
     * @param input_folder - the  input directory
     * @param output_file - the file where the results will be written to
     * @param sublabels - the classes of interest
     * @param points_per_label - how many points of each class are used
     * @param batch_id - indicates which batch gets used (i.e. first, second, etc. sample of N points of each class)
     * @param verbose - output results to console
     * @param average - average over multiple files
     * @param use_majority - use majority cost instead of hamming cost
     */
    void average_complete_folder(const std::string &input_folder, const std::string &output_file,
                                 const std::vector<double> &sublabels, int points_per_label, int batch_id, bool verbose,
                                 bool average, bool use_majority);
};

#endif /* AlphaLinkage_h */  
