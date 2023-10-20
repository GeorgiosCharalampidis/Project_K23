#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>
#include <random>
#include <cmath>
#include <limits>
#include <set>

#include <queue>
#include "lsh.h"
#include "global_functions.h"


// LSH Constructor
LSH::LSH(std::vector<std::vector<unsigned char>> dataset,std::vector<std::vector<unsigned char>> query,int k, int L, int num_dimensions, int num_buckets, int N, double R)
        : dataset(std::move(dataset)),
          query(std::move(query)),
          k(k), L(L),
          num_dimensions(num_dimensions),
          num_buckets(num_buckets),
          N(N), R(R),
          hash_tables(L, std::vector<std::vector<int>>(num_buckets)), // Ensure table has correct size initially
          hash_functions(L)
{
    // Δημιουργία των hash functions για κάθε table
    for(int i = 0; i < L; ++i) {
        hash_functions[i] = createHashFunctions(k, num_dimensions);
    }

    // Δημιουργία τυχαίων τιμών 'ri' για τα hash functions
    ri_values.resize(k);
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> dist(0, std::numeric_limits<int>::max()); // Range for int
    for (int i = 0; i < k; ++i) {
        ri_values[i] = dist(generator);
    }

    // Generate 'w' randomly in the range [0, 6]


    std::uniform_real_distribution<double> w_distribution(2.0, 6.0);
    w = w_distribution(generator);
    buildIndex(this->dataset);
}

// LSH Destructor
LSH::~LSH() {
    for (auto& table_functions : hash_functions) {
        for (auto& hash_function : table_functions) {
            hash_function.first.clear(); // Clear the vector within each pair
        }
        table_functions.clear(); // Clear the table's vector of hash functions
    }
    hash_functions.clear(); // Clear the vector of hash function tables
}

void LSH::buildIndex(const std::vector<std::vector<unsigned char>>& data_set) {
    // std::cout << "Building index with dataset size: " << dataset.size() << std::endl;
    for (auto& table : hash_tables) {
        table.resize(num_buckets);
    }
    // std::cout << "Resized hash_tables" << std::endl;

    for (int i = 0; i < data_set.size(); ++i) {
        // std::cout << "Processing dataset item: " << i << std::endl;
        for (int table_index = 0; table_index < L; ++table_index) {
            // std::cout << "Hashing for table: " << table_index << std::endl;
            int hash_value = hashDataPoint(calculateHiValues(data_set[i], table_index));
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

// Δημιουργία μιας λίστας από nf hash_functions για το LSH χρησιμοποιώντας κανονικές και ομοιόμορφες κατανομές
std::vector<std::pair<std::vector<double>, double>> LSH::createHashFunctions(int nf, int nd) const {
    // Αρχικοποίηση random number generator
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::normal_distribution<double> distribution(0.0, 1.0);
    std::uniform_real_distribution<double> uniform_dist(0, w);

    std::vector<std::pair<std::vector<double>, double>> local_hash_functions;
    local_hash_functions.reserve(nf);
    // Δημιουργήσαμε nf hash_functions που παράγουν και αποθηκεύουν v μεγέθους nd και t στο [0, w)
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


std::vector<int> LSH::calculateHiValues(const std::vector<unsigned char>& data_point, int table_index) {
    std::vector<int> hi_values; // Store hi values

    // Initialize random number generator
    if (table_index < 0 || table_index >= L) {
        throw std::out_of_range("Invalid table_index");
    }
    if (data_point.size() != num_dimensions) {
        throw std::invalid_argument("Invalid data_point dimensions");
    }

    auto& table_functions = hash_functions[table_index];
    if (table_functions.size() != k) {
        throw std::runtime_error("Invalid number of hash functions for the table.");
    }

    // Calculate hi values
    for (int i = 0; i < k; ++i) {
        auto& [v, t] = table_functions[i];
        double dot_product = 0.0;
        for (int j = 0; j < num_dimensions; ++j) {
            dot_product += v[j] * data_point[j];
        }
        int hi = static_cast<int>(std::floor((dot_product + t) / w));
        hi += 100000; // Ensure it's positive
        hi_values.push_back(hi);
    }

    return hi_values; // Return the vector of hi values
}



// Υπολογισμός του hash για ένα δεδομένο σημείο και το αντίστοιχο table
int LSH::hashDataPoint(const std::vector<int>& hi_values) {
    if (hi_values.size() != k) {
        throw std::runtime_error("Invalid number of hi values.");
    }

    const int64_t M = (1LL << 32) - 5; // Define M as a large prime
    uint64_t g_value = 0;

    for (int i = 0; i < k; ++i) {
        int hi = hi_values[i];
        uint64_t ri_hi_mod_M = (ri_values[i] * hi) % M;
        g_value = (g_value + ri_hi_mod_M) % M;
    }
    g_value = g_value % num_buckets;

    return g_value;
}

int LSH::countItemsInAllBuckets() const {
    int total_count = 0;

    // count items in all buckets
    for (int tableIndex = 0; tableIndex < L; ++tableIndex) {
        for (int bucketIndex = 0; bucketIndex < (num_buckets); ++bucketIndex) {
            const std::vector<int>& bucket = hash_tables[tableIndex][bucketIndex];
            total_count += bucket.size();
        }
    }
    return total_count;
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

std::vector<int> LSH::queryNNearestNeighbors(const std::vector<unsigned char>& query_point, int Number_of_Neighbors) {
    std::priority_queue<std::pair<double, int>> nearest_neighbors_queue;

    for (int table_index = 0; table_index < L; ++table_index) {
        int hash_value = hashDataPoint(calculateHiValues(query_point, table_index));
        const std::vector<int>& candidates = hash_tables[table_index][hash_value];

        for (int candidate_index : candidates) {
            double distance = euclideanDistance(dataset[candidate_index], query_point);
            nearest_neighbors_queue.emplace(distance, candidate_index);
        }
    }

    std::vector<int> nearest_neighbors;
    while (!nearest_neighbors_queue.empty() && nearest_neighbors.size() < Number_of_Neighbors) {
        nearest_neighbors.push_back(nearest_neighbors_queue.top().second);
        nearest_neighbors_queue.pop();
    }

    return nearest_neighbors;
}

std::vector<int> LSH::rangeSearch(const std::vector<unsigned char>& query_point, double Radius) {
    std::set<int> candidates_within_radius; // Χρησιμοποιούμε το set για να αποφύγουμε διπλότυπα

    for (int table_index = 0; table_index < L; ++table_index) {
        // Hash the query point for the current table
        int hash_value = hashDataPoint(calculateHiValues(query_point, table_index));

        // Retrieve the candidates from the hashed bucket
        const std::vector<int>& candidates = hash_tables[table_index][hash_value];

        for (int candidate_index : candidates) {
            double distance = euclideanDistance(dataset[candidate_index], query_point);

            // Check if the distance is within the desired range
            if (distance <= Radius) {
                candidates_within_radius.insert(candidate_index);
            }
        }
    }

    // Convert the set to a vector for the final result
    std::vector<int> result(candidates_within_radius.begin(), candidates_within_radius.end());

    return result;
}