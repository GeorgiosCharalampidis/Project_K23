//
// Created by test on 10/18/2023.
//

#ifndef PROJECTEM_HYPERCUBE_H
#define PROJECTEM_HYPERCUBE_H

#include <vector>
#include <unordered_map>
#include <string>

class Hypercube {
private:
    int dimension;
    int projectedDimension;
    std::vector<std::vector<double>> randomMatrix;

    // Private methods for random projection and hashing

public:
    std::unordered_map<std::string, std::vector<std::vector<double>>> hypercube;

    Hypercube(int dim, int projDim);

    void insert(const std::vector<double>& vec);
    std::vector<std::vector<double>> query(const std::vector<double>& vec, int hammingThreshold);
};


#endif //PROJECTEM_HYPERCUBE_H
