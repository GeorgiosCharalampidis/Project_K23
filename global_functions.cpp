#include "global_functions.h"
#include <vector>
#include <cmath>
#include <stdexcept>
#include <random>


//
// Functions that will be used throughout the project
//

// Υπολογισμός της ευκλείδιας απόστασης μεταξύ δύο διανυσμάτων
double euclideanDistance(const std::vector<unsigned char>& dataset, const std::vector<unsigned char>& query_set) {
    if (dataset.size() != query_set.size()) {
        throw std::runtime_error("Vectors must have the same dimension for L2 distance calculation.");
    }
    double distance = 0.0;
    for (size_t i = 0; i < dataset.size(); ++i) {
        double diff = static_cast<double>(dataset[i]) - static_cast<double>(query_set[i]);
        distance += diff * diff;
    }
    return std::sqrt(distance);
}

int computeDPrime(int n) {
    int logValue = static_cast<int>(std::log2(n));
    int d_prime_lower_bound = logValue - 3;
    int d_prime_upper_bound = logValue - 1;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(d_prime_lower_bound, d_prime_upper_bound);

    return dist(mt);
}



