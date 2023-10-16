#ifndef LSH_H
#define LSH_H

#include <vector>

void printHashTables(const std::vector<std::vector<std::vector<int>>>& hash_tables);


class LSH {
public:
    LSH(int num_tables = 5, int num_functions = 5, int num_dimensions = 784, double radius= 10000);
    ~LSH();
    void buildIndex(const std::vector<std::vector<unsigned char>>& dataset);
    void printHashTables() const;


    std::vector<int> queryNearestNeighbor(const std::vector<unsigned char>& query_point, const std::vector<std::vector<unsigned char>>& dataset);
//    std::vector<int> queryNNearestNeighbors(const std::vector<unsigned char>& query_point, int N);
//    std::vector<int> rangeSearch(const std::vector<unsigned char>& query_point, double R);

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

#endif