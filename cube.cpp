///////////////////////////////////////////////////////
//                  Start of ./cube                  //
///////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include "mnist.h"
#include "Hypercube.h"
#include "global_functions.h"


int main123(int argc, char** argv) {

    char repeatChoice = 'n'; // to control the loop
    do {
        std::vector<std::string> args(argv, argv + argc);

        std::string inputFile, queryFile, outputFile;
        int k, M, probes, N, number_of_images, image_size;
        double radius;

        if (args.size() == 1) {  // Only mode provided, prompt for paths
            std::cout << "Enter the path to the dataset: ";
            std::cin >> inputFile;
            std::cout << inputFile << std::endl;
        } else {
            for (size_t i = 1; i < args.size(); i++) {
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
            return 2;
        }

        // Print k nearest neighbors
        // Loop through the first 10 query points


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

        // Ask the user if they want to repeat with new files
        std::cout << "Do you want to repeat with new input.dat and query.dat? (y/n): ";
        std::cin >> repeatChoice;

        if(repeatChoice == 'y' || repeatChoice == 'Y') {
            std::cout << "Enter the path to the new dataset (-d): ";
            std::cin >> inputFile;

            std::cout << "Enter the path to the new query file (-q): ";
            std::cin >> queryFile;

            // Optional: clear the arguments and populate with new paths
            args.clear();
            args.emplace_back(argv[0]);
            args.emplace_back("-d");
            args.push_back(inputFile);
            args.emplace_back("-q");
            args.push_back(queryFile);
        }

    } while (repeatChoice == 'y' || repeatChoice == 'Y'); // Repeat if user chooses 'y' or 'Y'

    return 0;
}
