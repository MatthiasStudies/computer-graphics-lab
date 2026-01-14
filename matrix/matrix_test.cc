#include "matrix.h"
#include "gtest/gtest.h"

namespace {
	
TEST(MATRIX, ListInitialization2df) {
  SquareMatrix2df matrix = { Vector2df{1.0, 0.0},
                             Vector2df{0.0, 1.0} };
  
  EXPECT_NEAR(1.0, matrix.at(0,0), 0.00001);
  EXPECT_NEAR(0.0, matrix.at(0,1), 0.00001);
  EXPECT_NEAR(0.0, matrix.at(1,0), 0.00001);
  EXPECT_NEAR(1.0, matrix.at(1,1), 0.00001);
}

TEST(MATRIX, ListInitialization3df) {
  SquareMatrix3df matrix = { Vector3df{1.0, 0.0, 0.0},
                             Vector3df{0.0, 1.0, 0.0},
                             Vector3df{0.0, 0.0, 1.0} };
                               
  EXPECT_NEAR(1.0, matrix[0][0], 0.00001);
  EXPECT_NEAR(0.0, matrix[0][1], 0.00001);
  EXPECT_NEAR(0.0, matrix[0][2], 0.00001);
  EXPECT_NEAR(0.0, matrix[1][0], 0.00001);
  EXPECT_NEAR(1.0, matrix[1][1], 0.00001);
  EXPECT_NEAR(0.0, matrix[1][2], 0.00001);
  EXPECT_NEAR(0.0, matrix[2][0], 0.00001);
  EXPECT_NEAR(0.0, matrix[2][1], 0.00001);
  EXPECT_NEAR(1.0, matrix[2][2], 0.00001);
}

TEST(MATRIX, ListInitialization4df) {
  SquareMatrix4df matrix = { {1.0, 5.0, 9.0, 13.0},
                             {2.0, 6.0, 10.0, 14.0},
                             {3.0, 7.0, 11.0, 15.0},
                             {4.0, 8.0, 12.0, 16.0} };
  
  float v = 1.0f;
  for (size_t row = 0; row < 4; row++) {
    for (size_t column = 0;  column < 4; column++) {    
      EXPECT_NEAR(v++, matrix.at(row, column), 0.00001);
    }
  }
}



TEST(MATRIX, ProductWithVector3df) {
  SquareMatrix3df matrix = { {1.0, 0.0, 1.0},
                             {0.0, 1.0, 1.0},
                             {0.0, 0.0, 1.0} };
  Vector3df vector = {-6.0, 3.0,  1.0};
  Vector3df product = matrix * vector;  
  
  EXPECT_NEAR(-6.0, product[0], 0.00001);
  EXPECT_NEAR( 3.0, product[1], 0.00001);
  EXPECT_NEAR(-2.0, product[2], 0.00001);
}



TEST(MATRIX, ProductWithMatrix2df) {
  SquareMatrix2df matrix1 = { {1.0, 2.0},
                              {-1.0, 1.5} };
  SquareMatrix2df matrix2 = { {2.0, -1.0},
                              {1.0, 0.0} };
  SquareMatrix2df matrix = matrix1 * matrix2;

  EXPECT_NEAR( 3.0, matrix.at(0,0), 0.00001);
  EXPECT_NEAR( 1.0, matrix.at(0,1), 0.00001);
  EXPECT_NEAR( 2.5, matrix.at(1,0), 0.00001);
  EXPECT_NEAR( 2.0, matrix.at(1,1), 0.00001);
}


// --- Additional tests added below ---

TEST(MATRIX, EmptyInitializerList) {
  const float eps = 1e-6f;
  SquareMatrix2df m = {};
  EXPECT_NEAR(0.0f, m.at(0,0), eps);
  EXPECT_NEAR(0.0f, m.at(0,1), eps);
  EXPECT_NEAR(0.0f, m.at(1,0), eps);
  EXPECT_NEAR(0.0f, m.at(1,1), eps);
}

TEST(MATRIX, ShortInitializerRepeatsLast) {
  const float eps = 1e-6f;
  SquareMatrix3df m = { Vector3df{1.0f,2.0f,3.0f}, Vector3df{4.0f,5.0f,6.0f} };
  // columns: col0={1,2,3}, col1={4,5,6}, col2 should repeat last={4,5,6}
  EXPECT_NEAR(1.0f, m.at(0,0), eps);
  EXPECT_NEAR(2.0f, m.at(1,0), eps);
  EXPECT_NEAR(3.0f, m.at(2,0), eps);
  EXPECT_NEAR(4.0f, m.at(0,1), eps);
  EXPECT_NEAR(5.0f, m.at(1,1), eps);
  EXPECT_NEAR(6.0f, m.at(2,1), eps);
  EXPECT_NEAR(4.0f, m.at(0,2), eps);
  EXPECT_NEAR(5.0f, m.at(1,2), eps);
  EXPECT_NEAR(6.0f, m.at(2,2), eps);
}

TEST(MATRIX, MutableAccessAndOperatorIndex) {
  const float eps = 1e-6f;
  SquareMatrix3df m = { {1.0f,0.0f,0.0f}, {0.0f,1.0f,0.0f}, {0.0f,0.0f,1.0f} };
  m.at(0,1) = 7.5f;
  EXPECT_NEAR(7.5f, m.at(0,1), eps);
  m[2][0] = -3.0f;
  EXPECT_NEAR(-3.0f, m.at(0,2), eps);
}

TEST(MATRIX, ConstMatrixMultiplyVector) {
  const float eps = 1e-6f;
  const SquareMatrix3df m = { {1.0f,0.0f,0.0f}, {0.0f,2.0f,0.0f}, {0.0f,0.0f,3.0f} };
  Vector3df v = {1.0f, -1.0f, 2.0f};
  Vector3df r = m * v;
  EXPECT_NEAR(1.0f, r[0], eps);
  EXPECT_NEAR(-2.0f, r[1], eps);
  EXPECT_NEAR(6.0f, r[2], eps);
}

TEST(MATRIX, MatrixIdentityMultiplication) {
  const float eps = 1e-6f;
  SquareMatrix3df id = { {1.0f,0.0f,0.0f}, {0.0f,1.0f,0.0f}, {0.0f,0.0f,1.0f} };
  SquareMatrix3df m = { {2.0f,3.0f,4.0f}, {5.0f,6.0f,7.0f}, {8.0f,9.0f,10.0f} };
  SquareMatrix3df product = id * m;
  for (size_t row = 0; row < 3; ++row) {
    for (size_t col = 0; col < 3; ++col) {
      EXPECT_NEAR(m.at(row,col), product.at(row,col), eps);
    }
  }
}

} // namespace