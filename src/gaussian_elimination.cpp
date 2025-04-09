#include "gaussian_elimination.h" // Use .h
#include <cmath>                 // For std::abs
#include <iostream>              // For std::cerr
#include <limits>                // For std::numeric_limits
#include <optional>              // For return type
#include <Eigen/Dense>           // For Eigen types

std::optional<Eigen::VectorXd> gaussian_elimination_solver(Eigen::MatrixXd A) { // Note: A is a copy of the augmented matrix [A|b]

    // --- Setup & Validation ---
    const long long rows = A.rows();
    const long long cols = A.cols();
    // Epsilon for floating point comparisons (relative to double precision)
    const double epsilon = std::numeric_limits<double>::epsilon() * 100;

    // Validate matrix dimensions (should be N x N+1)
    if (rows == 0 || cols != rows + 1) {
        std::cerr << "Error: Invalid matrix dimensions (" << rows << "x" << cols << "). Must be N x (N+1)." << std::endl;
        return std::nullopt; // Return empty optional indicating failure
    }

    // --- Forward Elimination (Gaussian elimination main loop) ---
    for (long long k = 0; k < rows; ++k) {

        // --- Pivoting ---
        // Find pivot row (row with max absolute value in column k, at or below row k)
        long long pivot_row_idx = k;
        double max_abs_val = std::abs(A(k, k));
        for (long long i = k + 1; i < rows; ++i) {
            if (std::abs(A(i, k)) > max_abs_val) {
                max_abs_val = std::abs(A(i, k));
                pivot_row_idx = i;
            }
        }

        // Check for potential singularity (or ill-conditioned matrix)
        if (max_abs_val < epsilon) {
             std::cerr << "Warning: Matrix is likely singular or ill-conditioned (zero pivot found in column " << k << ")." << std::endl;
             // Depending on requirements, might continue or return error. Returning error for unique solution.
             return std::nullopt;
        }

        // Swap current row k with the pivot row if necessary
        if (pivot_row_idx != k) {
            A.row(k).swap(A.row(pivot_row_idx));
        }

        // --- Elimination Step ---
        // Get the pivot element (A[k,k]) after potential swap
        double pivot_element = A(k, k);

        // Check if pivot element is near zero after swapping (should not happen if max_abs_val check passed, but good for safety)
         if (std::abs(pivot_element) < epsilon) {
              std::cerr << "Error: Zero pivot encountered at (" << k << "," << k << ") after pivoting. Matrix is singular." << std::endl;
             return std::nullopt;
         }

        // Eliminate elements below the pivot in column k
        for (long long i = k + 1; i < rows; ++i) {
            double factor = A(i, k) / pivot_element;
            // Eigen's vectorized row operation
            A.row(i) -= factor * A.row(k);
            // Force the element to zero for numerical stability
             A(i, k) = 0.0;
        }
    } // End of Forward Elimination loop

    // --- Back Substitution ---
    Eigen::VectorXd x(rows); // Initialize solution vector
    for (long long i = rows - 1; i >= 0; --i) {
        // Get the diagonal element A[i,i]
        double diag_element = A(i, i);

        // Check for zero on diagonal during back substitution
        if (std::abs(diag_element) < epsilon) {
             std::cerr << "Error: Zero pivot encountered during back substitution at (" << i << "," << i << "). No unique solution." << std::endl;

             // Check for consistency (0 = 0 means infinite solutions, 0 = non-zero means no solution)
              bool all_coeffs_zero = true;
              for(long long j = i; j < rows; ++j) { // Check coefficients A[i,j] for j >= i
                  if (std::abs(A(i, j)) >= epsilon) {
                      all_coeffs_zero = false;
                      break;
                  }
              }
              // Check the right-hand side (augmented part) A[i, rows]
              if (all_coeffs_zero && std::abs(A(i, rows)) < epsilon) { // 0 = 0 case
                   std::cerr << "Info: System has infinitely many solutions (dependent equations found)." << std::endl;
              } else { // 0 = non-zero case (or non-zero coeffs with zero diagonal)
                   std::cerr << "Info: System has no solutions (inconsistent equations found)." << std::endl;
              }
             return std::nullopt; // No unique solution exists
         }

        // Calculate sum of A[i,j] * x[j] for j > i (already computed x[j] values)
        double sum = 0.0;
        for (long long j = i + 1; j < rows; ++j) {
            sum += A(i, j) * x(j);
        }

        // Calculate x[i] = ( b[i] - sum ) / A[i,i]
        // Note: b[i] corresponds to A(i, rows) in the augmented matrix
        x(i) = (A(i, rows) - sum) / diag_element;
    } // End of Back Substitution loop

    // --- Return Solution ---
    return x; // Return the calculated solution vector
}