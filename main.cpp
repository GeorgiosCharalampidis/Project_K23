#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include "minst.h"
#include "lsh.h"
#include "Hypercube.h"
#include "global_functions.h"

int main(int argc, char* argv[]) {

    std::string inputFile, queryFile, outputFile;
    int k, L, M, probes, N, numberOfNearest;
    double radius;
    int number_of_images, image_size;

    if (argc < 2) {
        std::cerr << "Specify a mode: lsh or cube." << std::endl;
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "./lsh") {
        if (argc == 2) {  // Only mode provided, prompt for paths
            std::cout << "Enter the path to the dataset: ";
            std::cin >> inputFile;
        } else {
            for (int i = 2; i < argc; ++i) {
                if (strcmp(argv[i], "-d") == 0) {
                    inputFile = argv[++i];
                } else if (strcmp(argv[i], "-q") == 0) {
                    queryFile = argv[++i];
                } else if (strcmp(argv[i], "-k") == 0) {
                    k = std::stoi(argv[++i]);
                } else if (strcmp(argv[i], "-L") == 0) {
                    L = std::stoi(argv[++i]);
                } else if (strcmp(argv[i], "-o") == 0) {
                    outputFile = argv[++i];
                } else if (strcmp(argv[i], "-N") == 0) {
                    numberOfNearest = std::stoi(argv[++i]);
                } else if (strcmp(argv[i], "-R") == 0) {
                    radius = std::stod(argv[++i]);
                }
            }
        }


        std::vector<std::vector<unsigned char>> dataset = read_mnist_images(inputFile, number_of_images, image_size);

        if (argc == 2) {
            // If no arguments were given other than mode, then prompt for the paths after building the search structure
            std::cout << "Enter the path to the query file: ";
            std::cin >> queryFile;
            std::cout << "Enter the path for output file: ";
            std::cin >> outputFile;
        }

        std::vector<std::vector<unsigned char>> query_set = read_mnist_images(queryFile, number_of_images,image_size);


        LSH lsh = (argc == 2) ? LSH(dataset) : LSH(dataset, k, L, numberOfNearest, radius);

        numberOfNearest = lsh.returnN(); // Get the number of nearest neighbors from the LSH object
        //lsh.printHashTables();
        // Perform the N nearest neighbor search

        // ...[Omitted previous code sections for brevity]

        // LSH nearest neighbors and true nearest neighbors
        for (int i = 0; i < 10; ++i) {

            auto startLSH = std::chrono::high_resolution_clock::now(); // start LSH timer
            // LSH nearest neighbors for the query point
            std::vector<std::pair<int, double>> nearestNeighbors = lsh.queryNNearestNeighbors(query_set[i]);
            auto endLSH = std::chrono::high_resolution_clock::now(); // end LSH timer

            auto startTrue = std::chrono::high_resolution_clock::now(); // start True timer
            // True nearest neighbors for the query point
            std::vector<std::pair<int, double>> trueNearestNeighbors = trueNNearestNeighbors(dataset, query_set[i], numberOfNearest);

            auto endTrue = std::chrono::high_resolution_clock::now(); // end True timer

            double tLSH = std::chrono::duration<double, std::milli>(endLSH - startLSH).count() / 1000.0; // convert to seconds
            double tTrue = std::chrono::duration<double, std::milli>(endTrue - startTrue).count() / 1000.0; // convert to seconds

            // Print in the combined format
            std::cout << "\nQuery: " << i << std::endl;

            for (int j = 0; j < numberOfNearest; ++j) {
                std::cout << "Nearest neighbor-" << j + 1 << ": " << nearestNeighbors[j].first << std::endl;  // Assuming LSH and True produce the same index
                std::cout << "distanceLSH: " << nearestNeighbors[j].second << std::endl;
                std::cout << "distanceTrue: " << trueNearestNeighbors[j].second << std::endl;
            }

            std::cout << "tLSH: " << tLSH << " seconds" << std::endl;
            std::cout << "tTrue: " << tTrue << " seconds" << std::endl;

            // Assuming the lsh.rangeSearch function is used for R-near neighbors and it returns indices of images within the radius
            std::vector<int> withinRange = lsh.rangeSearch(query_set[i]);
            std::cout << "R-near neighbors:" << std::endl;
            for (int neighbor : withinRange) {
                std::cout << neighbor << std::endl;
            }
        }

    } else if (mode == "./cube") {
        if (argc == 2) {  // Only mode provided, prompt for paths
            std::cout << "Enter the path to the dataset: ";
            std::cin >> inputFile;
        } else {
            for (int i = 2; i < argc; ++i) {
                if (strcmp(argv[i], "-d") == 0) {
                    inputFile = argv[++i];
                } else if (strcmp(argv[i], "-q") == 0) {
                    queryFile = argv[++i];
                } else if (strcmp(argv[i], "-k") == 0) {
                    k = std::stoi(argv[++i]);
                } else if (strcmp(argv[i], "-M") == 0) {
                    M = std::stoi(argv[++i]);
                } else if (strcmp(argv[i], "-probes") == 0) {
                    probes = std::stoi(argv[++i]);
                } else if (strcmp(argv[i], "-o") == 0) {
                    outputFile = argv[++i];
                } else if (strcmp(argv[i], "-N") == 0) {
                    N = std::stoi(argv[++i]);
                } else if (strcmp(argv[i], "-R") == 0) {
                    radius = std::stod(argv[++i]);
                }
            }
        }
        std::vector<std::vector<unsigned char>> dataset = read_mnist_images(inputFile, number_of_images, image_size);

        if (argc == 2) {
            // If no arguments were given other than mode, then prompt for the paths after building the search structure
            std::cout << "Enter the path to the query file: ";
            std::cin >> queryFile;
            std::cout << "Enter the path for output file: ";
            std::cin >> outputFile;
        }

        std::vector<std::vector<unsigned char>> query_set = read_mnist_images(queryFile, number_of_images,image_size);

        Hypercube hypercube = (argc == 2) ? Hypercube(dataset) : Hypercube(dataset,k, M, probes, N, radius);

        N = hypercube.returnN(); // Get the number of nearest neighbors from the Hypercube object

        std::ofstream outputFileStream("output.dat");
        if (!outputFileStream.is_open() || outputFileStream.fail()) {
            std::cerr << "Failed to open output.dat for writing." << std::endl;
            return 1;
        }


        // Print k nearest neighbors
        for (int i = 0; i < 10; ++i) {

            auto startCube = std::chrono::high_resolution_clock::now(); // start timer
            std::vector<std::pair<int, double>> nearestNeighbors = hypercube.kNearestNeighbors(query_set[i]);
            auto endCube = std::chrono::high_resolution_clock::now(); // end timer

            auto startTrue = std::chrono::high_resolution_clock::now(); // start True timer
            // True nearest neighbors for the query point
            std::vector<std::pair<int, double>> trueNearestNeighbors = trueNNearestNeighbors(dataset, query_set[i], N);
            auto endTrue = std::chrono::high_resolution_clock::now(); // end True timer

            double tCube = std::chrono::duration<double, std::milli>(endCube - startCube).count() / 1000.0; // convert to seconds
            double tTrue = std::chrono::duration<double, std::milli>(endTrue - startTrue).count() / 1000.0; // convert to seconds*/

            outputFileStream << "\nQuery: " << i << std::endl;

            for (int j = 0; j < N; ++j) {
                outputFileStream << "Nearest neighbor-" << j + 1 << ": " << nearestNeighbors[j].first << std::endl;
                outputFileStream << "distanceHypercube: " << nearestNeighbors[j].second << std::endl;
                outputFileStream << "distanceTrue: " << trueNearestNeighbors[j].second << std::endl;
            }

            outputFileStream << "tHypercube: " << tCube << " seconds" << std::endl;
            outputFileStream << "tTrue: " << tTrue << " seconds" << std::endl;

            // Assuming the hypercube.rangeSearch function is used for R-near neighbors and it returns indices of images within the radius
            std::vector<int> withinRange = hypercube.rangeSearch(query_set[i]);
            outputFileStream << "R-near neighbors:" << std::endl;

            for (const auto& neighbor : withinRange) {
                outputFileStream << neighbor << std::endl;
            }

        }

        outputFileStream.flush();  // Ensure all data is written
        outputFileStream.close();  // Close the file


    } else {
        std::cerr << "Invalid mode: " << mode << std::endl;
        return 1;
    }


    // After producing results, ask the user if they want to continue or terminate
    char decision;
    std::cout << std::endl << "Repeat the search for a different set/query file? (Type Y/N): ";
    std::cin >> decision;

    if (decision == 'Y' || decision == 'y') {
        return main(argc, argv);  // Recursive call to main (consider looping instead for large iterations)
    }
    else if(decision == 'N' || decision == 'n'){
        return 0;
    }

}