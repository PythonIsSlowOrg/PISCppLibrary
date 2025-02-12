#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <array>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <numeric>

template <typename T, int R, int C>
class matrix {
public:
    std::array<T, R*C> vals;

    matrix() {}
    
    int rows() const {
        return R;
    }
    
    int cols() const {
        return C;
    }

    T& get(int i, int j) {
        if (i < 0 || i >= R || j < 0 || j >= C)
            throw std::out_of_range("Index out of bounds");
        return vals[i * C + j];
    }

    const T& get(int i, int j) const {
        if (i < 0 || i >= R || j < 0 || j >= C)
            throw std::out_of_range("Index out of bounds");
        return vals[i * C + j];
    }

    std::array<T, C> getRow(int i) const {
        if (i < 0 || i >= R)
            throw std::out_of_range("Row index out of bounds");
        std::array<T, C> row;
        std::copy_n(vals.begin() + i * C, C, row.begin());
        return row;
    }

    std::array<T, R> getCol(int j) const {
        if (j < 0 || j >= C)
            throw std::out_of_range("Column index out of bounds");
        std::array<T, R> col;
        for (int i = 0; i < R; ++i)
            col[i] = vals[i * C + j];
        return col;
    }

    matrix operator*(const matrix& other) const {
        static_assert(C == R, "Matrix dimensions must match for multiplication");
        matrix result;
        for (int i = 0; i < R; ++i)
            for (int j = 0; j < C; ++j) {
                T sum = T{};
                for (int k = 0; k < C; ++k)
                    sum += get(i, k) * other.get(k, j);
                result.get(i, j) = sum;
            }
        return result;
    }

    matrix operator*(const T& scalar) const {
        matrix result;
        std::transform(vals.begin(), vals.end(), result.vals.begin(),
                      [scalar](const T& val) { return val * scalar; });
        return result;
    }

    matrix& operator*=(const matrix& other) {
        *this = *this * other;
        return *this;
    }

    matrix& operator*=(const T& scalar) {
        std::transform(vals.begin(), vals.end(), vals.begin(),
                      [scalar](T& val) { return val * scalar; });
        return *this;
    }

    matrix operator+(const matrix& other) const {
        matrix result;
        std::transform(vals.begin(), vals.end(), other.vals.begin(),
                      result.vals.begin(), std::plus<T>());
        return result;
    }

    matrix operator+(const T& scalar) const {
        matrix result;
        std::transform(vals.begin(), vals.end(), result.vals.begin(),
                      [scalar](const T& val) { return val + scalar; });
        return result;
    }

    matrix& operator+=(const matrix& other) {
        std::transform(vals.begin(), vals.end(), other.vals.begin(),
                      vals.begin(), std::plus<T>());
        return *this;
    }

    matrix& operator+=(const T& scalar) {
        std::transform(vals.begin(), vals.end(), vals.begin(),
                      [scalar](T& val) { return val + scalar; });
        return *this;
    }

    matrix& operator++() {
        unit();
        return *this;
    }

    matrix operator-(const matrix& other) const {
        matrix result;
        std::transform(vals.begin(), vals.end(), other.vals.begin(),
                      result.vals.begin(), std::minus<T>());
        return result;
    }

    matrix operator-(const T& scalar) const {
        matrix result;
        std::transform(vals.begin(), vals.end(), result.vals.begin(),
                      [scalar](const T& val) { return val - scalar; });
        return result;
    }

    matrix& operator-=(const matrix& other) {
        std::transform(vals.begin(), vals.end(), other.vals.begin(),
                      vals.begin(), std::minus<T>());
        return *this;
    }

    matrix& operator-=(const T& scalar) {
        std::transform(vals.begin(), vals.end(), vals.begin(),
                      [scalar](T& val) { return val - scalar; });
        return *this;
    }

    matrix& operator--() {
        unit();
        *this *= T{-1};
        return *this;
    }

    matrix transpose() const {
        matrix<T, C, R> result;
        for (int i = 0; i < R; ++i)
            for (int j = 0; j < C; ++j)
                result.get(j, i) = get(i, j);
        return result;
    }

    T det() const {
        static_assert(R == C, "Matrix must be square for determinant");
        if constexpr (R == 1) {
            return vals[0];
        } else if constexpr (R == 2) {
            return get(0,0) * get(1,1) - get(0,1) * get(1,0);
        } else {
            T result = T{};
            for (int j = 0; j < C; ++j) {
                result += get(0,j) * cof().get(0,j);
            }
            return result;
        }
    }

