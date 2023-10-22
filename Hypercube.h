#ifndef HYPERCUBE_H
#define HYPERCUBE_H

#include <vector>
#include <random>
#include <set>

class Hypercube {
public:
    Hypercube(std::vector<std::vector<unsigned char>> dataset,
              const std::vector<std::vector<unsigned char>>& query,
              int k = 14,int M=10,int probes=2, int N = 1, double R = 10000);
    ~Hypercube();



    // Builds the index for the given dataset
    void buildIndex(const std::vector<std::vector<unsigned char>>& data_set);


    std::vector<std::pair<int, double>> kNearestNeighbors(const std::vector<unsigned char>& q);
    std::vector<int> rangeSearch(const std::vector<unsigned char>& q, double R);

    int returnN() const;
    int returnR() const;

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

    // Reduces the dimensionality of the data point using the random projection matrix
    std::vector<float> reduceDimensionality(const std::vector<unsigned char>& data_point);



    // Member variables
    std::vector<std::vector<unsigned char>> dataset;
    const std::vector<std::vector<unsigned char>>& queryDataset;
    int k;
    int num_dimensions = 784;
    int N;
    double R;
    double w;
    int reduced_dimension;
    int M;
    int n=60000;
    int probes;
    std::vector<std::vector<float>> random_projection_matrix;
    std::vector<int> hash_table;
    std::vector<std::pair<std::vector<float>, float>> table_functions;
    std::mt19937 generator;
};

#endif // HYPERCUBE_H
