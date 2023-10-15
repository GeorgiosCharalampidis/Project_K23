#include <iostream>
#include <stdexcept>
#include <vector>
#include <random>
#include <cmath>
#include <limits>

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

class LSH {
public:
    LSH(int num_tables, int num_functions, int num_dimensions, double radius);
    void buildIndex(const std::vector<std::vector<unsigned char>>& dataset);
    void printHashTables() const;
    /*
    std::vector<int> queryNearestNeighbor(const std::vector<unsigned char>& query_point, const std::vector<std::vector<unsigned char>>& dataset);
    std::vector<int> queryNNearestNeighbors(const std::vector<unsigned char>& query_point, int N);
    std::vector<int> rangeSearch(const std::vector<unsigned char>& query_point, double R);
    */
private:
    int num_tables;
    int num_functions;
    int num_dimensions;
    double radius;
    std::vector<std::vector<std::vector<int>>> hash_tables;
    std::vector<std::vector<std::pair<std::vector<double>, double>>> hash_functions;

    int hash(const std::vector<unsigned char>& data_point, int table_index);
    static std::vector<std::pair<std::vector<double>, double>> createHashFunctions(int nf, int nd, int w);
};

LSH::LSH(int num_tables, int num_functions, int num_dimensions, double radius)
        : num_tables(num_tables), num_functions(num_functions),
          num_dimensions(num_dimensions), radius(radius),
          hash_tables(num_tables, std::vector<std::vector<int>>(1 << num_functions)), // Ensure table has correct size initially
          hash_functions(num_tables) {
    // Create hash functions for each table here
    for(int i = 0; i < num_tables; ++i) {
        hash_functions[i] = createHashFunctions(num_functions, num_dimensions, radius);
    }
}

std::vector<std::pair<std::vector<double>, double>> LSH::createHashFunctions(int nf, int nd, int w) {
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
        table.resize(1 << num_functions);
    }
    // std::cout << "Resized hash_tables" << std::endl;

    for (int i = 0; i < dataset.size(); ++i) {
        // std::cout << "Processing dataset item: " << i << std::endl;
        for (int table_index = 0; table_index < num_tables; ++table_index) {
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
    if (table_index < 0 || table_index >= num_tables) {
        throw std::out_of_range("Invalid table_index");
    }
    if (data_point.size() != num_dimensions) {
        throw std::invalid_argument("Invalid data_point dimensions");
    }

    int hash_value = 0;
    auto& table_functions = hash_functions[table_index];
    if (table_functions.size() != num_functions) {
        throw std::runtime_error("Invalid number of hash functions for the table.");
    }

    for (int i = 0; i < num_functions; ++i) {
        auto& [v, t] = table_functions[i];
        double dot_product = 0.0;
        for (int j = 0; j < num_dimensions; ++j) {
            dot_product += v[j] * data_point[j];
        }
        int hi = static_cast<int>(std::floor((dot_product + t) / radius));
        // std::cout << "hi before binary operation: " << hi << std::endl;
        hash_value = (hash_value << 1) | (hi >= 0 ? hi & 1 : 0);
        // std::cout << "hi after binary operation: " << hi << std::endl;
    }
    // std::cout << "Hash value before anding: " << hash_value << std::endl;
    hash_value = hash_value & ((1 << num_functions) - 1);
    // std::cout << "Final hash value: " << hash_value << std::endl;

    return hash_value;
}

void LSH::printHashTables() const {
    for (int tableIndex = 0; tableIndex < num_tables; ++tableIndex) {
        std::cout << "Table " << tableIndex << ":" << std::endl;

        for (int bucketIndex = 0; bucketIndex < (1 << num_functions); ++bucketIndex) {
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

/*

std::vector<int> LSH::queryNearestNeighbor(const std::vector<unsigned char>& query_point, const std::vector<std::vector<unsigned char>>& dataset) {
    std::vector<int> nearest_neighbors;

    for (int table_index = 0; table_index < num_tables; ++table_index) {
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


std::vector<int> LSH::queryNNearestNeighbors(const std::vector<unsigned char>& query_point, int N) {
    // Implementation pending
    return {};
}

std::vector<int> LSH::rangeSearch(const std::vector<unsigned char>& query_point, double R) {
    // Implementation pending
    return {};
}
*/
