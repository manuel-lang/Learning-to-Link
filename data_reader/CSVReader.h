//
//  csv_reader.hpp
//  AlphaLinkage
//
//  Created by Manuel Lang on 26.09.18.
//  Copyright © 2018 Manuel Lang. All rights reserved.
//

#ifndef csv_reader_hpp
#define csv_reader_hpp

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

inline std::vector<std::vector<double>> readcsv(const std::string &inputFileName)
{
    std::vector<std::vector<double> > data;
    std::ifstream inputFile(inputFileName);
    int l = 0;

    while (inputFile)
    {
        l++;
        std::string s;
        if (!getline(inputFile, s))
            break;
        if (s[0] != '#')
        {
            std::istringstream ss(s);
            std::vector<double> record;

            while (ss)
            {
                std::string line;
                if (!getline(ss, line, ','))
                    break;
                try
                {
                    record.push_back(stof(line));
                }
                catch (const std::invalid_argument &e)
                {
                    std::cout << "NaN found in file " << inputFileName << " line " << l
                    << std::endl;
                    e.what();
                }
            }

            data.push_back(record);
        }
    }

    if (!inputFile.eof())
    {
        std::cerr << "Could not read file " << inputFileName << "\n";
        std::__throw_invalid_argument("File not found.");
    }

    return data;
}


#endif /* csv_reader_hpp */
