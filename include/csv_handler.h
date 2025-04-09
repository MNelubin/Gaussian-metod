#ifndef CSV_HANDLER_H
#define CSV_HANDLER_H

#include <string>
#include <vector>
#include <Eigen/Dense> 


bool read_matrix_from_csv(const std::string& filename, Eigen::MatrixXd& matrix);

// for answer
bool write_vector_to_csv(const std::string& filename, const Eigen::VectorXd& vec);

// Function to write an Eigen Matrix to a CSV file
bool write_matrix_to_csv(const std::string& filename, const Eigen::MatrixXd& matrix);


#endif // CSV_HANDLER_H