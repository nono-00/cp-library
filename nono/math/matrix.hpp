#pragma once

#include <cassert>
#include <initializer_list>
#include <iostream>
#include <vector>

namespace nono {

///  # Matrix
///  行列積, 行列累乗くらいの機能しかない.
template <typename T>
class Matrix {
  public:
    Matrix() = default;
    Matrix(int h, int w): h_(h), w_(w), data_(h, std::vector<T>(w)) {}
    Matrix(int h, int w, T init): h_(h), w_(w), data_(h, std::vector<T>(w, init)) {}
    explicit Matrix(const std::vector<std::vector<T>>& data): h_(data.size()), w_(data.front().size()), data_(data) {
        for (int i = 0; i < h_; i++) {
            assert(static_cast<int>(data_[i].size()) == w_);
        }
    }
    Matrix(std::initializer_list<std::vector<T>> init): data_(init.begin(), init.end()) {
        h_ = data_.size();
        w_ = data_.front().size();
        for (int i = 0; i < h_; i++) {
            assert(static_cast<int>(data_[i].size()) == w_);
        }
    }

    const std::vector<T>& operator[](const int pos) const {
        assert(0 <= pos && pos < h_);
        return data_[pos];
    }

    std::vector<T>& operator[](const int pos) {
        assert(0 <= pos && pos < h_);
        return data_[pos];
    }

    ///  # operator += scalar
    ///  data[i][j] <- data[i][j] + rhs
    ///  O(nm)
    Matrix& operator+=(const T rhs) {
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
                data_[i][j] += rhs;
            }
        }
        return *this;
    }

    ///  # operator -= scalar
    ///  data[i][j] <- data[i][j] - rhs
    ///  O(nm)
    Matrix& operator-=(const T rhs) {
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
                data_[i][j] -= rhs;
            }
        }
        return *this;
    }

    ///  # operator *= scalar
    ///  data[i][j] <- data[i][j] * rhs
    ///  O(nm)
    Matrix& operator*=(const T rhs) {
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
                data_[i][j] *= rhs;
            }
        }
        return *this;
    }

    ///  # operator /= scalar
    ///  data[i][j] <- data[i][j] / rhs
    ///  O(nm)
    Matrix& operator/=(const T rhs) {
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
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
    friend Matrix operator*(const Matrix& lhs, const T rhs) {
        return Matrix(lhs) *= rhs;
    }
    friend Matrix operator*(const T lhs, const Matrix& rhs) {
        return Matrix(rhs) *= lhs;
    }
    friend Matrix operator/(const Matrix& lhs, const T rhs) {
        return Matrix(lhs) /= rhs;
    }
    friend bool operator==(const Matrix& lhs, const Matrix& rhs) {
        return lhs.data_ == rhs.data_;
    }
    friend bool operator!=(const Matrix& lhs, const Matrix& rhs) {
        return !(lhs.data_ == rhs.data_);
    }
    friend std::ostream& operator<<(std::ostream& os, const Matrix& mat) {
        for (int i = 0; i < mat.h_; i++) {
            for (int j = 0; j < mat.w_; j++) {
                os << mat[i][j] << " \n"[j + 1 == mat.w_];
            }
        }
        return os;
    }

    ///  # operator+= matrix
    ///  lhs[i][j] <- lhs[i][j] + rhs[i][j]
    ///  O(nm)
    Matrix& operator+=(const Matrix& rhs) {
        assert(h_ == rhs.h_ && w_ == rhs.w_);
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
                data_[i][j] += rhs.data_[i][j];
            }
        }
        return *this;
    }

    ///  # operator-= matrix
    ///  lhs[i][j] <- lhs[i][j] - rhs[i][j]
    ///  O(nm)
    Matrix& operator-=(const Matrix& rhs) {
        assert(h_ == rhs.h_ && w_ == rhs.w_);
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
                data_[i][j] -= rhs.data_[i][j];
            }
        }
        return *this;
    }

    ///  # operator*= matrix
    ///  lhs[i][j] <- sum[for k in [0, w)](lhs[i][k] * rhs[k][j])
    ///  O(nml)
    Matrix& operator*=(const Matrix& rhs) {
        assert(w_ == rhs.h_);
        std::vector<std::vector<T>> mat(h_, std::vector<T>(rhs.w_));
        for (int i = 0; i < h_; i++) {
            for (int k = 0; k < w_; k++) {
                for (int j = 0; j < rhs.w_; j++) {
                    mat[i][j] += data_[i][k] * rhs.data_[k][j];
                }
            }
        }
        data_ = std::move(mat);
        w_ = rhs.w_;
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

    ///  # pow(exp)
    ///  return (data) ** exp
    ///  O(n^3 log exp)
    [[nodiscard]] Matrix pow(long long exp) const {
        assert(h_ == w_);
        Matrix result(h_, w_);
        Matrix base(*this);
        for (int i = 0; i < h_; i++) {
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

    ///  # rotate()
    ///  多分時計周り
    ///  O(nm)
    [[nodiscard]] Matrix rotate() const {
        std::vector<std::vector<T>> result(w_, std::vector<T>(h_));
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
                result[j][h_ - i - 1] = data_[i][j];
            }
        }
        return Matrix(result);
    }

    ///  # transpose()
    ///  転置
    ///  O(nm)
    [[nodiscard]] Matrix transpose() const {
        std::vector<std::vector<T>> result(w_, std::vector<T>(h_));
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
                result[j][i] = data_[i][j];
            }
        }
        return Matrix(result);
    }

    ///  # row()
    int row() const {
        return h_;
    }

    ///  # column()
    int column() const {
        return w_;
    }

  private:
    int h_, w_;
    std::vector<std::vector<T>> data_;
};

}  //  namespace nono
