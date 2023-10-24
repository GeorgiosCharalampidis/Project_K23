//
// Created by test on 10/24/2023.
//

#ifndef PROJECTEM_HASHFUNCTION_H
#define PROJECTEM_HASHFUNCTION_H

#include <vector>

class HashFunction {
private:
    std::vector<double> v;
    double t;
    int num_dimensions;

public:
    HashFunction(int nd, double w_range_low, double w_range_high);
    int computeHash(const std::vector<unsigned char>& data_point, double w) const;
};



#endif //PROJECTEM_HASHFUNCTION_H
