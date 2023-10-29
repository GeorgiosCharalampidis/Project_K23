#include "KMeansPLusPlus.h"
#include "global_functions.h"
#include <algorithm>
#include <random>
#include <limits>
#include <iostream>
#include <fstream>
#include <chrono>

KMeansPlusPlus::KMeansPlusPlus(const std::vector<std::vector<unsigned char>>& data, int k,std::string mode, int k_LSH, int L_LSH,
                               int k_CUBE, int M_CUBE, int probes_CUBE, bool complete)
        : data_(data), k_(k), mode(mode), lsh(data, k_LSH, L_LSH), cube(data, k_CUBE, M_CUBE, probes_CUBE),
        complete(complete)

        {

        // KMeans++ initialization
        centroids_ = getInitialCentroids();

        if (mode == "Lloyds") {
            //std::cout << "Lloyds selected.." << std::endl << std::endl;
            auto start = std::chrono::high_resolution_clock::now(); // start timer
            Lloyds();
            auto end = std::chrono::high_resolution_clock::now(); // end timer
            double timer = std::chrono::duration<double, std::milli>(end - start).count() / 1000.0; // convert to seconds
            std::cout << "Lloyds: " << timer << " seconds" << std::endl << std::endl;
            std::cout << "Silhouette for Lloyds is being created.." << std::endl;
            printClustersInfo("Lloyds", timer, complete);
            std::cout << "Silhouette for Lloyds saved on output.txt" << std::endl << std::endl;

        } else if (mode == "LSH") {
            //std::cout << "LSH selected.." << std::endl << std::endl;
            auto start = std::chrono::high_resolution_clock::now(); // start timer
            reverseSearch("LSH");
            auto end = std::chrono::high_resolution_clock::now(); // end timer
            double timer = std::chrono::duration<double, std::milli>(end - start).count() / 1000.0; // convert to seconds
            std::cout << "reverseLSH: " << timer << " seconds" << std::endl << std::endl;
            std::cout << "Silhouette for LSH is being created.." << std::endl;
            printClustersInfo("Range Search LSH", timer, complete);
            std::cout << "Silhouette for LSH saved on output.txt" << std::endl << std::endl;

        } else if (mode == "HyperCube") {
            //std::cout << "HyperCube selected.." << std::endl << std::endl;
            auto start = std::chrono::high_resolution_clock::now(); // start timer
            reverseSearch("HyperCube");
            auto end = std::chrono::high_resolution_clock::now(); // end timer
            double timer = std::chrono::duration<double, std::milli>(end - start).count() / 1000.0; // convert to seconds
            std::cout << "reverseHyperCube: " << timer << " seconds" << std::endl << std::endl;
            std::cout << "Silhouette for HyperCube is being created.." << std::endl;
            printClustersInfo("Range Search Hypercube", timer, complete);
            std::cout << "Silhouette for HyperCube saved on output.txt" << std::endl << std::endl;
    }
}
KMeansPlusPlus::~KMeansPlusPlus() {
    for (auto& cluster : clusters_) {
        cluster.clear();
    }
    clusters_.clear();
    assignments_.clear();
    centroids_.clear();
    data_.clear();
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
    mt.seed(rd());
    std::uniform_real_distribution<double> dist(0.0, total);
    // Choose new centroid: r chosen with probability proportional to D(r)^2.

    double randomValue = dist(mt);

    // Use binary search to find the index corresponding to the randomValue
    auto it = std::lower_bound(partialSums.begin(), partialSums.end(), randomValue);
    return static_cast<int>(std::distance(partialSums.begin(), it));
}

std::vector<std::vector<unsigned char>> KMeansPlusPlus::getInitialCentroids() {
    std::vector<std::vector<unsigned char>> centroids;
    std::vector<double> squaredDistances;

    // 1. Choose a centroid uniformly at random
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    std::uniform_int_distribution<size_t> distInt(0, data_.size() - 1);
    size_t randomIndex = distInt(mt);
    centroids.push_back(data_[randomIndex]);

    // Iterate k-1 times to choose the remaining k-1 centroids
    for (size_t i = 1; i < k_; ++i) {
        squaredDistances.clear();

        // 2. For each point, compute squared distance to the nearest centroid
        for (const auto &point: data_) {
            double distance = minDistanceToCentroid(point, centroids);
            squaredDistances.push_back(distance * distance);
        }

        // Normalize squaredDistances by dividing each distance by the maximum distance
        double maxDistance = *std::max_element(squaredDistances.begin(), squaredDistances.end());
        for (double &distanceValue: squaredDistances) {  // Renamed 'dist' to 'distanceValue' to avoid shadowing
            distanceValue /= maxDistance;
        }

        // Compute partial sums for squared normalized distances
        std::vector<double> partialSums(squaredDistances.size());
        std::partial_sum(squaredDistances.begin(), squaredDistances.end(), partialSums.begin());

        // 3. Choose the next centroid based on the probabilities
        int chosenIndex = getNextCentroidIndex(partialSums);
        centroids.push_back(data_[chosenIndex]);
    }

    return centroids;
}

