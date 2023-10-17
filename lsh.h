#ifndef LSH_H
#define LSH_H

#include <vector>
#include <random>

void printHashTables(const std::vector<std::vector<std::vector<int>>>& hash_tables);


class LSH {
public:
    LSH(int k = 4, int L = 5, int num_dimensions = 784,int num_buckets = 32,int N = 1, double R = 10000);
    ~LSH();
    void buildIndex(const std::vector<std::vector<unsigned char>>& dataset);
    void printHashTables() const;
  //  int getNumberofBuckets() const;
  //  std::vector<int> ri_values;
    // const int64_t M = (1LL << 31) - 5; // Define M as a large prime, for example: 2^31 - 5
   // static std::default_random_engine generator_;
   // const int64_t M = (1LL << 31) - 5; // Define M as a large prime, for example: 2^31 - 5


    std::vector<int> queryNearestNeighbor(const std::vector<unsigned char>& query_point, const std::vector<std::vector<unsigned char>>& dataset);
//    std::vector<int> queryNNearestNeighbors(const std::vector<unsigned char>& query_point, int N);
//    std::vector<int> rangeSearch(const std::vector<unsigned char>& query_point, double R);

private:
    int k; // Number of hash functions
    int L; // Number of hash tables
    int num_buckets; // Number of buckets
    int N; // Number of nearest neighbors
    double w;

    int num_dimensions;
    double R; // radius
    std::vector<std::vector<std::vector<int>>> hash_tables;
    std::vector<std::vector<std::pair<std::vector<double>, double>>> hash_functions;

    int hash(const std::vector<unsigned char>& data_point, int table_index);
    std::vector<std::pair<std::vector<double>, double>> createHashFunctions(int nf, int nd) const;



};

#endif