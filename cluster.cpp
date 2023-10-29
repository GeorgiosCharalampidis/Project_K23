//
// Created by test on 10/26/2023.
//
///////////////////////////////////////////////////////
//                  Start of ./cluster               //
///////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "KMeansPLusPlus.h"
#include "mnist.h"

int main(int argc, char* argv[]) {
    std::string input, configPath, outputPath, method;
    bool complete = false;
    int number_of_clusters = 5;
    int number_of_vector_hash_tables = 3;
    int number_of_vector_hash_functions = 4;
    int max_number_M_hypercube = 10;
    int number_of_hypercube_dimensions = 7;
    int number_of_probes = 2;
    int number_of_images;
    int image_size;
    bool configProvided = false;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            input = argv[++i];
        } else if (arg == "-c" && i + 1 < argc) {
            configPath = argv[++i];
            configProvided = true;
        } else if (arg == "-o" && i + 1 < argc) {
            outputPath = argv[++i];
        } else if (arg == "-complete" &&  i + 1 < argc) {
            complete = true;
        } else if (arg == "-m" && i + 1 < argc) {
            method = argv[++i];
        }
    }

    // Read configuration file if provided
    if (configProvided) {
        std::ifstream configFile(configPath);
        if (configFile.is_open()) {
            std::string line;
            while (getline(configFile, line)) {
                size_t pos = line.find(":");
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    int value = stoi(line.substr(pos + 1));

                    if (key == "number_of_clusters") {
                        number_of_clusters = value;
                    } else if (key == "number_of_vector_hash_tables") {
                        number_of_vector_hash_tables = value;
                    } else if (key == "number_of_vector_hash_functions") {
                        number_of_vector_hash_functions = value;
                    } else if (key == "max_number_M_hypercube") {
                        max_number_M_hypercube = value;
                    } else if (key == "number_of_hypercube_dimensions") {
                        number_of_hypercube_dimensions = value;
                    } else if (key == "number_of_probes") {
                        number_of_probes = value;
                    }
                }
            }
            configFile.close();
        } else {
            std::cerr << "Unable to open configuration file." << std::endl;
            return 1;
        }
    } else {
        std::cout << "Proceeding with default configuration values.." << std::endl;
    }


    std::vector<std::vector<unsigned char>> dataset = read_mnist_images(input, number_of_images, image_size);

    std::vector<std::vector<unsigned char>> test_set(dataset.begin(), dataset.begin() + 5000);

    // Depending on the method:
    if (method == "Classic") {
        KMeansPlusPlus plus(test_set, number_of_clusters, "Lloyds", number_of_vector_hash_functions, number_of_vector_hash_tables,
                            number_of_hypercube_dimensions, max_number_M_hypercube, number_of_probes, complete);

    } else if (method == "LSH") {

        KMeansPlusPlus plus(test_set, number_of_clusters, "LSH", number_of_vector_hash_functions, number_of_vector_hash_tables,
                            number_of_hypercube_dimensions, max_number_M_hypercube, number_of_probes, complete);
    } else if (method == "HyperCube") {
        KMeansPlusPlus plus(test_set, number_of_clusters, "HyperCube", number_of_vector_hash_functions, number_of_vector_hash_tables,
                            number_of_hypercube_dimensions, max_number_M_hypercube, number_of_probes, complete);    }
    else {
        std::cerr << "Invalid method specified." << std::endl;
        return 1;
    }

    return 0;
}
