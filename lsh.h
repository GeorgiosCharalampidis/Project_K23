#ifndef LSH_H
#define LSH_H

#include <vector>
#include <random>

class LSH {
public:
    explicit LSH(std::vector<std::vector<unsigned char>> dataset,int k = 4, int L = 5,int N = 1, double R = 10000);
    ~LSH();

  //  void printHashTables() const;
    //  int getNumberofBuckets() const;
    [[nodiscard]] std::vector<std::pair<std::vector<double>, double>> createHashFunctions(int nf, int nd) const;
    std::vector<int> calculateHiValues(const std::vector<unsigned char>& data_point, int table_index);


    std::vector<int> rangeSearch(const std::vector<unsigned char>& query_point);

    std::vector<std::pair<int, double>> queryNNearestNeighbors(const std::vector<unsigned char>& query_point);

    [[nodiscard]] int returnN() const;

private:

    int k; // Number of hash functions
    int L; // Number of hash tables
    int num_buckets = 15000; // Number of buckets
    int N; // Number of nearest neighbors
    std::vector<std::vector<unsigned char>> dataset;
    double w;
    std::vector<int> ri_values;


    int num_dimensions = 784;
    double R; // radius
    std::vector<std::vector<std::vector<int>>> hash_tables;
    std::vector<std::vector<std::pair<std::vector<double>, double>>> hash_functions;
    void buildIndex();
    int hashDataPoint(const std::vector<int>& hi_values);


};

#endif