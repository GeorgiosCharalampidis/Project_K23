//
// Created by test on 10/23/2023.
//

#ifndef PROJECTEM_KMEANSPLUSPLUS_H
#define PROJECTEM_KMEANSPLUSPLUS_H


#include <vector>

class KMeansPlusPlus {
public:
    KMeansPlusPlus(const std::vector<std::vector<unsigned char>>& data, int k);


private:
    std::vector<std::vector<unsigned char>> data_;
    int k_;
    std::vector<std::vector<unsigned char>> centroids_;
    std::vector<int> assignments_;
    std::vector<std::vector<std::vector<unsigned char>>> clusters_;

    double minDistanceToCentroid(const std::vector<unsigned char>& point,
                                 const std::vector<std::vector<unsigned char>>& centroids);

    int getNextCentroidIndex(const std::vector<double>& squaredDistances);

    std::vector<std::vector<unsigned char>> getInitialCentroids();

    void runKMeans();

    int getClosestCentroidIndex(const std::vector<unsigned char>& point,
                                const std::vector<std::vector<unsigned char>>& centroids);

    std::vector<unsigned char> computeMean(const std::vector<std::vector<unsigned char>>& cluster);
};

#endif //PROJECTEM_KMEANSPLUSPLUS_H
