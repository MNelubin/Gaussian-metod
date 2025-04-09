
#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include <iomanip>

#include "csv_handler.h"
#include "gaussian_elimination.h"
#include "matrix_generator.h"
#include <Eigen/Dense>

#ifndef PROJECT_NAME
#define PROJECT_NAME "GAUS_app"
#endif

void print_usage() {
    std::cerr << "Usage:\n"
              << "  " << PROJECT_NAME << " --file <input_csv_file> <output_solution_csv>\n"
              << "  " << PROJECT_NAME << " --generate <size> <seed> <output_matrix_csv> [min_val] [max_val]\n" // Clarified output file purpose
              << std::endl;
}


int main(int argc, char* argv[]) {
    if (argc < 4) {
        print_usage();
        return 1;
    }

    std::string mode = argv[1];
    std::string output_file;
    Eigen::MatrixXd augmented_matrix;
    // bool matrix_loaded = false; // Not strictly needed anymore with new structure

    try {
        if (mode == "--file" && argc == 4) {
            std::string input_file = argv[2];
            output_file = argv[3]; // This file will store the SOLUTION
            std::cout << "Reading matrix from file: " << input_file << std::endl;
            if (!read_matrix_from_csv(input_file, augmented_matrix)) {
                return 1; // Error message already printed by function
            }

            // --- Proceed to solve ---
            std::cout << "Solving the system (" << augmented_matrix.rows() << "x" << augmented_matrix.rows() <<")..." << std::endl;
            auto start_time = std::chrono::high_resolution_clock::now();
            std::optional<Eigen::VectorXd> solution = gaussian_elimination_solver(augmented_matrix);
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            std::cout << "Solver finished in " << duration.count() << " ms." << std::endl;

            if (solution) {
                std::cout << "Solution found." << std::endl;
                std::cout << "Writing solution to " << output_file << std::endl;
                if (!write_vector_to_csv(output_file, *solution)) {
                    return 1; // Error message already printed
                }
                std::cout << "Done." << std::endl;
                return 0; // Success for --file mode
            } else {
                std::cerr << "Failed to find a unique solution." << std::endl;
                return 1; // Failure for --file mode
            }
            // --- End of --file mode ---

        } else if (mode == "--generate" && argc >= 5) {
            int size;
            unsigned int seed;
            double min_val = -10.0;
            double max_val = 10.0;

            size = std::stoi(argv[2]);
            seed = std::stoul(argv[3]);
            output_file = argv[4]; // This file will store the GENERATED MATRIX
            if (argc >= 6) min_val = std::stod(argv[5]);
            if (argc >= 7) max_val = std::stod(argv[6]);

             if(size <= 0) {
                 std::cerr << "Error: Matrix size must be positive." << std::endl;
                 print_usage();
                 return 1;
            }
             if (min_val >= max_val) {
                 std::cerr << "Error: min_val must be less than max_val for generation." << std::endl;
                 print_usage();
                 return 1;
             }

            std::cout << "Generating a " << size << "x" << size << " system with seed " << seed << std::endl;
            augmented_matrix = generate_augmented_matrix(size, seed, min_val, max_val);
            std::cout << "Generation complete." << std::endl;

            // --- Save the generated matrix ---
            std::cout << "Writing generated matrix to " << output_file << std::endl;
            if (!write_matrix_to_csv(output_file, augmented_matrix)) {
                 // Error message already printed by function
                 return 1; // Indicate failure
            }
            std::cout << "Generated matrix successfully saved to " << output_file << std::endl;
            return 0; // Success for --generate mode (matrix saved, no solving)
            // --- End of --generate mode ---

        } else {
            print_usage();
            return 1;
        }

    } catch (const std::invalid_argument& e) {
        std::cerr << "Error processing arguments or generating matrix: " << e.what() << std::endl;
        print_usage();
        return 1;
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Argument value out of range: " << e.what() << std::endl;
        print_usage();
        return 1;
     } catch (const std::exception& e) {
        std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
        return 1;
    } catch (...) {
         std::cerr << "An unknown critical error occurred." << std::endl;
         return 1;
     }

    // Should not be reached with current logic, but good practice
    return 1;
}