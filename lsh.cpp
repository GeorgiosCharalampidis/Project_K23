#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <random>
#include <cmath>
#include "minst.h"

// Συνάρτηση για υπολογισμό της ευκλείδειας απόστασης (L2) μεταξύ δύο διανυσμάτων
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

// Συνάρτηση για δημιουργία τυχαίων hash functions
std::vector<int> createRandomHashFunction(int num_dimensions, int M, int H) {
    std::vector<int> hi;

    for (int j = 0; j < H; ++j) {
        int ri = std::rand() % M;
        hi.push_back(ri);
    }

    return hi;
}

class LSH {
public:
    LSH(int num_tables, int num_functions, int num_dimensions, double radius);

    void buildIndex(const std::vector<std::vector<unsigned char>>& dataset);
    std::vector<int> query(const std::vector<unsigned char>& query_point, const std::vector<std::vector<unsigned char>>& dataset);

private:
    int num_tables;
    int num_functions;
    int num_dimensions;
    double radius;
    int M;

    std::vector<std::vector<std::vector<int>>> hash_tables;
    std::vector<std::vector<int>> hash_functions; // Αποθηκεύστε τις hash functions

    int hash(const std::vector<unsigned char>& data_point, int table_index);
};

LSH::LSH(int num_tables, int num_functions, int num_dimensions, double radius) {
    this->num_tables = num_tables;
    this->num_functions = num_functions;
    this->num_dimensions = num_dimensions;
    this->radius = radius;

    this->hash_tables.resize(num_tables);
    for (int i = 0; i < num_tables; ++i) {
        this->hash_tables[i].resize(1 << num_functions);
    }

    this->M = 1000; // Μπορείτε να τροποποιήσετε αυτή την τιμή ανάλογα με τις ανάγκες σας
    this->hash_functions.resize(num_tables);
}

void LSH::buildIndex(const std::vector<std::vector<unsigned char>>& dataset) {
    for (int table_index = 0; table_index < num_tables; ++table_index) {
        std::vector<int> table_functions = createRandomHashFunction(num_dimensions, M, num_functions);
        hash_functions.push_back(table_functions);
    }

    for (int i = 0; i < dataset.size(); ++i) {
        for (int table_index = 0; table_index < num_tables; ++table_index) {
            int hash_value = hash(dataset[i], table_index);
            hash_tables[table_index][hash_value].push_back(i);
        }
    }
}

int LSH::hash(const std::vector<unsigned char>& data_point, int table_index) {
    if (table_index < 0 || table_index >= num_tables) {
        throw std::out_of_range("Invalid table_index");
    }

    if (data_point.size() != num_dimensions) {
        throw std::invalid_argument("Invalid data_point dimensions");
    }

    int hash_value = 0;
    std::vector<int> table_functions = hash_functions[table_index];

    if (table_functions.size() != num_functions) {
        throw std::runtime_error("Invalid number of hash functions for the table.");
    }

    for (int i = 0; i < num_functions; ++i) {
        int ri = table_functions[i];
        hash_value += (static_cast<int>(data_point[i]) * ri) % M;
    }

    hash_value = hash_value % (1 << num_functions);

    return hash_value;
}

std::vector<int> LSH::query(const std::vector<unsigned char>& query_point, const std::vector<std::vector<unsigned char>>& dataset) {
    std::vector<int> nearest_neighbors;

    for (int table_index = 0; table_index < num_tables; ++table_index) {
        int hash_value = hash(query_point, table_index);
        std::vector<int> candidates = hash_tables[table_index][hash_value];

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
