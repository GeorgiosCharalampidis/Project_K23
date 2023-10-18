#include "Hypercube.h"
#include <iostream>
#include <random>

Hypercube::Hypercube(int dim, int projDim)
        : dimension(dim), projectedDimension(projDim) {
    // Initialize the random matrix
    // ...
}

void Hypercube::insert(const std::vector<double>& vec) {
    // Implement insertion logic
    // ...
}

std::vector<std::vector<double>> Hypercube::query(const std::vector<double>& vec, int hammingThreshold) {
    // Implement query logic
    // ...
}
