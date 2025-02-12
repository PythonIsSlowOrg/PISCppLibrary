#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <numeric>

template <typename T>
class matrix {
public:
    int rows;
    int cols;
    std::vector<T> vals;

    matrix(int _rows, int _cols) : rows(_rows), cols(_cols) {
        vals.resize(cols * rows, T{});
    }

    T& get(int i, int j) {
        if (i < 0 || i >= rows || j < 0 || j >= cols)
            throw std::out_of_range("Index out of bounds");
        return vals[i * cols + j];
    }

    const T& get(int i, int j) const {
        if (i < 0 || i >= rows || j < 0 || j >= cols)
            throw std::out_of_range("Index out of bounds");
        return vals[i * cols + j];
    }

    std::vector<T> getRow(int i) const {
        if (i < 0 || i >= rows)
            throw std::out_of_range("Row index out of bounds");
        return std::vector<T>(vals.begin() + i * cols, vals.begin() + (i + 1) * cols);
    }

    std::vector<T> getCol(int j) const {
        if (j < 0 || j >= cols)
            throw std::out_of_range("Column index out of bounds");
        std::vector<T> col(rows);
        for (int i = 0; i < rows; ++i)
            col[i] = get(i, j);
        return col;
    }

    matrix operator*(const matrix& other) const {
        if (cols != other.rows)
            throw std::invalid_argument("Invalid matrix dimensions for multiplication");
        
        matrix result(rows, other.cols);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < other.cols; ++j)
                for (int k = 0; k < cols; ++k)
                    result.get(i, j) += get(i, k) * other.get(k, j);
        return result;
    }

    matrix operator*(const T& scalar) const {
        matrix result = *this;
        for (auto& val : result.vals)
            val *= scalar;
        return result;
    }

    matrix& operator*=(const matrix& other) {
        *this = *this * other;
        return *this;
    }

    matrix& operator*=(const T& scalar) {
        for (auto& val : vals)
            val *= scalar;
        return *this;
    }

    matrix operator+(const matrix& other) const {
        if (rows != other.rows || cols != other.cols)
            throw std::invalid_argument("Matrix dimensions must match for addition");
        
        matrix result = *this;
        for (size_t i = 0; i < vals.size(); ++i)
            result.vals[i] += other.vals[i];
        return result;
    }

    matrix operator+(const T& scalar) const {
        matrix result = *this;
        for (auto& val : result.vals)
            val += scalar;
        return result;
    }

    matrix& operator+=(const matrix& other) {
        if (rows != other.rows || cols != other.cols)
            throw std::invalid_argument("Matrix dimensions must match for addition");
        
        for (size_t i = 0; i < vals.size(); ++i)
            vals[i] += other.vals[i];
        return *this;
    }

    matrix& operator+=(const T& scalar) {
        for (auto& val : vals)
            val += scalar;
        return *this;
    }

    matrix& operator++() {
        if (rows != cols)
            throw std::invalid_argument("Matrix must be square for unit addition");
        
        for (int i = 0; i < rows; ++i)
            get(i, i) += T{1};
        return *this;
    }

    matrix operator-(const matrix& other) const {
        if (rows != other.rows || cols != other.cols)
            throw std::invalid_argument("Matrix dimensions must match for subtraction");
        
        matrix result = *this;
        for (size_t i = 0; i < vals.size(); ++i)
            result.vals[i] -= other.vals[i];
        return result;
    }

    matrix operator-(const T& scalar) const {
        matrix result = *this;
        for (auto& val : result.vals)
            val -= scalar;
        return result;
    }

    matrix& operator-=(const matrix& other) {
        if (rows != other.rows || cols != other.cols)
            throw std::invalid_argument("Matrix dimensions must match for subtraction");
        
        for (size_t i = 0; i < vals.size(); ++i)
            vals[i] -= other.vals[i];
        return *this;
    }

    matrix& operator-=(const T& scalar) {
        for (auto& val : vals)
            val -= scalar;
        return *this;
    }

    matrix& operator--() {
        if (rows != cols)
            throw std::invalid_argument("Matrix must be square for unit subtraction");
        
        for (int i = 0; i < rows; ++i)
            get(i, i) -= T{1};
        return *this;
    }

    matrix transpose() const {
        matrix result(cols, rows);
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                result.get(j, i) = get(i, j);
        return result;
    }

    T det() const {
        if (rows != cols)
            throw std::invalid_argument("Matrix must be square for determinant");

        if (rows == 1) return get(0, 0);
        if (rows == 2) return get(0, 0) * get(1, 1) - get(0, 1) * get(1, 0);

        T determinant = T{};
        for (int j = 0; j < cols; ++j) {
            matrix<T> submatrix(rows - 1, cols - 1);
            for (int i = 1; i < rows; ++i)
                for (int k = 0, l = 0; k < cols; ++k)
                    if (k != j)
                        submatrix.get(i - 1, l++) = get(i, k);
            
            determinant += (j % 2 == 0 ? 1 : -1) * get(0, j) * submatrix.det();
        }
        return determinant;
    }

    matrix cof() const {
        if (rows != cols)
            throw std::invalid_argument("Matrix must be square for cofactor matrix");

        matrix result(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                matrix<T> submatrix(rows - 1, cols - 1);
                for (int k = 0, m = 0; k < rows; ++k) {
                    if (k == i) continue;
                    for (int l = 0, n = 0; l < cols; ++l) {
                        if (l == j) continue;
                        submatrix.get(m, n++) = get(k, l);
                    }
                    ++m;
                }
                result.get(i, j) = ((i + j) % 2 == 0 ? 1 : -1) * submatrix.det();
            }
        }
        return result;
    }

    matrix adj() const {
        return cof().transpose();
    }

    matrix inverse() const {
        T determinant = det();
        if (std::abs(determinant) < 1e-10)
            throw std::runtime_error("Matrix is singular");
        
        return adj() * (T{1} / determinant);
    }

    matrix circ(const matrix& other) const {
        if (rows != other.rows || cols != other.cols)
            throw std::invalid_argument("Matrix dimensions must match for element-wise multiplication");
        
        matrix result(rows, cols);
        for (size_t i = 0; i < vals.size(); ++i)
            result.vals[i] = vals[i] * other.vals[i];
        return result;
    }

    T mean() const {
        if (vals.empty()) return T{};
        return std::accumulate(vals.begin(), vals.end(), T{}) / T(vals.size());
    }

    T meanRow(int i) const {
        auto row = getRow(i);
        return std::accumulate(row.begin(), row.end(), T{}) / T(cols);
    }

    T meanCol(int j) const {
        auto col = getCol(j);
        return std::accumulate(col.begin(), col.end(), T{}) / T(rows);
    }

    std::vector<T> meanRows() const {
        std::vector<T> means(rows);
        for (int i = 0; i < rows; ++i)
            means[i] = meanRow(i);
        return means;
    }

    std::vector<T> meanCols() const {
        std::vector<T> means(cols);
        for (int j = 0; j < cols; ++j)
            means[j] = meanCol(j);
        return means;
    }

    std::vector<std::vector<T>> rowSpace() const {

        matrix temp = *this;
        return gaussianElimination(temp).first;
    }

    std::vector<std::vector<T>> colSpace() const {

        return transpose().rowSpace();
    }

    std::vector<std::vector<T>> null() const {

        matrix temp = *this;
        auto [reduced, pivot_cols] = gaussianElimination(temp);
        
        std::vector<std::vector<T>> basis;
        std::vector<bool> is_pivot(cols, false);
        for (int col : pivot_cols) is_pivot[col] = true;

        for (int j = 0; j < cols; ++j) {
            if (is_pivot[j]) continue;
            
            std::vector<T> basis_vector(cols, T{});
            basis_vector[j] = T{1};
            
            for (size_t i = 0; i < pivot_cols.size(); ++i) {
                int pivot_col = pivot_cols[i];
                basis_vector[pivot_col] = -reduced[i][j];
            }
            
            basis.push_back(basis_vector);
        }
        
        return basis;
    }

    std::vector<std::vector<T>> leftNull() const {

        return transpose().null();
    }

    int rank() const {
        matrix temp = *this;
        return gaussianElimination(temp).first.size();
    }

    matrix& unit() {
        if (rows != cols)
            throw std::invalid_argument("Matrix must be square for unit matrix");
        
        std::fill(vals.begin(), vals.end(), T{});
        for (int i = 0; i < rows; ++i)
            get(i, i) = T{1};
        return *this;
    }

    matrix& zero() {
        std::fill(vals.begin(), vals.end(), T{});
        return *this;
    }

    matrix power(int n) {
        if (rows != cols)
            throw std::invalid_argument("Matrix must be square for power operation");

        if (n == 0) {
            matrix result = *this;
            return result.unit();
        }

        if (n < 0) {
            matrix result = inverse();
            return result.power(-n);
        }

        matrix result = *this;
        matrix temp = *this;
        --n;
        
        while (n > 0) {
            if (n % 2 == 1)
                result *= temp;
            if (n > 1)
                temp *= temp;
            n /= 2;
        }
        
        return result;
    }

    std::pair<std::vector<std::vector<T>>, std::vector<int>> gaussianElimination(matrix& m) const {
        std::vector<std::vector<T>> result;
        std::vector<int> pivot_cols;
        
        int pivot_row = 0;
        for (int j = 0; j < m.cols && pivot_row < m.rows; ++j) {
            int pivot = pivot_row;
            while (pivot < m.rows && std::abs(m.get(pivot, j)) < 1e-10)
                ++pivot;
            
            if (pivot < m.rows) {
                if (pivot != pivot_row) {
                    for (int k = 0; k < m.cols; ++k)
                        std::swap(m.get(pivot, k), m.get(pivot_row, k));
                }
                
                T pivot_val = m.get(pivot_row, j);
                for (int k = 0; k < m.cols; ++k)
                    m.get(pivot_row, k) /= pivot_val;
                
                for (int i = 0; i < m.rows; ++i) {
                    if (i != pivot_row) {
                        T factor = m.get(i, j);
                        for (int k = 0; k < m.cols; ++k)
                            m.get(i, k) -= factor * m.get(pivot_row, k);
                    }
                }
                
                result.push_back(m.getRow(pivot_row));
                pivot_cols.push_back(j);
                ++pivot_row;
            }
        }
        
        return {result, pivot_cols};
    }
};

#endif