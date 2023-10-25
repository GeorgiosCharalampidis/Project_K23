///////////////////////////////////////////////////////
//                  Start of ./lsh                   //
///////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include "main_helper.h"
#include "KMeansPLusPlus.h"
#include "mnist.h"
#include "lsh_class.h"
#include "Hypercube.h"
#include "global_functions.h"
#include <chrono>
//#include <cstdlib>  // ~For system() maybe we will use it, maybe we will not

int main(int argc, char* argv[]) 
{

    int number_of_images, image_size;

    // Create vector to store dataset
    std::string input = R"(C:\Users\test\CLionProjects\Project_K23\input.dat)";
    std::string query = R"(C:\Users\test\CLionProjects\Project_K23\query.dat)";
    std::string outputPath = R"(C:\Users\test\CLionProjects\Project_K23\output.dat)";

    std::vector<std::vector<unsigned char>> dataset = read_mnist_images(input, number_of_images, image_size);
    std::vector<std::vector<unsigned char>> query_set = read_mnist_images(query, number_of_images,image_size);

    // Create a Test_Set with the first 100 images of the dataset
    std::vector<std::vector<unsigned char>> test_set(dataset.begin(), dataset.begin() + 10000);
    std::vector<std::vector<unsigned char>> dataset_small(dataset.begin(), dataset.begin() + 10000);

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

    /* 
    const char* command ; 
    std::cin >> command; //"command_to_run_another_executable arg1 arg2 arg3"; something like that as input
    int result;
    if(command = "./cube")
    {
        result = system(command); //runs ./cube
    }
    else if (command = "./lsh")
    {
        continue;
    }
    else break; //Goodbye

    if (result == 0) 
    {
        // The program executed successfully.
    } else {
        // There was an error executing the program.
    }
    
    //~this is a way to call another exec in this main, maybe we will use it, maybe we will noy
    */
   


    return 0;
   
}
