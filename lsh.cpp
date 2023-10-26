///////////////////////////////////////////////////////
//                  Start of ./lsh                   //
///////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include "mnist.h"
#include "lsh_class.h"
#include "global_functions.h"

int main(int argc, char** argv) {

    char repeatChoice = 'n'; // to control the loop
    do {

            std::vector<std::string> args(argv, argv + argc);

            std::string inputFile, queryFile, outputFile;
            int k, L, numberOfNearest, number_of_images, image_size;
            double radius;

            if (args.size() == 1) {  // Only mode provided, prompt for paths
                std::cout << "Enter the path to the dataset: ";
                std::cin >> inputFile;
            } else {
                for (size_t i = 1; i < args.size(); i++) {
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

            std::ofstream outputFileStream("output.dat");
            if (!outputFileStream.is_open() || outputFileStream.fail()) {
                std::cerr << "Failed to open output.dat for writing." << std::endl;
                return 2;
            }


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
                outputFileStream << "\nQuery: " << i << std::endl;

                for (int j = 0; j < numberOfNearest; ++j) {
                    outputFileStream << "Nearest neighbor-" << j + 1 << ": " << nearestNeighbors[j].first << std::endl;  // Assuming LSH and True produce the same index
                    outputFileStream << "distanceLSH: " << nearestNeighbors[j].second << std::endl;
                    outputFileStream << "distanceTrue: " << trueNearestNeighbors[j].second << std::endl;
                }

                outputFileStream << "tLSH: " << tLSH << " seconds" << std::endl;
                outputFileStream << "tTrue: " << tTrue << " seconds" << std::endl;

                // Assuming the lsh.rangeSearch function is used for R-near neighbors and it returns indices of images within the radius
                std::vector<int> withinRange = lsh.rangeSearch(query_set[i]);
                outputFileStream << "R-near neighbors:" << std::endl;
                for (int neighbor : withinRange) {
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


