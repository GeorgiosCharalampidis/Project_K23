#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <chrono>
#include "minst.h"
#include "lsh.h"
#include "Hypercube.h"
#include "global_functions.h"

void handleLSHMode(const std::vector<std::string>& args, int argc) {
    std::string inputFile, queryFile, outputFile;
    int k, L, numberOfNearest, number_of_images, image_size;
    double radius;

    if (args.size() == 2) {  // Only mode provided, prompt for paths
        std::cout << "Enter the path to the dataset: ";
        std::cin >> inputFile;
    } else {
        for (size_t i = 2; i < args.size(); i++) {
            if (args[i] == "-d") {
                inputFile = args[++i];
            } else if (args[i] == "-q") {
                queryFile = args[++i];
            } else if (args[i] == "-k") {
                k = std::stoi(args[++i]);
            } else if (args[i] == "-L") {
                L = std::stoi(args[++i]);
            } else if (args[i] == "-o") {
                outputFile = args[++i];
            } else if (args[i] == "-N") {
                numberOfNearest = std::stoi(args[++i]);
            } else if (args[i] == "-R") {
                radius = std::stod(args[++i]);
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
}

// Function to handle the Hypercube mode logic
void handleCubeMode(const std::vector<std::string>& args, int argc) {
    std::string inputFile, queryFile, outputFile;
    int k, M, probes, N, number_of_images, image_size;
    double radius;

    if (args.size() == 2) {  // Only mode provided, prompt for paths
        std::cout << "Enter the path to the dataset: ";
        std::cin >> inputFile;
    } else {
        for (size_t i = 2; i < args.size(); i++) {
            if (args[i] == "-d") {
                inputFile = args[++i];
            } else if (args[i] == "-q") {
                queryFile = args[++i];
            } else if (args[i] == "-k") {
                k = std::stoi(args[++i]);
            } else if (args[i] == "-M") {
                M = std::stoi(args[++i]);
            } else if (args[i] == "-probes") {
                probes = std::stoi(args[++i]);
            } else if (args[i] == "-o") {
                outputFile = args[++i];
            } else if (args[i] == "-N") {
                N = std::stoi(args[++i]);
            } else if (args[i] == "-R") {
                radius = std::stod(args[++i]);
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
        return;
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
}



int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Specify a mode: lsh or cube." << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    char decision;

    do {
        if (mode == "./lsh") {
            handleLSHMode(std::vector<std::string>(argv, argv + argc), argc);
        } else if (mode == "./cube") {
            handleCubeMode(std::vector<std::string>(argv, argv + argc), argc);
        } else {
            std::cerr << "Invalid mode: " << mode << std::endl;
            return 1;
        }

        // After producing results, ask the user if they want to continue or terminate
        std::cout << "\nRepeat the search for a different set/query file? (Type Y/N): ";
        std::cin >> decision;
        std::cin.ignore(); // Clear newline left in buffer

        // If user decides to continue, prompt for new mode and arguments
        if (decision == 'Y' || decision == 'y') {
            std::cout << "Enter the program mode (./lsh or ./cube): ";
            std::getline(std::cin, mode);

            // Clear previous arguments and keep only the program name (argv[0])
            argv[1] = &mode[0];
            argc = 2;

            if (mode == "./lsh" || mode == "./cube") {
                std::cout << "Enter any additional arguments (or press enter to skip): ";
                std::string args;
                std::getline(std::cin, args);
                std::istringstream iss(args);
                std::string arg;
                while (iss >> arg && argc < 20) {
                    argv[argc] = new char[arg.length() + 1];  // +1 for the null-terminator


                    std::strcpy(argv[argc], arg.c_str());
                    argc++;
                }
            } else {
                std::cerr << "Invalid mode: " << mode << std::endl;
                return 1;
            }
        }

    } while (decision == 'Y' || decision == 'y');

    return 0;
}