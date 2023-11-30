#pragma once

#include <cassert>
#include <initializer_list>
#include <vector>

namespace nono {

template <typename T>
class Matrix {
  public:
    Matrix() = default;
    Matrix(int row, int column): row_(row), column_(column), data_(row, std::vector<T>(column)) {}
    Matrix(int row, int column, T elem): row_(row), column_(column), data_(row, std::vector<T>(column, elem)) {}
    explicit Matrix(const std::vector<std::vector<T>>& data)
        : row_(data.size()),
          column_(data.front().size()),
          data_(data) {
        for (int i = 0; i < row_; i++) {
            assert(static_cast<int>(data_[i].size()) == column_);
        }
    }
    Matrix(std::initializer_list<std::vector<T>> init): data_(init.begin(), init.end()) {
        row_ = data_.size();
        column_ = data_.front().size();
        for (int i = 0; i < row_; i++) {
            assert(static_cast<int>(data_[i].size()) == column_);
        }
    }

    const std::vector<T>& operator[](const int pos) const {
        assert(0 <= pos && pos < row_);
        return data_[pos];
    }

    std::vector<T>& operator[](const int pos) {
        assert(0 <= pos && pos < row_);
        return data_[pos];
    }

    //  スカラー演算
    Matrix& operator+=(const T rhs) {
        for (int i = 0; i < row_; i++) {
            for (int j = 0; j < column_; j++) {
                data_[i][j] += rhs;
            }
        }
        return *this;
    }

    Matrix& operator-=(const T rhs) {
        for (int i = 0; i < row_; i++) {
            for (int j = 0; j < column_; j++) {
                data_[i][j] -= rhs;
            }
        }
        return *this;
    }

    Matrix& operator*=(const T rhs) {
        for (int i = 0; i < row_; i++) {
            for (int j = 0; j < column_; j++) {
                data_[i][j] *= rhs;
            }
        }
        return *this;
    }

    Matrix& operator/=(const T rhs) {
        for (int i = 0; i < row_; i++) {
            for (int j = 0; j < column_; j++) {
                data_[i][j] /= rhs;
            }
        }
        return *this;
    }

    friend Matrix operator+(const Matrix& lhs, const T rhs) {
        return Matrix(lhs) += rhs;
    }
    friend Matrix operator+(const T lhs, const Matrix& rhs) {
        return Matrix(rhs) += lhs;
    }
    friend Matrix operator-(const Matrix& lhs, const T rhs) {
        return Matrix(lhs) -= rhs;
    }
    friend Matrix operator-(const T lhs, const Matrix& rhs) {
        return Matrix(rhs) -= lhs;
    }
    friend Matrix operator*(const Matrix& lhs, const T rhs) {
        return Matrix(lhs) *= rhs;
    }
    friend Matrix operator*(const T lhs, const Matrix& rhs) {
        return Matrix(rhs) *= lhs;
    }
    friend Matrix operator/(const Matrix& lhs, const T rhs) {
        return Matrix(lhs) /= rhs;
    }
    friend Matrix operator/(const T lhs, const Matrix& rhs) {
        return Matrix(rhs) /= lhs;
    }

    //  行列演算
    Matrix& operator+=(const Matrix& rhs) {
        assert(row_ == rhs.row_ && column_ == rhs.column_);
        for (int i = 0; i < row_; i++) {
            for (int j = 0; j < column_; j++) {
                data_[i][j] += rhs.data_[i][j];
            }
        }
        return *this;
    }

    Matrix& operator-=(const Matrix& rhs) {
        assert(row_ == rhs.row_ && column_ == rhs.column_);
        for (int i = 0; i < row_; i++) {
            for (int j = 0; j < column_; j++) {
                data_[i][j] -= rhs.data_[i][j];
            }
        }
        return *this;
    }

    Matrix& operator*=(const Matrix& rhs) {
        assert(column_ == rhs.row_);
        std::vector<std::vector<T>> mat(row_, std::vector<T>(rhs.column_));
        for (int i = 0; i < row_; i++) {
            for (int k = 0; k < column_; k++) {
                for (int j = 0; j < rhs.column_; j++) {
                    mat[i][j] += data_[i][k] * rhs.data_[k][j];
                }
            }
        }
        data_ = std::move(mat);
        column_ = rhs.column_;
        return *this;
    }

    friend Matrix operator+(const Matrix& lhs, const Matrix& rhs) {
        return Matrix(lhs) += rhs;
    }
    friend Matrix operator-(const Matrix& lhs, const Matrix& rhs) {
        return Matrix(lhs) -= rhs;
    }
    friend Matrix operator*(const Matrix& lhs, const Matrix& rhs) {
        return Matrix(lhs) *= rhs;
    }

    [[nodiscard]] Matrix pow(long long exp) const {
        assert(row_ == column_);
        Matrix result(row_, column_);
        Matrix base(*this);
        for (int i = 0; i < row_; i++) {
            result[i][i] = static_cast<T>(1);
        }
        while (exp > 0) {
            if (exp & 1) {
                result *= base;
            }
            base *= base;
            exp >>= 1;
        }
        return result;
    }

    [[nodiscard]] Matrix rotate() {
        std::vector<std::vector<T>> result(column_, std::vector<T>(row_));
        for (int i = 0; i < row_; i++) {
            for (int j = 0; j < column_; j++) {
                result[j][row_ - i - 1] = data_[i][j];
            }
        }
        return Matrix(result);
    }

    [[nodiscard]] Matrix transpose() {
        std::vector<std::vector<T>> result(column_, std::vector<T>(row_));
        for (int i = 0; i < row_; i++) {
            for (int j = 0; j < column_; j++) {
                result[j][i] = data_[i][j];
            }
        }
        return Matrix(result);
    }

    int row() {
        return row_;
    }

    int column() {
        return column_;
    }

  private:
    int row_, column_;
    std::vector<std::vector<T>> data_;
};

}  //  namespace nono
