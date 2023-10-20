#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include "minst.h"
#include "lsh.h"
#include "Hypercube.h"

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

        // Create the lsh object using parsed arguments
        LSH lsh(dataset,query_set,4,5,784,15000,5,20000);
        lsh.buildIndex(dataset);
        //lsh.printHashTables();

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

        Hypercube hypercube(dataset,query_set,k,784,M,probes,N,radius);

        hypercube.buildIndex(dataset);

    } else {
        std::cerr << "Invalid mode: " << mode << std::endl;
        return 1;
    }



    // After producing results, ask the user if they want to continue or terminate
    char decision;
    std::cout << "Repeat the search for a different set/query file? (Type Y/N): ";
    std::cin >> decision;

    if (decision == 'Y' || decision == 'y') {
        return main(argc, argv);  // Recursive call to main (consider looping instead for large iterations)
    }
    else if(decision == 'N' || decision == 'n'){
        return 0;
    }

}
