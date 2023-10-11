#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

// Συνάρτηση που αντιστρέφει τη σειρά των bytes σε έναν ακέραιο (για τη μορφή του αρχείου MNIST)
auto reverseInt = [](int i) {
    unsigned char c1, c2, c3, c4;
    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;
    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
};

// Συνάρτηση που διαβάζει τα δεδομένα των εικόνων MNIST από ένα αρχείο
std::vector<std::vector<unsigned char>> read_mnist_images(std::string full_path, int& number_of_images, int& image_size) {
    std::ifstream file(full_path, std::ios::binary);

    if (file.is_open()) {
        int magic_number = 0, n_rows = 0, n_cols = 0;

        // Διαβάζουμε τα metadata του αρχείου MNIST
        file.read(reinterpret_cast<char*>(&magic_number), sizeof(magic_number));
        magic_number = reverseInt(magic_number);

        if (magic_number != 2051)
            throw std::runtime_error("Not valid MNIST file!");

        file.read(reinterpret_cast<char*>(&number_of_images), sizeof(number_of_images));
        number_of_images = reverseInt(number_of_images);
        file.read(reinterpret_cast<char*>(&n_rows), sizeof(n_rows));
        n_rows = reverseInt(n_rows);
        file.read(reinterpret_cast<char*>(&n_cols), sizeof(n_cols));
        n_cols = reverseInt(n_cols);

        image_size = n_rows * n_cols;

        // Δημιουργούμε ένα vector για το dataset των εικόνων
        std::vector<std::vector<unsigned char>> dataset(number_of_images, std::vector<unsigned char>(image_size));

        // Διαβάζουμε τα δεδομένα κάθε εικόνας
        for (int i = 0; i < number_of_images; i++) {
            file.read(reinterpret_cast<char*>(&dataset[i][0]), image_size);
        }
        return dataset;
    } else {
        throw std::runtime_error("Could not open file `" + full_path + "`!");
    }
}

// Συνάρτηση που εκτυπώνει μια εικόνα από το dataset
void print_image(const std::vector<unsigned char>& image, int width, int height) {
    // Εκτυπώνουμε την εικόνα στην οθόνη
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            std::cout << static_cast<int>(image[i * width + j]) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    int number_of_images, image_size;

    // Προφανώς η τοποθεσία του αρχείου θα πρέπει να αλλάξει ανάλογα με τον υπολογιστή σας
    std::string mnist_file = R"(C:\Users\test\CLionProjects\ProjectEm\input.dat)";

    // Διαβάζουμε τα δεδομένα των εικόνων MNIST από το αρχείο
    std::vector<std::vector<unsigned char>> dataset = read_mnist_images(mnist_file, number_of_images, image_size);

    // Εκτυπώνουμε όλες τις εικόνες στο dataset
    // Ντελαπόνγκο , στο input.dat υπάρχουν 60000 εικόνες , με κάθε εικόνα να έχει 784 pixels
    // Οπότε πρακτικά έχουμε ένα vector 60000 εικόνων , με κάθε διάνυσμα στο vector να έχει 784 διαστάσεις

    /*
    for (int i = 0; i < number_of_images; i++) {
        std::cout << "Image " << i + 1 << ":" << std::endl;
        print_image(dataset[i], 28, 28); // Οι διαστάσεις 28x28 αντιστοιχούν στο MNIST
    }
    */

    std::cout << "Hello World!" << std::endl;
    return 0;
}
