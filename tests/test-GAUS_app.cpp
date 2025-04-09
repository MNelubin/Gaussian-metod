#include <limits> // Для numeric_limits
#include "gtest/gtest.h"

// Подключаем наши заголовки
#include "gaussian_elimination.h"
#include "matrix_generator.h"
#include "csv_handler.h" // Может понадобиться для тестов чтения/записи

#include <Eigen/Dense>
#include <optional>
#include <fstream> // Для файловых операций в тестах
#include <vector>

// Допуск для сравнения double в тестах
const double ABS_ERROR = std::numeric_limits<double>::epsilon() * 1000; // Немного больше, чем в солвере

// --- Тесты для gaussian_elimination ---

TEST(GaussianEliminationTest, Simple2x2) {
    Eigen::MatrixXd augmented_matrix(2, 3);
    augmented_matrix << 1, 2, 5,  // x + 2y = 5
                       3, 4, 11; // 3x + 4y = 11
    Eigen::VectorXd expected_solution(2);
    expected_solution << 1, 2; // x=1, y=2

    std::optional<Eigen::VectorXd> solution = gaussian_elimination_solver(augmented_matrix);

    ASSERT_TRUE(solution.has_value());
    ASSERT_EQ(solution->size(), 2);
    ASSERT_NEAR((*solution)(0), expected_solution(0), ABS_ERROR);
    ASSERT_NEAR((*solution)(1), expected_solution(1), ABS_ERROR);
}

TEST(GaussianEliminationTest, Basic3x3) {
    Eigen::MatrixXd augmented_matrix(3, 4);
    augmented_matrix << 2, 1, -1, 8,    // 2x + y - z = 8
                       -3, -1, 2, -11,  // -3x - y + 2z = -11
                       -2, 1, 2, -3;   // -2x + y + 2z = -3
    Eigen::VectorXd expected_solution(3);
    expected_solution << 2, 3, -1; // x=2, y=3, z=-1

    std::optional<Eigen::VectorXd> solution = gaussian_elimination_solver(augmented_matrix);

    ASSERT_TRUE(solution.has_value());
    ASSERT_EQ(solution->size(), 3);
    ASSERT_NEAR((*solution)(0), expected_solution(0), ABS_ERROR);
    ASSERT_NEAR((*solution)(1), expected_solution(1), ABS_ERROR);
    ASSERT_NEAR((*solution)(2), expected_solution(2), ABS_ERROR);
}

TEST(GaussianEliminationTest, SingularMatrixNoSolution) {
    Eigen::MatrixXd augmented_matrix(2, 3);
    augmented_matrix << 1, 1, 2,  // x + y = 2
                       2, 2, 5;  // 2x + 2y = 5 (inconsistent)

    std::optional<Eigen::VectorXd> solution = gaussian_elimination_solver(augmented_matrix);
    ASSERT_FALSE(solution.has_value());
}

TEST(GaussianEliminationTest, SingularMatrixInfiniteSolutions) {
     Eigen::MatrixXd augmented_matrix(2, 3);
     augmented_matrix << 1, 1, 2,  // x + y = 2
                        2, 2, 4;  // 2x + 2y = 4 (dependent)
     // Наша реализация возвращает nullopt при обнаружении нулевой строки в конце
     std::optional<Eigen::VectorXd> solution = gaussian_elimination_solver(augmented_matrix);
     ASSERT_FALSE(solution.has_value()); // Ожидаем nullopt, т.к. нет *единственного* решения
}


TEST(GaussianEliminationTest, RowSwapNeeded) {
    Eigen::MatrixXd augmented_matrix(2, 3);
     augmented_matrix << 0, 1, 2,  // 0x + y = 2  => y = 2
                        1, 1, 3;  // x + y = 3  => x = 1
    Eigen::VectorXd expected_solution(2);
    expected_solution << 1, 2;

    std::optional<Eigen::VectorXd> solution = gaussian_elimination_solver(augmented_matrix);

     ASSERT_TRUE(solution.has_value());
     ASSERT_EQ(solution->size(), 2);
     ASSERT_NEAR((*solution)(0), expected_solution(0), ABS_ERROR);
     ASSERT_NEAR((*solution)(1), expected_solution(1), ABS_ERROR);
}

// --- Тесты для matrix_generator ---

TEST(MatrixGeneratorTest, GeneratesCorrectSize) {
    int size = 5;
    unsigned int seed = 123;
    Eigen::MatrixXd mat = generate_augmented_matrix(size, seed);
    ASSERT_EQ(mat.rows(), size);
    ASSERT_EQ(mat.cols(), size + 1);
}

