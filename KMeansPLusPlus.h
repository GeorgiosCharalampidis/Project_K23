//
// Created by test on 10/23/2023.
//

#ifndef PROJECTEM_KMEANSPLUSPLUS_H
#define PROJECTEM_KMEANSPLUSPLUS_H


#include <vector>
#include <random>
#include "lsh_class.h"
#include "Hypercube.h"


class KMeansPlusPlus {
public:
    explicit KMeansPlusPlus(const std::vector<std::vector<unsigned char>>& data, int k=10, std::string mode="Lloyds",
                            int k_LSH = 4, int L_LSH = 3, int k_CUBE = 7, int M_CUBE = 10, int probes_CUBE = 2,bool complete = false);

private:
    LSH lsh;
    Hypercube cube;
    std::string mode;
    std::vector<std::vector<unsigned char>> data_;
    int k_;
    std::vector<std::vector<unsigned char>> centroids_;
    std::vector<int> assignments_;
    std::vector<std::vector<std::vector<unsigned char>>> clusters_;
    std::mt19937 mt;
    bool complete;

    std::vector<std::vector<unsigned char>> getInitialCentroids();
    std::vector<unsigned char> computeMean(const std::vector<std::vector<unsigned char>>& cluster);

    int getClosestCentroidIndex(const std::vector<unsigned char>& point,
                                const std::vector<std::vector<unsigned char>>& centroids);
    int getNextCentroidIndex(const std::vector<double>& squaredDistances);

    void Lloyds();
    void reverseSearch(const std::string& method);

    double minDistanceToCentroid(const std::vector<unsigned char>& point,
                                 const std::vector<std::vector<unsigned char>>& centroids);
    double computeSilhouetteForPoint(const std::vector<unsigned char>& point, int assignedCluster);
    double averageDistanceToSameCluster(const std::vector<unsigned char>& point, int assignedCluster);
    double averageDistanceToNearestCluster(const std::vector<unsigned char>& point, int assignedCluster);
    void printClustersInfo(const std::string& algorithmName, double clusteringTime, bool completeOutput=false);



    };

#endif //PROJECTEM_KMEANSPLUSPLUS_H
