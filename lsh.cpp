#include <iostream>
#include <stdexcept>
#include <vector>
#include <random>
#include <cmath>
#include <limits>
#include "lsh.h"

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

LSH::LSH(int k, int L, int num_dimensions, int num_buckets, int N, double R)
        : k(k), L(L),
          num_dimensions(num_dimensions),
          num_buckets(num_buckets),
          N(N), R(R),
          hash_tables(L, std::vector<std::vector<int>>(num_buckets)), // Ensure table has correct size initially
          hash_functions(L)
          {
                 // Create hash functions for each table here
                for(int i = 0; i < L; ++i) {
                    hash_functions[i] = createHashFunctions(k, num_dimensions);
                }

                /*
                // Create random 'ri' values for hash functions
                ri_values.resize(k);
                for (int i = 0; i < k; ++i) {
                    std::uniform_int_distribution<int> dist(1, M - 1);
                    ri_values[i] = dist(generator_);
                }
                */

            // Generate 'w' randomly in the range [0, 6]
            std::random_device rd;
            std::default_random_engine generator(rd());
            std::uniform_real_distribution<double> w_distribution(2.0, 6.0);
            w = w_distribution(generator);

          }



LSH::~LSH() {
    for (auto& table_functions : hash_functions) {
        for (auto& hash_function : table_functions) {
            hash_function.first.clear(); // Clear the vector within each pair
        }
        table_functions.clear(); // Clear the table's vector of hash functions
    }
    hash_functions.clear(); // Clear the vector of hash function tables
}


std::vector<std::pair<std::vector<double>, double>> LSH::createHashFunctions(int nf, int nd) const {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::normal_distribution<double> distribution(0.0, 1.0);
    std::uniform_real_distribution<double> uniform_dist(0, w);

    std::vector<std::pair<std::vector<double>, double>> local_hash_functions;
    local_hash_functions.reserve(nf);

    for (int i = 0; i < nf; ++i) {
        std::vector<double> v(nd);
        for (int j = 0; j < nd; ++j) {
            v[j] = distribution(generator);
        }
        double t = uniform_dist(generator);
        local_hash_functions.emplace_back(v, t);
    }
    return local_hash_functions;
}

void LSH::buildIndex(const std::vector<std::vector<unsigned char>>& dataset) {
    // std::cout << "Building index with dataset size: " << dataset.size() << std::endl;
    for (auto& table : hash_tables) {
        table.resize(num_buckets);
    }
    // std::cout << "Resized hash_tables" << std::endl;

    for (int i = 0; i < dataset.size(); ++i) {
        // std::cout << "Processing dataset item: " << i << std::endl;
        for (int table_index = 0; table_index < L; ++table_index) {
            // std::cout << "Hashing for table: " << table_index << std::endl;
            int hash_value = hash(dataset[i], table_index);
            // std::cout << "Hashed item " << i << " for table " << table_index << std::endl;

            try {
                // std::cout << "Before inserting into hash table" << std::endl;
                // std::cout << "hash_tables size: " << hash_tables.size() << " current table size: " << hash_tables[table_index].size() << std::endl;
                hash_tables[table_index][hash_value].push_back(i);
                // std::cout << "Inserted item " << i << " into hash table" << std::endl;
            } catch (const std::exception& e) {
                // std::cout << "Exception caught: " << e.what() << std::endl;
            } catch (...) {
                // std::cout << "Unknown exception caught" << std::endl;
            }
        }
    }
    // std::cout << "Finished building index" << std::endl;
}

int LSH::hash(const std::vector<unsigned char>& data_point, int table_index) {
    // std::cout << "Calculating hash for table: " << table_index << std::endl;
    if (table_index < 0 || table_index >= L) {
        throw std::out_of_range("Invalid table_index");
    }
    if (data_point.size() != num_dimensions) {
        throw std::invalid_argument("Invalid data_point dimensions");
    }

    int hash_value = 0;
    auto& table_functions = hash_functions[table_index];
    if (table_functions.size() != k) {
        throw std::runtime_error("Invalid number of hash functions for the table.");
    }

    for (int i = 0; i < k; ++i) {
        auto& [v, t] = table_functions[i];
        double dot_product = 0.0;
        for (int j = 0; j < num_dimensions; ++j) {
            dot_product += v[j] * data_point[j];
        }
        int hi = static_cast<int>(std::floor((dot_product + t) / w));
        // std::cout << "hi before binary operation: " << hi << std::endl;
        hash_value = (hash_value << 1) | (hi >= 0 ? hi & 1 : 0);
        // std::cout << "hi after binary operation: " << hi << std::endl;
    }
    // std::cout << "Hash value before anding: " << hash_value << std::endl;
    hash_value = hash_value & (num_buckets - 1);
    // std::cout << "Final hash value: " << hash_value << std::endl;

    return hash_value;
}


void LSH::printHashTables() const {
    for (int tableIndex = 0; tableIndex < L; ++tableIndex) {
        std::cout << "Table " << tableIndex << ":" << std::endl;

        for (int bucketIndex = 0; bucketIndex < (num_buckets); ++bucketIndex) {
            const std::vector<int>& bucket = hash_tables[tableIndex][bucketIndex];

            std::cout << "Bucket " << bucketIndex << ": ";
            for (int item : bucket) {
                std::cout << item << " ";
            }
            std::cout << std::endl;
        }

        std::cout << std::endl;
    }
}


std::vector<int> LSH::queryNearestNeighbor(const std::vector<unsigned char>& query_point, const std::vector<std::vector<unsigned char>>& dataset) {
    std::vector<int> nearest_neighbors;

    for (int table_index = 0; table_index < L; ++table_index) {
        int hash_value = hash(query_point, table_index);
        std::vector<int>& candidates = hash_tables[table_index][hash_value];

        double min_distance = std::numeric_limits<double>::max();
        int nearest_neighbor_index = -1;
        for (int candidate_index : candidates) {
            double distance = euclideanDistance(dataset[candidate_index], query_point);
            if (distance < min_distance) {
                min_distance = distance;
                nearest_neighbor_index = candidate_index;
            }
        }

        nearest_neighbors.push_back(nearest_neighbor_index);
    }

    return nearest_neighbors;
}

/*

std::vector<int> LSH::queryNNearestNeighbors(const std::vector<unsigned char>& query_point, int N) {
    // Implementation pending
    return {};
}

std::vector<int> LSH::rangeSearch(const std::vector<unsigned char>& query_point, double R) {
    // Implementation pending
    return {};
}
*/