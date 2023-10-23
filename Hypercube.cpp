#include "Hypercube.h"
#include <iostream>
#include <random>
#include <cmath>
#include <set>
#include <algorithm>
#include <queue>
#include "global_functions.h"  // Make sure this contains the computeDPrime function

Hypercube::Hypercube(std::vector<std::vector<unsigned char>> dataset,
                     int k,int M,int probes,
                     int N, double R)
        : dataset(std::move(dataset)),
          k(k),
          M(M),probes(probes),
          N(N), R(R),
          hash_table(1 << k, -1), // 2^k buckets
          table_functions(createHashFunctions(k, computeDPrime(n)))
{
    generator = std::mt19937(std::random_device{}());
    reduced_dimension = computeDPrime(n); 

    // Create the random projection matrix
    std::normal_distribution<float> distribution(0.0, 1.0);
    for (int i = 0; i < reduced_dimension; ++i) {
        std::vector<float> v;
        for (int j = 0; j < num_dimensions; ++j) {
            v.push_back(distribution(generator));
        }
        random_projection_matrix.push_back(v);
    }

    // Generate 'w' randomly in the range [0, 6]

    std::uniform_real_distribution<double> w_distribution(2.0, 6.0);
    w = w_distribution(generator);

    buildIndex();

}

Hypercube::~Hypercube() {
    table_functions.clear();
}

void Hypercube::buildIndex() {
    for (int i = 0; i < dataset.size(); ++i) {
        int hash_value = hashDataPoint(calculateHiValues(dataset[i]));
        hash_table[hash_value] = i;
    }
}

std::vector<int> Hypercube::probe(const std::vector<unsigned char>& query_point, int maxHammingDistance) {
    int hash_value = hashDataPoint(calculateHiValues(query_point));
    std::set<int> candidates;
    int vertices_checked = 0;

    for (int distance = 1; distance <= maxHammingDistance && vertices_checked < probes; ++distance) {
        for (int i = 0; i < k; ++i) {
            int neighbor_hash = hash_value ^ (1 << i);
            if (hash_table[neighbor_hash] != -1) {
                candidates.insert(hash_table[neighbor_hash]);
            }
            vertices_checked++;
        }
    }

    return {candidates.begin(), candidates.end()};
}

int Hypercube::fi(int hi_value) {
    return hi_value % 2;
}

int Hypercube::hashDataPoint(const std::vector<int>& hi_values) {
    if (hi_values.size() != k) {
        throw std::runtime_error("Invalid number of hi values.");
    }

    int g_value = 0;
    for (int i = 0; i < k; ++i) {
        int hi = hi_values[i];
        g_value |= (fi(hi) << i);
    }

    return g_value;
}

std::vector<std::pair<std::vector<float>, float>> Hypercube::createHashFunctions(int k_, int dim) {
    std::vector<std::pair<std::vector<float>, float>> table_functions_;
    std::normal_distribution<float> distribution(0.0, 1.0);
    std::uniform_real_distribution<float> offset_distribution(0.0, w);

    for (int i = 0; i < k_; ++i) {
        std::vector<float> v;
        for (int j = 0; j < dim; ++j) {
            v.push_back(distribution(generator));
        }
        float t = offset_distribution(generator);
        table_functions_.emplace_back(v, t);
    }

    return table_functions_;
}

std::vector<int> Hypercube::calculateHiValues(const std::vector<unsigned char>& data_point) {
    auto reduced_data_point = reduceDimensionality(data_point);

    if (reduced_data_point.size() != reduced_dimension) {
        throw std::runtime_error("Reduced data point dimensions mismatch.");
    }

    std::vector<int> hi_values;
    for (int i = 0; i < k; ++i) {
        auto& [v, t] = table_functions[i];
        double dot_product = 0.0;
        for (int j = 0; j < reduced_dimension; ++j) {
            dot_product += v[j] * reduced_data_point[j];
        }
        int hi = static_cast<int>(std::floor((dot_product + t) / w));
        hi += 100000;
        hi_values.push_back(hi);
    }

    return hi_values;
}

std::vector<std::pair<int, double>> Hypercube::kNearestNeighbors(const std::vector<unsigned char>& q) {
    std::priority_queue<std::pair<double, int>> nearest_neighbors_queue;

    std::vector<int> candidateIndices = probe(q, k);

    for (const auto& index : candidateIndices) {
        double distance = euclideanDistance(dataset[index], q);
        nearest_neighbors_queue.emplace(distance, index);
    }

    std::vector<std::pair<int, double>> nearest_neighbors;
    while (!nearest_neighbors_queue.empty() && nearest_neighbors.size() < N) {
        nearest_neighbors.emplace_back(nearest_neighbors_queue.top().second, nearest_neighbors_queue.top().first);
        nearest_neighbors_queue.pop();
    }

    // Reverse the vector to have the closest neighbors at the beginning
    std::reverse(nearest_neighbors.begin(), nearest_neighbors.end());

    return nearest_neighbors;
}



std::vector<int> Hypercube::rangeSearch(const std::vector<unsigned char>& q) {
    std::vector<int> candidateIndices = probe(q, k);
    std::set<int> inRangeIndices; // Use a set to avoid duplicates

    int candidates_checked = 0;
    for (const auto& index : candidateIndices) {
        if (candidates_checked >= M) break;

        double distance = euclideanDistance(dataset[index], q);
        if (distance <= R) {
            inRangeIndices.insert(index);
        }

        candidates_checked++;
    }

    // Convert the set to a vector for the final result
    std::vector<int> result(inRangeIndices.begin(), inRangeIndices.end());

    return result;
}



std::vector<float> Hypercube::reduceDimensionality(const std::vector<unsigned char>& data_point) {
    std::vector<float> reduced_point(reduced_dimension, 0.0);
    for (int i = 0; i < reduced_dimension; ++i) {
        for (int j = 0; j < num_dimensions; ++j) {
            reduced_point[i] += data_point[j] * random_projection_matrix[i][j];
        }
    }
    return reduced_point;
}

int Hypercube::returnN() const {
    return N;
}