// MACQUEEN Update
void KMeansPlusPlus::Lloyds() {
    // Get initial centroids
    std::vector<std::vector<unsigned char>> oldCentroids;

    do {
        oldCentroids = centroids_;

        // Assignment and immediate update steps
        clusters_.clear();
        clusters_.resize(k_);
        assignments_.resize(data_.size());

        for (size_t i = 0; i < data_.size(); ++i) {
            const auto& point = data_[i];
            int closestCentroidIndex = getClosestCentroidIndex(point, centroids_);
            if (closestCentroidIndex >= 0 && closestCentroidIndex < k_) {
                clusters_[closestCentroidIndex].push_back(point);
                assignments_[i] = closestCentroidIndex;

                // MacQueen's update
                centroids_[closestCentroidIndex] = computeMean(clusters_[closestCentroidIndex]);

            } else {
                std::cerr << "Error: Invalid centroid index " << closestCentroidIndex << " for data point " << i << std::endl;
            }
        }

    } while (centroids_ != oldCentroids);
}


// Clustering using reverse search LSH or HyperCube
void KMeansPlusPlus::reverseSearch(const std::string& method) {
    bool converged = false;

    while (!converged) {
        clusters_.clear();
        clusters_.resize(centroids_.size());
        std::vector<int> assignments(data_.size(), -1);
        // Calculate the initial radius as half the minimum distance between centroids
        double min_distance = std::numeric_limits<double>::max();
        for (size_t i = 0; i < centroids_.size(); ++i) {
            for (size_t j = i + 1; j < centroids_.size(); ++j) {
                double dist = euclideanDistance(centroids_[i], centroids_[j]);
                if (dist < min_distance) {
                    min_distance = dist;
                }
            }
        }

        double current_radius = min_distance / 2;
        bool allCentroidsGotPoints;

        do {
            allCentroidsGotPoints = true;
            //std::cout << "current_radius: " << current_radius << std::endl;
            for (size_t i = 0; i < centroids_.size(); ++i) {
                auto& centroid = centroids_[i];
                std::vector<int> points_in_radius;

                if (method == "LSH") {
                    points_in_radius = lsh.rangeSearch(centroid, current_radius);
                } else if (method == "HyperCube") {
                    //std::cout << centroid.size() << std::endl;
                    points_in_radius = cube.rangeSearch(centroid, current_radius);

                }
                //std::cout << "points_in_radius.size(): " << points_in_radius.size() << std::endl;
                for (int point_idx : points_in_radius) {
                    if (assignments[point_idx] == -1 || euclideanDistance(data_[point_idx], centroid) < euclideanDistance(data_[point_idx], centroids_[assignments[point_idx]])) {
                        // Assignment
                        assignments[point_idx] = i;
                        clusters_[i].push_back(data_[point_idx]);

                    }
                }

                if (points_in_radius.empty()) {
                    allCentroidsGotPoints = false; // At least one centroid got no points
                    //std::cout << "Centroid " << i << " got no points" << std::endl;
                }
            }

            // Update centroids once after range search for the current radius is done for all centroids
            for (size_t i = 0; i < centroids_.size(); ++i) {
                if (!clusters_[i].empty()) {
                    centroids_[i] = computeMean(clusters_[i]);
                }
            }

            if (!allCentroidsGotPoints) {
                current_radius *= 2;
                //std::cout << "Doubling radius to " << current_radius << std::endl;
            }

        } while (!allCentroidsGotPoints);

        // Check for convergence
        converged = true;
        std::vector<std::vector<unsigned char>> new_centroids(centroids_.size());
        for (size_t i = 0; i < centroids_.size(); ++i) {
            new_centroids[i] = computeMean(clusters_[i]);
            if (euclideanDistance(centroids_[i], new_centroids[i]) > 0.0001) {
                converged = false;
                break;
            }
        }

        // Handle unassigned points
        for (size_t i = 0; i < assignments.size(); ++i) {
            if (assignments[i] == -1) {
                int closestCentroidIndex = getClosestCentroidIndex(data_[i], centroids_);
                assignments[i] = closestCentroidIndex;
                clusters_[closestCentroidIndex].push_back(data_[i]);

                // Update the centroid after assigning a previously unassigned point
                centroids_[closestCentroidIndex] = computeMean(clusters_[closestCentroidIndex]);
            }
        }
        assignments_ = assignments;
    }
}

