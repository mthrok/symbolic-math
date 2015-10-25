#include "symbol.hpp"
#include "gtest/gtest.h"

INITIALIZE_EASYLOGGINGPP


TEST(Scalar, ConstInitialization) {
  Symbol::Scalar c0(0);
  Symbol::Scalar c1(1);
  Symbol::Scalar c2(2);

  EXPECT_EQ(c0, 0);
  EXPECT_EQ(c1, 1);
  EXPECT_EQ(c2, 2);

  EXPECT_NE(c0, 2);
  EXPECT_NE(c1, 0);
  EXPECT_NE(c2, 1);
}

TEST(Scalar, VariableInitialization) {
  Symbol::Scalar x("x");
  Symbol::Scalar y("y");

  EXPECT_EQ(x, std::string("x"));
  EXPECT_EQ(y, std::string("y"));

  EXPECT_NE(x, std::string("y"));
  EXPECT_NE(y, std::string("x"));
}

TEST(Scalar, ConstantEquality) {
  Symbol::Scalar zero(0);
  Symbol::Scalar one(1);
  Symbol::Scalar two(2);

  EXPECT_EQ(zero, std::to_string((double)0));
  EXPECT_EQ(one,  std::to_string((double)1));
  EXPECT_EQ(two,  std::to_string((double)2));

  EXPECT_NE(zero, std::to_string((double)1));
  EXPECT_NE(one,  std::to_string((double)2));
  EXPECT_NE(two,  std::to_string((double)0));

  EXPECT_EQ(zero, zero);
  EXPECT_EQ(one, one);
  EXPECT_EQ(two, two);

  EXPECT_NE(zero, one);
  EXPECT_NE(one, two);
  EXPECT_NE(two, zero);
}

TEST(Scalar, VariableEquality) {
  Symbol::Scalar x("x");
  Symbol::Scalar y1("y");
  Symbol::Scalar y2("y");

  EXPECT_EQ(x, x);

  EXPECT_NE(x, y1);

  EXPECT_EQ(y1, y2);
}

TEST(Scalar, VariableNegation) {
  Symbol::Scalar x("x");
  Symbol::Scalar y = -x;
  Symbol::Scalar z = -x;

  EXPECT_NE(x, y);

  EXPECT_EQ(y, z);

  EXPECT_EQ(x, -y);

  EXPECT_EQ(-x, y);
}

TEST(Scalar, ConstantAddition) {
  Symbol::Scalar zero(0);
  Symbol::Scalar one(1);
  Symbol::Scalar two(2);

  EXPECT_EQ(one + one, two);

  EXPECT_EQ(one + one, two + zero);

  EXPECT_EQ(one + one + zero, two + zero);
}

TEST(Scalar, VariableAddition) {
  Symbol::Scalar x("x");
  Symbol::Scalar y("y");
  Symbol::Scalar z = x + y;

  EXPECT_EQ(x + y, y + x);
  EXPECT_EQ(z, y + x);
  EXPECT_EQ(z, x + y);
  EXPECT_EQ(z, z);

  EXPECT_NE(x + y, z + x);
  EXPECT_NE(z, x);
}