TEST(MatrixGeneratorTest, ReproducibleWithSeed) {
    int size = 3;
    unsigned int seed = 456;
    Eigen::MatrixXd mat1 = generate_augmented_matrix(size, seed);
    Eigen::MatrixXd mat2 = generate_augmented_matrix(size, seed);
    ASSERT_EQ(mat1.rows(), mat2.rows());
    ASSERT_EQ(mat1.cols(), mat2.cols());
    // Проверяем поэлементно (сравнение матриц напрямую может быть неточным из-за double)
    for (int i=0; i<mat1.rows(); ++i) {
        for (int j=0; j<mat1.cols(); ++j) {
            ASSERT_DOUBLE_EQ(mat1(i,j), mat2(i,j)); // Точное сравнение для воспроизводимости
        }
    }
}

TEST(MatrixGeneratorTest, ThrowsOnInvalidArgs) {
     ASSERT_THROW(generate_augmented_matrix(0, 123), std::invalid_argument);
     ASSERT_THROW(generate_augmented_matrix(-5, 123), std::invalid_argument);
     ASSERT_THROW(generate_augmented_matrix(5, 123, 10.0, 5.0), std::invalid_argument); // min >= max
}

// --- Тесты для csv_handler (пример) ---

// Fixture для создания временных файлов
class CsvHandlerTest : public ::testing::Test {
protected:
    const std::string test_input_filename = "test_input.csv";
    const std::string test_output_filename = "test_output.csv";

    void TearDown() override {
        // Удаляем файлы после каждого теста
        std::remove(test_input_filename.c_str());
        std::remove(test_output_filename.c_str());
    }

    void create_test_csv(const std::vector<std::vector<double>>& data) {
        std::ofstream outfile(test_input_filename);
        outfile << std::fixed << std::setprecision(10);
        // Опционально: добавить заголовок? "A,B,C,Sol"
        for (const auto& row : data) {
            for (size_t j = 0; j < row.size(); ++j) {
                outfile << row[j] << (j == row.size() - 1 ? "" : ",");
            }
            outfile << "\n";
        }
        outfile.close();
    }
};

TEST_F(CsvHandlerTest, ReadValidCsv) {
    std::vector<std::vector<double>> data = {{1.1, 2.2, 3.3}, {4.4, 5.5, 6.6}};
    create_test_csv(data);

    Eigen::MatrixXd matrix;
    ASSERT_TRUE(read_matrix_from_csv(test_input_filename, matrix));

    ASSERT_EQ(matrix.rows(), 2);
    ASSERT_EQ(matrix.cols(), 3);
    ASSERT_NEAR(matrix(0, 0), 1.1, ABS_ERROR);
    ASSERT_NEAR(matrix(0, 1), 2.2, ABS_ERROR);
    ASSERT_NEAR(matrix(0, 2), 3.3, ABS_ERROR);
    ASSERT_NEAR(matrix(1, 0), 4.4, ABS_ERROR);
    ASSERT_NEAR(matrix(1, 1), 5.5, ABS_ERROR);
    ASSERT_NEAR(matrix(1, 2), 6.6, ABS_ERROR);
}

TEST_F(CsvHandlerTest, ReadCsvWithInconsistentColumns) {
    std::ofstream outfile(test_input_filename);
    outfile << "1,2,3\n4,5\n"; // Несовместимые колонки
    outfile.close();

    Eigen::MatrixXd matrix;
    ASSERT_FALSE(read_matrix_from_csv(test_input_filename, matrix));
}

TEST_F(CsvHandlerTest, ReadCsvWithInvalidNumber) {
    std::ofstream outfile(test_input_filename);
    outfile << "1,abc,3\n4,5,6\n"; // Не число
    outfile.close();

    Eigen::MatrixXd matrix;
    ASSERT_FALSE(read_matrix_from_csv(test_input_filename, matrix));
}

TEST_F(CsvHandlerTest, WriteVectorToCsv) {
    Eigen::VectorXd vec(3);
    vec << 1.23456789, -0.5, 100.0;

    ASSERT_TRUE(write_vector_to_csv(test_output_filename, vec));

    // Проверяем содержимое файла
    std::ifstream infile(test_output_filename);
    ASSERT_TRUE(infile.is_open());
    std::string line;
    std::vector<double> read_values;

    // Пропускаем заголовок
    std::getline(infile, line);
    ASSERT_EQ(line, "Solution");

    while(std::getline(infile, line)) {
         try {
             read_values.push_back(std::stod(line));
         } catch(...) {
             FAIL() << "Could not parse value from output file: " << line;
         }
    }
    infile.close();

    ASSERT_EQ(read_values.size(), vec.size());
    ASSERT_NEAR(read_values[0], vec(0), ABS_ERROR);
    ASSERT_NEAR(read_values[1], vec(1), ABS_ERROR);
    ASSERT_NEAR(read_values[2], vec(2), ABS_ERROR);
}