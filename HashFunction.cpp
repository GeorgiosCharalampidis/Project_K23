#include "HashFunction.h"
#include <random>

HashFunction::HashFunction(int nd, double w_range_low, double w_range_high)
        : num_dimensions(nd) {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::normal_distribution<double> distribution(0.0, 1.0);
    std::uniform_real_distribution<double> uniform_dist(w_range_low, w_range_high);

    v.resize(nd);
    for (int j = 0; j < nd; ++j) {
        v[j] = distribution(generator);
    }
    t = uniform_dist(generator);
}

int HashFunction::computeHash(const std::vector<unsigned char>& data_point, double w) const {
    double dot_product = 0.0;
    for (int j = 0; j < num_dimensions; ++j) {
        dot_product += v[j] * data_point[j];
    }
    return static_cast<int>(std::floor((dot_product + t) / w));
}
