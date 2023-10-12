#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <random>
#include <cmath>
#include "minst.h"

// Συνάρτηση για υπολογισμό της ευκλείδειας απόστασης (L2) μεταξύ δύο διανυσμάτων
double euclideanDistance(const std::vector<unsigned char>& dataset, const std::vector<unsigned char>& query_set) {

    if (dataset.size() != query_set.size()) {
        throw std::runtime_error("Vectors must have the same dimension for L2 distance calculation.");
    }

    //Για κάθε εικόνα στο dataset και query set θα υπολογίζουμε την ευκλείδεια απόσταση μεταξύ τους
    //Το vec1

    double distance = 0.0;
    for (size_t i = 0; i < dataset.size(); ++i) {
        double diff = static_cast<double>(dataset[i]) - static_cast<double>(query_set[i]);
        distance += diff * diff;
    }

    return std::sqrt(distance);
}


// Υλοποίηση της LSH συνάρτηση
class LSH {
public:
    LSH(int num_tables, int num_functions, int num_dimensions, double radius);

    void buildIndex(const std::vector<std::vector<unsigned char>>& dataset);
    std::vector<int> query(const std::vector<unsigned char>& query_point);

private:
    int num_tables;
    int num_functions;
    int num_dimensions;
    double radius;

    std::vector<std::vector<std::vector<int>>> hash_tables;

    // Υλοποιήστε τις LSH συναρτήσεις κατακερματισμού και hash functions.

    int hash(const std::vector<unsigned char>& data_point, int table_index);
};

LSH::LSH(int num_tables, int num_functions, int num_dimensions, double radius) {
    this->num_tables = num_tables;
    this->num_functions = num_functions;
    this->num_dimensions = num_dimensions;
    this->radius = radius;

    // Δημιουργούμε τους πίνακες κατακερματισμού και αρχικοποιούμε τα hash tables.
    this->hash_tables.resize(num_tables);
    for (int i = 0; i < num_tables; ++i) {
        this->hash_tables[i].resize(1 << num_functions);
    }
}

void LSH::buildIndex(const std::vector<std::vector<unsigned char>>& dataset) {
    // Εδώ μπορείτε να υλοποιήσετε τη διαδικασία δημιουργίας του LSH για το dataset.
    // Περιλαμβάνει τη δημιουργία των πινάκων κατακερματισμού, των hash functions και την εισαγωγή των δεδομένων στους πίνακες.

    // Εδώ πρέπει να υλοποιήσετε τον υπολογισμό των hash values για κάθε διάνυσμα του dataset,
    // και να τα αποθηκεύσετε στους κατάλληλους πίνακες κατακερματισμού και τα hash tables.
    // Χρησιμοποιήστε τυχαίες συναρτήσεις κατακερματισμού και hash functions.

    // ...
}

std::vector<int> LSH::query(const std::vector<unsigned char>& query_point) {
    // Εδώ μπορείτε να υλοποιήσετε τη διαδικασία αναζήτησης του κοντινότερου γείτονα στο query_point.

    // Χρησιμοποιήστε τους πίνακες κατακερματισμού και τις hash functions που έχετε δημιουργήσει.
    // Βρείτε το hash value του ερωτηματικού διανύσματος και αναζητήστε τα κοντινότερα διανύσματα.

}