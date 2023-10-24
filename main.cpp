#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
 #include "main_helper.h"

/*
#include "KMeansPLusPlus.h"
#include "minst.h"
#include "lsh.h"
#include "Hypercube.h"
#include "global_functions.h"
*/

#include <chrono>

/*

int main() {

    int number_of_images, image_size;

    // Create vector to store dataset
    std::string input = R"(C:\Users\test\CLionProjects\Project_K23\input.dat)";
    std::string query = R"(C:\Users\test\CLionProjects\Project_K23\query.dat)";
    std::string outputPath = R"(C:\Users\test\CLionProjects\Project_K23\output.dat)";

    std::vector<std::vector<unsigned char>> dataset = read_mnist_images(input, number_of_images, image_size);
    std::vector<std::vector<unsigned char>> query_set = read_mnist_images(query, number_of_images,image_size);

    // Create a Test_Set with the first 100 images of the dataset
    std::vector<std::vector<unsigned char>> test_set(query_set.begin(), query_set.begin() + 100);

    LSH lsh(dataset);

    int numberOfNearest = lsh.returnN(); // Get the number of nearest neighbors from the LSH object
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



    //KMeansPlusPlus plus(dataset, 5);

    return 0;
}

    */



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
