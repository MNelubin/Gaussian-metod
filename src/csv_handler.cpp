#include "csv_handler.h" // Use .h
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>    // For stod exceptions
#include <iomanip>      // Required for std::setprecision, std::fixed
#include <iterator>     // Required for std::distance
#include <lazycsv.hpp>  // Your lazycsv header
#include <limits> // Required for numeric_limits in matrix writing if used for precision


bool read_matrix_from_csv(const std::string& filename, Eigen::MatrixXd& matrix) {
    try {
        std::vector<std::vector<double>> data;
        // Explicitly state that the CSV has NO header
        lazycsv::parser<lazycsv::mmap_source, lazycsv::has_header<false>> parser{filename};
        // parser.skip_header(); // Uncomment if your CSV has a header row to ignore

        std::size_t expected_cols = 0;
        std::size_t row_index = 0; // Row counter for error messages

        for (const auto& row : parser) {
            row_index++;

            // --- WAY TO COUNT CELLS ---
            std::size_t current_cell_count = std::distance(row.begin(), row.end());

            // --- WAY TO CHECK IF ROW IS EMPTY ---
            if (current_cell_count == 0) {
                 continue;
            }

            // Check column consistency (only after first non-empty row is processed)
            if (expected_cols == 0 && current_cell_count > 0) { // First non-empty row sets expected count
                 expected_cols = current_cell_count;
            } else if (current_cell_count != expected_cols) {
                 std::cerr << "Error: Inconsistent number of columns on row " << row_index
                           << ". Expected " << expected_cols << ", got "
                           << current_cell_count << "." << std::endl; // Use the calculated count
                 return false;
            }

            // Process cells 
            std::vector<double> row_data;
            row_data.reserve(current_cell_count); // Optimize vector allocation

            for (const auto& cell : row) {
                 try {
                    std::string cell_str = std::string(cell.raw()); // Convert string_view to string
                    if (cell_str.empty()) {
                         std::cerr << "Error: Empty cell found in CSV on row " << row_index << "." << std::endl;
                         return false;
                    }
                    row_data.push_back(std::stod(cell_str));
                 } catch (const std::invalid_argument& e) {
                    std::cerr << "Error: Invalid number format in CSV on row " << row_index
                              << ", cell content: '" << cell.raw() << "'" << std::endl; // Use cell.raw()
                    return false;
                 } catch (const std::out_of_range& e) {
                    std::cerr << "Error: Number out of range in CSV on row " << row_index
                              << ", cell content: '" << cell.raw() << "'" << std::endl; // Use cell.raw()
                    return false;
                 }
            }
            // Only add if row_data was populated (it should be if current_cell_count > 0)
            if (!row_data.empty()) {
                 data.push_back(row_data);
            }
        }

        // Check if any valid data rows were read
         if (data.empty()) { // No need to check data[0] if data is empty
             std::cerr << "Error: No valid data rows found in CSV file '" << filename << "'." << std::endl;
             return false;
         }

        // Resize and fill Eigen matrix
         long long num_rows = data.size();
         long long num_cols = data[0].size(); // All rows have expected_cols == data[0].size() now
         matrix.resize(num_rows, num_cols);

         for (long long i = 0; i < num_rows; ++i) {
             for (long long j = 0; j < num_cols; ++j) {
                 matrix(i, j) = data[i][j];
             }
         }
         return true;

    } catch (const lazycsv::error& e) { // Catch specific lazycsv errors
         std::cerr << "LazyCSV Error reading file '" << filename << "': " << e.what() << std::endl;
         return false;
    } catch (const std::exception& e) {
        std::cerr << "Standard Error reading CSV file '" << filename << "': " << e.what() << std::endl;
        return false;
    }
     catch (...) { // Catch any other unknown errors
         std::cerr << "An unknown error occurred while reading CSV file '" << filename << "'." << std::endl;
         return false;
     }
}

// The write_vector_to_csv function should remain unchanged and correct.
bool write_vector_to_csv(const std::string& filename, const Eigen::VectorXd& vec) {
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "' for writing." << std::endl;
        return false;
    }

    outfile << "Solution\n"; // Optional header
    outfile << std::fixed << std::setprecision(10); // From <iomanip>

    for (long long i = 0; i < vec.size(); ++i) {
        outfile << vec(i) << "\n";
    }

    outfile.close();
    return !outfile.fail();
}

bool write_matrix_to_csv(const std::string& filename, const Eigen::MatrixXd& matrix) {
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "' for writing." << std::endl;
        return false;
    }

    // --- ADD EMPTY LINE AT THE BEGINNING ---
    //outfile << "\n";

    // Set precision for floating-point numbers
    outfile << std::fixed << std::setprecision(15); // Use high precision

    // Iterate through rows and columns of the Eigen matrix
    for (long long i = 0; i < matrix.rows(); ++i) {
        for (long long j = 0; j < matrix.cols(); ++j) {
            outfile << matrix(i, j);
            // Add comma separator if not the last column
            if (j < matrix.cols() - 1) {
                outfile << ",";
            }
        }
        // Add newline character after each row
        outfile << "\n";
    }

    outfile.close();
    return !outfile.fail(); // Check for errors during close
}