#ifndef HYPERCUBE_H
#define HYPERCUBE_H

#include <vector>
#include <random>
#include <set>

class Hypercube {
public:
    Hypercube(std::vector<std::vector<unsigned char>> dataset,
              const std::vector<std::vector<unsigned char>>& query,
              int k, int num_dimensions, int N, double R, double w);
    ~Hypercube();

    // Inserts a data point into the hypercube
    void insert(const std::vector<double>& vec);

    // Queries the hypercube with a data point
    std::vector<std::vector<double>> query(const std::vector<double>& vec, int hammingThreshold);

    // Builds the index for the given dataset
    void buildIndex(const std::vector<std::vector<unsigned char>>& data_set);


    // Θα χρησιμοποιηθεί για την επιστροφή και των τριών μεταβλητών
    struct NearestNeighborsResult {
        std::vector<unsigned char> closestNeighbor;
        std::vector<std::vector<unsigned char>> NNearestNeighbors;
        std::vector<std::vector<unsigned char>> withinRange;
    };

    NearestNeighborsResult findNearest(const std::vector<unsigned char>& q, int N, double R);


private:
    // Calculates hash value for a given data point using hi values
    int hashDataPoint(const std::vector<int>& hi_values);

    // Calculates hi values for a given data point
    std::vector<int> calculateHiValues(const std::vector<unsigned char>& data_point);

    // Returns candidates by probing the hypercube for the given query point
    std::vector<int> probe(const std::vector<unsigned char>& query_point, int maxHammingDistance);

    // Defines the function to map hi values to {0, 1}
    int fi(int hi_value);

    // Creates hash functions for the hypercube
    std::vector<std::pair<std::vector<float>, float>> createHashFunctions(int k, int dim);


    std::vector<std::vector<unsigned char>> kNearestNeighbors(const std::vector<unsigned char>& q, int N);
    std::vector<std::vector<unsigned char>> rangeSearch(const std::vector<unsigned char>& q, double R);


        // Member variables
    std::vector<std::vector<unsigned char>> dataset;
    const std::vector<std::vector<unsigned char>>& queryDataset;
    int k;
    int num_dimensions;
    int N;
    double R;
    double w;
    std::vector<int> hash_table;
    std::vector<std::pair<std::vector<float>, float>> table_functions;
    std::mt19937 generator;
};

#endif // HYPERCUBE_H
