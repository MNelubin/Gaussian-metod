#ifndef GAUSSIAN_ELIMINATION_H
#define GAUSSIAN_ELIMINATION_H

#include <Eigen/Dense>
#include <optional> // Для возврата решения или индикации ошибки

// Функция решает систему линейных уравнений Ax = b методом Гаусса
std::optional<Eigen::VectorXd> gaussian_elimination_solver(Eigen::MatrixXd augmented_matrix);

#endif // GAUSSIAN_ELIMINATION_H