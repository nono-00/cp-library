#pragma once

#include <cassert>
#include <initializer_list>
#include <iostream>
#include <vector>

namespace nono {

//  # Matrix
//  h行w列の行列。行列積, 行列累乗くらいの機能しかない.
//  スカラーとの四則演算、行列どうしの加減乗算、==, !=, 出力ができる
template <typename T>
class Matrix {
  public:
    Matrix() = default;
    Matrix(int h, int w): h_(h), w_(w), data_(h, std::vector<T>(w)) {}
    Matrix(int h, int w, T init): h_(h), w_(w), data_(h, std::vector<T>(w, init)) {}

    //  # Matrix(data)
    //  dataが空だと壊れる。各行の長さが揃っていないと壊れる
    explicit Matrix(const std::vector<std::vector<T>>& data): h_(data.size()), w_(data.front().size()), data_(data) {
        for (int i = 0; i < h_; i++) {
            assert(static_cast<int>(data_[i].size()) == w_);
        }
    }

    //  # Matrix(init)
    //  initが空だと壊れる。各行の長さが揃っていないと壊れる
    Matrix(std::initializer_list<std::vector<T>> init): data_(init.begin(), init.end()) {
        h_ = data_.size();
        w_ = data_.front().size();
        for (int i = 0; i < h_; i++) {
            assert(static_cast<int>(data_[i].size()) == w_);
        }
    }

    //  # operator[](pos)
    //  pos行目を返す
    //  0 <= pos < row() でないと壊れる
    const std::vector<T>& operator[](const int pos) const {
        assert(0 <= pos && pos < h_);
        return data_[pos];
    }

    std::vector<T>& operator[](const int pos) {
        assert(0 <= pos && pos < h_);
        return data_[pos];
    }

    //  # operator+=(rhs)
    //  スカラーを全要素に足す。data[i][j] <- data[i][j] + rhs
    //  O(hw)
    Matrix& operator+=(const T rhs) {
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
                data_[i][j] += rhs;
            }
        }
        return *this;
    }

    //  # operator-=(rhs)
    //  スカラーを全要素から引く。data[i][j] <- data[i][j] - rhs
    //  O(hw)
    Matrix& operator-=(const T rhs) {
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
                data_[i][j] -= rhs;
            }
        }
        return *this;
    }

    //  # operator*=(rhs)
    //  全要素をスカラー倍する。data[i][j] <- data[i][j] * rhs
    //  O(hw)
    Matrix& operator*=(const T rhs) {
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
                data_[i][j] *= rhs;
            }
        }
        return *this;
    }

    //  # operator/=(rhs)
    //  全要素をスカラーで割る。data[i][j] <- data[i][j] / rhs
    //  O(hw)
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

    //  # operator+=(rhs)
    //  行列を足す。lhs[i][j] <- lhs[i][j] + rhs[i][j]
    //  行数と列数が同じでないと壊れる
    //  O(hw)
    Matrix& operator+=(const Matrix& rhs) {
        assert(h_ == rhs.h_ && w_ == rhs.w_);
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
                data_[i][j] += rhs.data_[i][j];
            }
        }
        return *this;
    }

    //  # operator-=(rhs)
    //  行列を引く。lhs[i][j] <- lhs[i][j] - rhs[i][j]
    //  行数と列数が同じでないと壊れる
    //  O(hw)
    Matrix& operator-=(const Matrix& rhs) {
        assert(h_ == rhs.h_ && w_ == rhs.w_);
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
                data_[i][j] -= rhs.data_[i][j];
            }
        }
        return *this;
    }

    //  # operator*=(rhs)
    //  行列を右から掛ける。lhs[i][j] <- sum[for k in [0, w)](lhs[i][k] * rhs[k][j])
    //  lhsの列数とrhsの行数が同じでないと壊れる
    //  O(h * w * rhsの列数)
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

    //  # pow(exp)
    //  return (data) ** exp。exp <= 0 なら単位行列を返す
    //  正方行列でないと壊れる
    //  O(h^3 log exp)
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

    //  # rotate()
    //  時計回りに90度回した、w行h列の行列を返す
    //  O(hw)
    [[nodiscard]] Matrix rotate() const {
        std::vector<std::vector<T>> result(w_, std::vector<T>(h_));
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
                result[j][h_ - i - 1] = data_[i][j];
            }
        }
        return Matrix(result);
    }

    //  # transpose()
    //  転置
    //  O(hw)
    [[nodiscard]] Matrix transpose() const {
        std::vector<std::vector<T>> result(w_, std::vector<T>(h_));
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
                result[j][i] = data_[i][j];
            }
        }
        return Matrix(result);
    }

    //  # row()
    //  行数
    int row() const {
        return h_;
    }

    //  # column()
    //  列数
    int column() const {
        return w_;
    }

  private:
    int h_, w_;
    std::vector<std::vector<T>> data_;
};

}  //  namespace nono
