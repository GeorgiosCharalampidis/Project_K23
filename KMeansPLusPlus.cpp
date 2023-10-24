#include "KMeansPLusPlus.h"
#include "global_functions.h"
#include <algorithm>
#include <random>
#include <numeric>
#include <limits>
#include <iostream>

KMeansPlusPlus::KMeansPlusPlus(const std::vector<std::vector<unsigned char>>& data, int k)
        : data_(data), k_(k) {

    runKMeans();
}

double KMeansPlusPlus::minDistanceToCentroid(const std::vector<unsigned char>& point,
                                             const std::vector<std::vector<unsigned char>>& centroids) {
    double minDistance = std::numeric_limits<double>::max();
    for (const auto& centroid : centroids) {
        double distance = euclideanDistance(point, centroid);
        if (distance < minDistance) {
            minDistance = distance;
        }
    }
    return minDistance;
}

int KMeansPlusPlus::getNextCentroidIndex(const std::vector<double>& squaredDistances) {
    std::vector<double> partialSums(squaredDistances.size(), 0.0);
    partialSums[0] = squaredDistances[0];

    // Compute the partial sums
    for (size_t i = 1; i < squaredDistances.size(); ++i) {
        partialSums[i] = partialSums[i-1] + squaredDistances[i];
    }

    double total = partialSums.back();

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, total);
    double randomValue = dist(mt);

    // Use binary search to find the index corresponding to the randomValue
    auto it = std::lower_bound(partialSums.begin(), partialSums.end(), randomValue);
    return static_cast<int>(std::distance(partialSums.begin(), it));
}


std::vector<std::vector<unsigned char>> KMeansPlusPlus::getInitialCentroids() {
    std::vector<std::vector<unsigned char>> centroids;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<size_t> dist(0, data_.size() - 1);
    size_t randomIndex = dist(mt);

    centroids.push_back(data_[randomIndex]);

    for (int t = 1; t < k_; ++t) {
        std::vector<double> squaredDistances;
        for (const auto &point: data_) {
            double distance = minDistanceToCentroid(point, centroids);
            squaredDistances.push_back(distance * distance);
        }

        int chosenIndex = getNextCentroidIndex(squaredDistances);
        centroids.push_back(data_[chosenIndex]);
    }

    return centroids;
}


void KMeansPlusPlus::runKMeans() {
    // Get initial centroids
    centroids_ = getInitialCentroids();
    std::vector<std::vector<unsigned char>> oldCentroids;


    while (centroids_ != oldCentroids){
        oldCentroids = centroids_;

        /*
        // Print the contents of the data_ vector
        std::cout << "Data Points:" << std::endl;
        for (const auto& point : data_) {
            for (const unsigned char value : point) {
                std::cout << static_cast<int>(value) << " ";
            }
            std::cout << std::endl;
        }
        */

        // Assignment step
        clusters_.clear();
        clusters_.resize(k_);
        assignments_.resize(data_.size());

        // Debugging
        /*
        for (int i = 0; i < k_; ++i) {
            std::cout << "Cluster " << i << " (Before Update): Dimensions = " << clusters_[i].size() << std::endl;
            std::cout << "Centroid " << i << " (Before Update): Dimensions = " << centroids_[i].size() << std::endl;
        }
         */


        for (size_t i = 0; i < data_.size(); ++i) {
            const auto& point = data_[i];
            int closestCentroidIndex = getClosestCentroidIndex(point, centroids_);
            if (closestCentroidIndex >= 0 && closestCentroidIndex < k_) {
                clusters_[closestCentroidIndex].push_back(point);
                //std::cout << "Cluster " << closestCentroidIndex << " (After Assignment): Dimensions = " << clusters_[closestCentroidIndex].size() << std::endl;
                assignments_[i] = closestCentroidIndex;
            } else {
                std::cerr << "Error: Invalid centroid index " << closestCentroidIndex << " for data point " << i << std::endl;
            }
        }

        // Update step
        for (int i = 0; i < k_; ++i) {
            //std::cout << "Updating centroid " << i << std::endl;
            centroids_[i] = computeMean(clusters_[i]);
            // print clusters values
            //std::cout << "Cluster " << i << " (After Update): Dimensions = " << clusters_[i].size() << std::endl;
            // print centroids values
            //std::cout << "Centroid " << i << " (After Update): Dimensions = " << centroids_[i].size() << std::endl;
            //std::cout << "Updated centroid " << i << std::endl;
        }

        /*
        for (int i = 0; i < k_; ++i) {
            std::cout << "Cluster " << i << " (After Update): Dimensions = " << clusters_[i].size() << std::endl;
            std::cout << "Centroid " << i << " (After Update): Dimensions = " << centroids_[i].size() << std::endl;
        }
        */

    }
}

int KMeansPlusPlus::getClosestCentroidIndex(const std::vector<unsigned char>& point,
                                            const std::vector<std::vector<unsigned char>>& centroids) {

    //std::cout << "Debug: Entering getClosestCentroidIndex" << std::endl;
    //std::cout << "Debug: Dimensions of point: " << point.size() << std::endl;
    //std::cout << "Debug: Number of centroids: " << centroids.size() << std::endl;

    int index = 0;
    double minDistance = std::numeric_limits<double>::max();
    //std::cout << "centroid size: " << centroids.size() << std::endl;
    for (int i = 0; i < centroids.size(); ++i) {
        //std::cout << "Debug: Entering for loop" << std::endl;
        //std::cout << "Debug: Dimensions of centroid " << i << ": " << centroids[i].size() << std::endl;
        //std::cout << "Debug: Dimensions of point: " << point.size() << std::endl;
        double distance = euclideanDistance(point, centroids[i]);
        if (distance < minDistance) {
            minDistance = distance;
            index = i;
        }
    }

    //std::cout << "Debug: Closest centroid index: " << index << std::endl;

    return index;
}

std::vector<unsigned char> KMeansPlusPlus::computeMean(const std::vector<std::vector<unsigned char>>& cluster) {
    if (cluster.empty()) {
        return {};
    }

    size_t dimension = cluster[0].size();
    // Using vector of doubles for intermediate mean values
    std::vector<double> intermediateMean(dimension, 0.0);
    std::vector<unsigned char> mean(dimension, 0);

    // Sum up values using double to avoid overflow
    for (const auto& point : cluster) {
        for (size_t i = 0; i < dimension; ++i) {
            intermediateMean[i] += point[i];
        }
    }

    // Calculate mean using floating-point division and round the result
    for (size_t i = 0; i < dimension; ++i) {
        intermediateMean[i] /= static_cast<double>(cluster.size());
        mean[i] = static_cast<unsigned char>(std::round(intermediateMean[i]));
    }

    return mean;
}