    matrix cof() const {
        static_assert(R == C, "Matrix must be square for cofactor");
        matrix result;
        for (int i = 0; i < R; ++i)
            for (int j = 0; j < C; ++j) {
                matrix<T, R-1, C-1> minor;
                int minor_i = 0;
                for (int r = 0; r < R; ++r) {
                    if (r == i) continue;
                    int minor_j = 0;
                    for (int c = 0; c < C; ++c) {
                        if (c == j) continue;
                        minor.get(minor_i, minor_j) = get(r, c);
                        ++minor_j;
                    }
                    ++minor_i;
                }
                result.get(i,j) = ((i + j) % 2 == 0 ? 1 : -1) * minor.det();
            }
        return result;
    }

    matrix adj() const {
        return cof().transpose();
    }

    matrix inverse() const {
        static_assert(R == C, "Matrix must be square for inverse");
        T d = det();
        if (d == T{})
            throw std::runtime_error("Matrix is not invertible");
        return adj() * (T{1} / d);
    }

    matrix circ(const matrix& other) const {
        matrix result;
        std::transform(vals.begin(), vals.end(), other.vals.begin(),
                      result.vals.begin(), std::multiplies<T>());
        return result;
    }

    T mean() const {
        return std::accumulate(vals.begin(), vals.end(), T{}) / (R * C);
    }

    T meanRow(int i) const {
        return std::accumulate(vals.begin() + i * C, vals.begin() + (i + 1) * C, T{}) / C;
    }

    T meanCol(int j) const {
        T sum = T{};
        for (int i = 0; i < R; ++i)
            sum += get(i, j);
        return sum / R;
    }

    std::array<T, R> meanRows() const {
        std::array<T, R> means;
        for (int i = 0; i < R; ++i)
            means[i] = meanRow(i);
        return means;
    }

    std::array<T, C> meanCols() const {
        std::array<T, C> means;
        for (int j = 0; j < C; ++j)
            means[j] = meanCol(j);
        return means;
    }

    int rank() const {
        matrix temp = *this;
        int rank = 0;
        std::array<bool, R> row_used;
        row_used.fill(false);

        for (int j = 0; j < C; ++j) {
            int i;
            for (i = 0; i < R; ++i) {
                if (!row_used[i] && std::abs(temp.get(i, j)) > T{1e-10})
                    break;
            }
            
            if (i != R) {
                ++rank;
                row_used[i] = true;
                for (int p = 0; p < R; ++p) {
                    if (p != i && std::abs(temp.get(p, j)) > T{1e-10}) {
                        T factor = temp.get(p, j) / temp.get(i, j);
                        for (int k = 0; k < C; ++k)
                            temp.get(p, k) -= factor * temp.get(i, k);
                    }
                }
            }
        }
        
        return rank;
    }

    matrix& unit() {
        static_assert(R == C, "Matrix must be square for unit matrix");
        zero();
        for (int i = 0; i < R; ++i)
            get(i, i) = T{1};
        return *this;
    }

    matrix& zero() {
        std::fill(vals.begin(), vals.end(), T{});
        return *this;
    }

    matrix pow(int n) {
        static_assert(R == C, "Matrix must be square for power operation");
        if (n == 0) {
            matrix result;
            return result.unit();
        }
        if (n < 0) {
            return inverse().pow(-n);
        }
        if (n == 1) {
            return *this;
        }
        if (n % 2 == 0) {
            matrix half = pow(n/2);
            return half * half;
        }
        return *this * pow(n-1);
    }

    bool inRow(const std::array<T, C>& vec) const {
        matrix aug(R, C+1);
        for (int i = 0; i < R; ++i)
            for (int j = 0; j < C; ++j)
                aug.get(i,j) = get(i,j);
        for (int i = 0; i < R; ++i)
            aug.get(i,C) = vec[i];
        return aug.rank() == rank();
    }

    bool inCol(const std::array<T, R>& vec) const {
        return transpose().inRow(vec);
    }

    bool inNull(const std::array<T, C>& vec, T epsilon) const {
        matrix result = *this * matrix<T,C,1>{vec};
        return std::all_of(result.vals.begin(), result.vals.end(),
                          [epsilon](const T& val) { return std::abs(val) < epsilon; });
    }

    bool inLeftNull(const std::array<T, R>& vec, T epsilon) const {
        return transpose().inNull(vec, epsilon);
    }
};

#endif