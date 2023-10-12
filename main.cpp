#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "minst.h"
#include "lsh.h"




int main() {
    int number_of_images, image_size;

    // Η άσκηση ζητάει τα paths κατά την εκτέλεση του προγράμματος, που έχω υλοποιηθεί παρακάτω
    // Για λόγους ταχύτητας και ευκολίας στο debugging, δίνω καρφωτά τις τοποθεσίες

    /*

    std::string input;
    std::cout << R"(Enter the path to the MNIST file (e.g., C:\path\to\index.dat): )";
    std::cin >> input;

    // create vector to store dataset
    std::vector<std::vector<unsigned char>> dataset = read_mnist_images(input, number_of_images, image_size);

    std::cout << std::endl;

    std::string query;
    std::cout << R"(Enter the path to the MNIST_test file (e.g., C:\path\to\query.dat): )";
    std::cin >> query;

    std::cout << std::endl;

    std::string outputPath;
    std::cout << R"(Enter the path to the MNIST_test file (e.g., C:\path\to\output.dat): )";
    std::cin >> outputPath;


    // Create vector to store query_set
    std::vector<std::vector<unsigned char>> query_set = read_mnist_images(query, number_of_images,image_size);

    // Create an output file stream with the user-specified file path
    std::ofstream outputFile(outputPath, std::ios::binary);

    // Close the file
    outputFile.close();

    */


    // Δίνω καρφωτά τις τοποθεσίες των input.dat και query.dat
    // Δημιουργώ αντίστοιχα τα dataset και query_set

    std::string input = R"(C:\Users\test\CLionProjects\Project_K23\input.dat)";
    std::string query = R"(C:\Users\test\CLionProjects\Project_K23\query.dat)";
    std::string outputPath = R"(C:\Users\test\CLionProjects\Project_K23\output.dat)";

    std::vector<std::vector<unsigned char>> dataset = read_mnist_images(input, number_of_images, image_size);
    std::vector<std::vector<unsigned char>> query_set = read_mnist_images(query, number_of_images,image_size);

    std::ofstream outputFile(outputPath, std::ios::binary);

    // Close the file
    outputFile.close();


    // Εκτυπώνουμε τις εικόνες των δύο datasets για testing

    /*

    for (int i = 0; i < 10; i++) {
        std::cout << "Image " << i + 1 << " of dataset:" << std::endl;
        print_image(dataset[i], 28, 28); // Οι διαστάσεις 28x28 αντιστοιχούν στο MNIST
        std::cout << "Image " << i + 1 << " of query_set:" << std::endl;
        print_image(query_set[i], 28, 28); // Οι διαστάσεις 28x28 αντιστοιχούν στο MNIST
    }

    */



    return 0;
}
