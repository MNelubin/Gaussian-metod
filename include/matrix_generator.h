#ifndef MATRIX_GENERATOR_H
#define MATRIX_GENERATOR_H

#include <Eigen/Dense>
#include <random> // Для генератора
#include <stdexcept> // Для invalid_argument

// Генерирует расширенную матрицу [A|b] заданного размера
Eigen::MatrixXd generate_augmented_matrix(int size,
                                          unsigned int seed,
                                          double min_val = -10.0,
                                          double max_val = 10.0);

#endif // MATRIX_GENERATOR_H    