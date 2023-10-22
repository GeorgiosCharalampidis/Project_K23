#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include "main_helper.h"


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