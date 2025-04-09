#include "matrix_generator.h"

Eigen::MatrixXd generate_augmented_matrix(int size,
                                          unsigned int seed,
                                          double min_val,
                                          double max_val)
{
    if (size <= 0) {
        throw std::invalid_argument("Matrix size must be positive.");
    }
     if (min_val >= max_val) {
         throw std::invalid_argument("min_val must be less than max_val.");
     }

    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> distrib(min_val, max_val);

    Eigen::MatrixXd augmented_matrix(size, size + 1);

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size + 1; ++j) {
            augmented_matrix(i, j) = distrib(gen);
        }
    }

    return augmented_matrix;
}