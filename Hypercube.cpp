#include "Hypercube.h"
#include <iostream>
#include <random>
#include <cmath>
#include <set>
#include <algorithm>
#include <limits>

// Υπολογισμός της ευκλείδιας απόστασης μεταξύ δύο διανυσμάτων
double euclideanDistance_(const std::vector<unsigned char>& dataset, const std::vector<unsigned char>& query_set) {
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



Hypercube::Hypercube(std::vector<std::vector<unsigned char>> dataset,
                     const std::vector<std::vector<unsigned char>>& query,
                     int k, int num_dimensions, int N, double R, double w)
                    : dataset(std::move(dataset)),
                      queryDataset(query), // This was probably an error before.
                      k(k), num_dimensions(num_dimensions),
                      N(N), R(R), w(w),
                      hash_table(1 << k, -1),
                      table_functions(createHashFunctions(k, num_dimensions))
{
    generator = std::mt19937(std::random_device{}());

}

Hypercube::~Hypercube() {
    table_functions.clear();
}

std::vector<std::vector<double>> Hypercube::query(const std::vector<double>& vec, int hammingThreshold) {
    std::vector<int> candidateIndices = probe((const std::vector<unsigned char> &) vec, hammingThreshold);

    // Find the closest candidate to the query
    // TODO: Implement the logic to calculate the distance and determine the closest data point

    return {};  // Return the closest data point(s) based on the logic you implement
}

void Hypercube::buildIndex(const std::vector<std::vector<unsigned char>>& data_set) {
    for (int i = 0; i < data_set.size(); ++i) {
        int hash_value = hashDataPoint(calculateHiValues(data_set[i]));
        hash_table[hash_value] = i;
    }
}

std::vector<int> Hypercube::probe(const std::vector<unsigned char>& query_point, int maxHammingDistance) {
    int hash_value = hashDataPoint(calculateHiValues(query_point));
    std::set<int> candidates;

    for (int distance = 1; distance <= maxHammingDistance; ++distance) {
        for (int i = 0; i < k; ++i) {
            int neighbor_hash = hash_value ^ (1 << i);
            if (hash_table[neighbor_hash] != -1) {
                candidates.insert(hash_table[neighbor_hash]);
            }
        }
    }

    return std::vector<int>(candidates.begin(), candidates.end());
}

int Hypercube::fi(int hi_value) {
    return hi_value % 2;  // map even to 0, odd to 1
}

int Hypercube::hashDataPoint(const std::vector<int>& hi_values) {
    if (hi_values.size() != k) {
        throw std::runtime_error("Invalid number of hi values.");
    }

    int g_value = 0;
    for (int i = 0; i < k; ++i) {
        int hi = hi_values[i];
        g_value |= (fi(hi) << i);  // Use fi function
    }

    return g_value;
}

std::vector<std::pair<std::vector<float>, float>> Hypercube::createHashFunctions(int k, int dim) {
    std::vector<std::pair<std::vector<float>, float>> table_functions;
    std::normal_distribution<float> distribution(0.0, 1.0);
    std::uniform_real_distribution<float> offset_distribution(0.0, w);

    for (int i = 0; i < k; ++i) {
        std::vector<float> v;
        for (int j = 0; j < dim; ++j) {
            v.push_back(distribution(generator));
        }
        float t = offset_distribution(generator);
        table_functions.push_back({v, t});
    }

    return table_functions;
}

std::vector<int> Hypercube::calculateHiValues(const std::vector<unsigned char>& data_point) {
    if (data_point.size() != num_dimensions) {
        throw std::runtime_error("Data point dimensions mismatch.");
    }

    std::vector<int> hi_values;
    for (int i = 0; i < k; ++i) {
        auto& [v, t] = table_functions[i];
        double dot_product = 0.0;
        for (int j = 0; j < num_dimensions; ++j) {
            dot_product += v[j] * data_point[j];
        }
        int hi = static_cast<int>(std::floor((dot_product + t) / w));
        hi += 100000;
        hi_values.push_back(hi);
    }

    return hi_values;
}

std::vector<std::vector<unsigned char>> Hypercube::kNearestNeighbors(const std::vector<unsigned char>& q, int N) {
    std::vector<int> candidateIndices = probe(q, k);
    std::vector<std::pair<double, std::vector<unsigned char>>> distanceAndPoints;
    for (const auto& index : candidateIndices) {
        double distance = euclideanDistance_(dataset[index], q);
        distanceAndPoints.push_back({distance, dataset[index]});
    }

    std::sort(distanceAndPoints.begin(), distanceAndPoints.end());

    std::vector<std::vector<unsigned char>> kNearest;
    for (int i = 0; i < std::min(N, static_cast<int>(distanceAndPoints.size())); ++i) {
        kNearest.push_back(distanceAndPoints[i].second);
    }

    return kNearest;
}

std::vector<std::vector<unsigned char>> Hypercube::rangeSearch(const std::vector<unsigned char>& q, double R) {
    std::vector<int> candidateIndices = probe(q, k);
    std::vector<std::vector<unsigned char>> inRange;
    for (const auto& index : candidateIndices) {
        double distance = euclideanDistance_(dataset[index], q);
        if (distance <= R) {
            inRange.push_back(dataset[index]);
        }
    }
    return inRange;
}

Hypercube::NearestNeighborsResult Hypercube::findNearest(const std::vector<unsigned char>& q, int N, double R) {
    NearestNeighborsResult result;
    result.closestNeighbor = kNearestNeighbors(q, 1)[0];
    result.NNearestNeighbors = kNearestNeighbors(q, N);
    result.withinRange = rangeSearch(q, R);
    return result;
}