int KMeansPlusPlus::getClosestCentroidIndex(const std::vector<unsigned char>& point,
                                            const std::vector<std::vector<unsigned char>>& centroids) {

    int index = 0;
    double minDistance = std::numeric_limits<double>::max();
    for (int i = 0; i < centroids.size(); ++i) {
        double distance = euclideanDistance(point, centroids[i]);
        if (distance < minDistance) {
            minDistance = distance;
            index = i;
        }
    }

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

double KMeansPlusPlus::computeSilhouetteForPoint(const std::vector<unsigned char>& point, int assignedCluster) {
    double a_i = averageDistanceToSameCluster(point, assignedCluster);
    double b_i = averageDistanceToNearestCluster(point, assignedCluster);

    return (b_i - a_i) / std::max(a_i, b_i);
}

double KMeansPlusPlus::averageDistanceToSameCluster(const std::vector<unsigned char>& point, int assignedCluster) {
    double totalDistance = 0.0;
    int count = 0;

    for (const auto& otherPoint : clusters_[assignedCluster]) {
        // Make sure the point doesn't compare to itself
        if (point != otherPoint) {
            totalDistance += euclideanDistance(point, otherPoint);
            count++;
        }
    }

    if (count == 0) return 0;  // edge case: either the cluster is empty or only has the point itself
    return totalDistance / count;
}

double KMeansPlusPlus::averageDistanceToNearestCluster(const std::vector<unsigned char>& point, int assignedCluster) {
    double minAverageDistance = std::numeric_limits<double>::max();

    for (int i = 0; i < k_; i++) {
        if (i == assignedCluster) continue;  // skip the cluster to which the point belongs

        double totalDistance = 0.0;
        int count = 0;

        for (const auto& otherPoint : clusters_[i]) {
            totalDistance += euclideanDistance(point, otherPoint);
            count++;
        }

        if (count == 0) continue;  // edge case: cluster might be empty
        double averageDistance = totalDistance / count;

        if (averageDistance < minAverageDistance) {
            minAverageDistance = averageDistance;
        }
    }
    return minAverageDistance;
}

void KMeansPlusPlus::printClustersInfo(const std::string& algorithmName, double clusteringTime, bool completeOutput) {

    std::ofstream outputFileStream("output.txt");
    if (!outputFileStream.is_open() || outputFileStream.fail()) {
        std::cerr << "Failed to open output.dat for writing." << std::endl;
        return;
    }

    outputFileStream << "Algorithm: " << algorithmName << "\n";

    // Compute the silhouette values for each cluster and the dataset
    std::vector<double> silhouetteValues(k_, 0.0);
    int totalDataPoints = 0;
    double overallSilhouette = 0.0;
    for (int i = 0; i < k_; i++) {
        double silhouetteSum = 0.0;
        for (const auto& point : clusters_[i]) {
            silhouetteSum += computeSilhouetteForPoint(point, i);
        }
        silhouetteValues[i] = silhouetteSum / clusters_[i].size();
        overallSilhouette += silhouetteSum;
        totalDataPoints += clusters_[i].size();
    }
    overallSilhouette /= totalDataPoints;

    for (int i = 0; i < k_; i++) {
        outputFileStream << std::endl << "CLUSTER-" << (i+1) << " {size: " << clusters_[i].size()
                  << ", centroid: [";
        for (const auto& val : centroids_[i]) {
            outputFileStream << static_cast<int>(val) << " ";  // Casting unsigned char to int for printing
        }
        outputFileStream << "]}\n";
    }
    outputFileStream << std::endl << "clustering_time: " << clusteringTime << " //in seconds\n";
    outputFileStream << std::endl << "Silhouette: [";
    for (const auto& s : silhouetteValues) {
        outputFileStream << s << ", ";
    }

    outputFileStream << overallSilhouette << "]\n";

    if (completeOutput) {
        outputFileStream << std::endl << "Complete Output:" << std::endl;
        for (int i = 0; i < k_; i++) {
            outputFileStream << std::endl << "CLUSTER-" << (i+1) << " {centroid: [";
            for (const auto& val : centroids_[i]) {
                outputFileStream << static_cast<int>(val) << " ";
            }
            outputFileStream << "], ";
            for (size_t j = 0; j < assignments_.size(); ++j) {
                if (assignments_[j] == i) {
                    outputFileStream << "image_number" << j << ", ";
                }
            }
            outputFileStream << "}\n";
        }
    }

    outputFileStream.flush();  // Ensure all data is written
    outputFileStream.close();  // Close the file

}

