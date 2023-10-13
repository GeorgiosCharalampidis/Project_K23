#ifndef LSH_H
#define LSH_H

#include <vector>


double euclideanDistance(const std::vector<unsigned char>& vec1, const std::vector<unsigned char>& vec2);


class LSH {
public:
    LSH(int num_tables, int num_functions, int num_dimensions, double radius);

    void buildIndex(const std::vector<std::vector<unsigned char>> &dataset);

    std::vector<int> query(const std::vector<unsigned char> &query_point);

private:
    int num_tables;
    int num_functions;
    int num_dimensions;
    double radius;

    std::vector<std::vector<std::vector<int>>> hash_tables;

    int hash(const std::vector<unsigned char> &data_point, int table_index);

#endif

};