#ifndef LSH_H
#define LSH_H

#include <vector>

void printHashTables(const std::vector<std::vector<std::vector<int>>>& hash_tables);


class LSH {
public:
    explicit LSH(int num_tables = 5, int num_functions = 4, int num_dimensions = 784, double radius = 10000.0);
    ~LSH();

    void buildIndex(const std::vector<std::vector<unsigned char>>& dataset);
    int queryNearestNeighbor(const std::vector<unsigned char>& query_point, const std::vector<std::vector<unsigned char>>& dataset);
    std::vector<int> queryNNearestNeighbors(const std::vector<unsigned char>& query_point, int N);
    std::vector<int> rangeSearch(const std::vector<unsigned char>& query_point, double R);
    std::vector<std::vector<int>> createHashFunctions(int nf, int nd, int w);
    void printHashTables() const;


private:
    int num_tables;
    int num_functions;
    int num_dimensions;
    double radius;
    int M;

    std::vector<std::vector<std::vector<int>>> hash_tables;
    std::vector<std::vector<int>> hash_functions;

    int hash(const std::vector<unsigned char>& data_point, int table_index);
};

#endif
