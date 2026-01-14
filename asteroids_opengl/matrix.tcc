#include <cassert>

// Implementations for SquareMatrix declared in matrix.h

template <class FLOAT, size_t N>
SquareMatrix<FLOAT, N>::SquareMatrix(std::initializer_list< Vector<FLOAT, N> > values) {
  size_t i = 0;
  if (values.size() == 0) {
    for (; i < N; ++i) matrix[i] = Vector<FLOAT, N>();
    return;
  }

  auto it = values.begin();
  for (; i < values.size() && i < N; ++i, ++it) {
    matrix[i] = *it;
  }
  if (i < N) {
    Vector<FLOAT, N> last = *(values.end() - 1);
    for (; i < N; ++i) matrix[i] = last;
  }
}

// returns reference to the i-th column vector
template <class FLOAT, size_t N>
Vector<FLOAT, N> & SquareMatrix<FLOAT, N>::operator[](std::size_t i) {
  assert(i < N);
  return matrix[i];
}

// returns i-th column vector
template <class FLOAT, size_t N>
Vector<FLOAT, N> SquareMatrix<FLOAT, N>::operator[](std::size_t i) const {
  assert(i < N);
  return matrix[i];
}

// returns the value at the given row and column
template <class FLOAT, size_t N>
FLOAT SquareMatrix<FLOAT, N>::at(size_t row, size_t column) const {
  assert(row < N && column < N);
  return matrix[column][row];
}

// returns the reference value at the given row and column
template <class FLOAT, size_t N>
FLOAT & SquareMatrix<FLOAT, N>::at(size_t row, size_t column) {
  assert(row < N && column < N);
  return matrix[column][row];
}

// returns the product of this SquareMatrix and the given vector
template <class FLOAT, size_t N>
Vector<FLOAT,N> SquareMatrix<FLOAT, N>::operator*(const Vector<FLOAT,N> vector) const {
  Vector<FLOAT, N> result;
  for (size_t col = 0; col < N; ++col) {
    result += vector[col] * matrix[col];
  }
  return result;
}

// returns the product of two square matrices (friend)
template <class F, size_t K>
SquareMatrix<F, K> operator*(const SquareMatrix<F, K> & factor1, const SquareMatrix<F, K> & factor2) {
  SquareMatrix<F, K> result{ };
  for (size_t col = 0; col < K; ++col) {
    result[col] = factor1 * factor2[col];
  }
  return result;
}
