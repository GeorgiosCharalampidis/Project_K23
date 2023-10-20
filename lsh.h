#ifndef LSH_H
#define LSH_H

#include <vector>
#include <random>

void printHashTables(const std::vector<std::vector<std::vector<int>>>& hash_tables);


class LSH {
public:
    LSH(std::vector<std::vector<unsigned char>> dataset,std::vector<std::vector<unsigned char>> query,int k = 4, int L = 5, int num_dimensions = 784,int num_buckets = 15000,int N = 1, double R = 10000);
    ~LSH();
    void buildIndex(const std::vector<std::vector<unsigned char>>& dataset);
    void printHashTables() const;
    int countItemsInAllBuckets() const;
    //  int getNumberofBuckets() const;
    std::vector<std::pair<std::vector<double>, double>> createHashFunctions(int nf, int nd) const;
    std::vector<int> calculateHiValues(const std::vector<unsigned char>& data_point, int table_index);


    std::vector<int> rangeSearch(const std::vector<unsigned char>& query_point, double R);

    std::vector<std::pair<int, double>> queryNNearestNeighbors(const std::vector<unsigned char>& query_point, int N);
    std::vector<std::pair<int, double>> trueNNearestNeighbors(const std::vector<unsigned char>& query_point, int N);



        private:
    int k; // Number of hash functions
    int L; // Number of hash tables
    int num_buckets; // Number of buckets
    int N; // Number of nearest neighbors
    std::vector<std::vector<unsigned char>> dataset;
    std::vector<std::vector<unsigned char>> query;
    double w;
    std::vector<int> ri_values;


    int num_dimensions;
    double R; // radius
    std::vector<std::vector<std::vector<int>>> hash_tables;
    std::vector<std::vector<std::pair<std::vector<double>, double>>> hash_functions;

    int hashDataPoint(const std::vector<int>& hi_values);






};

#endif