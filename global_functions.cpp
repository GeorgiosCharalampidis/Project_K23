#include "global_functions.h"
#include <vector>
#include <cmath>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <queue>


//
// Functions that will be used throughout the project
//




// Υπολογισμός της ευκλείδιας απόστασης μεταξύ δύο διανυσμάτων
double euclideanDistance(const std::vector<unsigned char>& dataset, const std::vector<unsigned char>& query_set) {
    if (dataset.size() != query_set.size()) {
        throw std::runtime_error("Vectors must have the same dimension for L2 distance calculation.");
    }
    double distance = 0.0;
    for (size_t i = 0; i < dataset.size(); ++i) {
        double diff = static_cast<double>(dataset[i]) - static_cast<double>(query_set[i]);
        distance += diff * diff;
    }
    return std::sqrt(distance);
}

int computeDPrime(int n) {
    int logValue = static_cast<int>(std::log2(n));
    int d_prime_lower_bound = logValue - 3;
    int d_prime_upper_bound = logValue - 1;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(d_prime_lower_bound, d_prime_upper_bound);

    return dist(mt);
}

std::vector<std::pair<int, double>> trueNNearestNeighbors(const std::vector<std::vector<unsigned char>>& dataset,
                                                          const std::vector<unsigned char>& query_point, int N) {
    // Check for dataset's emptiness
    if (dataset.empty()) {
        throw std::runtime_error("Dataset is empty.");
    }

    if (N <= 0) {
        throw std::invalid_argument("N must be positive.");
    }

    // Pair: distance, index. We use distance as the key for the priority queue.
    std::priority_queue<std::pair<double, int>> max_heap;

    for (int i = 0; i < dataset.size(); ++i) {
        double distance = euclideanDistance(dataset[i], query_point);

        // If we haven't yet found N neighbors, or the current point is closer than the farthest neighbor found so far.
        if (max_heap.size() < N || distance < max_heap.top().first) {
            if (max_heap.size() == N) {
                max_heap.pop(); // Remove the farthest neighbor
            }
            max_heap.emplace(distance, i); // Add the current point
        }
    }

    std::vector<std::pair<int, double>> nearest_neighbors;
    while (!max_heap.empty()) {
        nearest_neighbors.emplace_back(max_heap.top().second, max_heap.top().first);
        max_heap.pop();
    }

    // The priority queue will order from largest to smallest distance. So, reverse for correct ordering.
    std::reverse(nearest_neighbors.begin(), nearest_neighbors.end());

    return nearest_neighbors;
}

std::vector<unsigned char> convertToUnsignedChar(const std::vector<double>& vec) {
    std::vector<unsigned char> result;
    result.reserve(vec.size());

    for(const auto& val : vec) {
        result.push_back(static_cast<unsigned char>(std::round(val)));
    }

    return result;
}

// ------------------------------------     SECTION OF UNUSED FUNCTIONS     ------------------------------------ //

/*

// reverseSearch uses MacQueen's Update to update the centroid after every point assignment
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
            for (size_t i = 0; i < centroids_.size(); ++i) {
                auto& centroid = centroids_[i];
                std::vector<int> points_in_radius;

                if (method == "LSH") {
                    points_in_radius = lsh.rangeSearch(centroid, current_radius);
                } else if (method == "HyperCube") {
                    points_in_radius = cube.rangeSearch(centroid, current_radius);
                }

                if (points_in_radius.empty()) {
                    allCentroidsGotPoints = false; // At least one centroid got no points
                } else {
                    for (int point_idx : points_in_radius) {
                        if (assignments[point_idx] == -1 || euclideanDistance(data_[point_idx], centroid) < euclideanDistance(data_[point_idx], centroids_[assignments[point_idx]])) {
                            // Assignment
                            assignments[point_idx] = i;
                            clusters_[i].push_back(data_[point_idx]);

                            // MacQueen's Update
                            centroids_[i] = computeMean(clusters_[i]);
                        }
                    }
                }
            }

            if (!allCentroidsGotPoints) {
                current_radius *= 2;
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



 */