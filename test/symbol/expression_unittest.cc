#include "symbol.hpp"
#include "gtest/gtest.h"

INITIALIZE_EASYLOGGINGPP

TEST(Expression, ConstInitialization) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);

  Symbol::Expression n_zero(-0);
  Symbol::Expression n_one(-1);
  Symbol::Expression n_two(-2);

  EXPECT_EQ("0", zero);
  EXPECT_EQ("1", one);
  EXPECT_EQ("2", two);

  EXPECT_EQ("0", n_zero);
  EXPECT_EQ(" - 1", n_one);
  EXPECT_EQ(" - 2", n_two);

  EXPECT_EQ(zero, 0);
  EXPECT_EQ(one, 1);
  EXPECT_EQ(two, 2);

  EXPECT_EQ(n_zero, 0);
  EXPECT_EQ(n_one, -1);
  EXPECT_EQ(n_two, -2);

  EXPECT_NE(zero, 1);
  EXPECT_NE(one,  2);
  EXPECT_NE(two,  0);

  EXPECT_NE(n_zero, -1);
  EXPECT_NE(n_one,  1);
  EXPECT_NE(n_two,  2);
}

TEST(Expression, VariableInitialization) {
  Symbol::Expression x("x", 3);
  Symbol::Expression y("y", 5);

  EXPECT_EQ("x", x);
  EXPECT_EQ("y", y);

  EXPECT_NE("y", x);
  EXPECT_NE("x", y);
}

TEST(Expression, ConstantEquality) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);

  Symbol::Expression n_zero(-0);
  Symbol::Expression n_one(-1);
  Symbol::Expression n_two(-2);

  EXPECT_EQ(zero, zero);
  EXPECT_EQ(one, one);
  EXPECT_EQ(two, two);

  EXPECT_EQ(zero, n_zero);
  EXPECT_NE(one, n_one);
  EXPECT_NE(two, n_two);
}

TEST(Expression, VariableEquality) {
  Symbol::Expression x("x", 3);
  Symbol::Expression y1("y", 5);
  Symbol::Expression y2("y");
  // Normally we don't initialize
  // variables with duplicated names

  EXPECT_EQ(x, 3);

  EXPECT_EQ(y1, 5);

  EXPECT_EQ(y2, NAN);

  EXPECT_NE(y1, NAN);

  EXPECT_EQ(x, x);

  EXPECT_NE(x, y1);

  EXPECT_EQ(y1, y2);
  // Expressions are compared on symbol-base,
  // so they are equal even though they have different values
  // To include values, evaluate() should be used.
}

TEST(Expression, ConstantNegation) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);
  Symbol::Expression nOne(-1);
  Symbol::Expression nTwo(-2);

  EXPECT_EQ("0", -zero);
  EXPECT_EQ(" - 1", -one);
  EXPECT_EQ(" - 2", -two);

  EXPECT_EQ(zero, -zero);
  EXPECT_EQ(-one, nOne);
  EXPECT_EQ(-two, nTwo);

  EXPECT_NE(nOne, one);
  EXPECT_NE(two, nTwo);
}

TEST(Expression, VariableNegation) {
  Symbol::Expression x("x");
  Symbol::Expression y = -x;
  Symbol::Expression z = -x;

  EXPECT_EQ(" - x", y);
  EXPECT_EQ(" - x", z);

  EXPECT_NE(x, y);
  EXPECT_EQ(y, z);

  EXPECT_EQ(x, -y);
  EXPECT_EQ(-y, -z);
}

TEST(Expression, ConstantAddition) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);

  EXPECT_EQ(one + one, two);

  EXPECT_EQ(one + one, two + zero);

  EXPECT_EQ(one + one + zero, two + zero);

  EXPECT_EQ((one + one) + zero, two);

  EXPECT_EQ(one + (one + zero), two);
}

TEST(Expression, VariableAddition) {
  Symbol::Expression x("x");
  Symbol::Expression y("y");
  Symbol::Expression z = x + y;

  EXPECT_EQ(x + y, y + x);
  EXPECT_EQ(z, y + x);
  EXPECT_EQ(z, x + y);
  EXPECT_EQ(z, z);

  EXPECT_NE(x + y, z + x);
  EXPECT_NE(z, x);

  EXPECT_EQ((x + y) + x, x + (y + x));
  EXPECT_EQ((x + y) + x, x + (x + y));
}

TEST(Expression, MixedAddition) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);

  Symbol::Expression x("x");
  Symbol::Expression y("y");

  EXPECT_EQ(x + two, two + x);
  EXPECT_EQ(x + zero, x);

  EXPECT_EQ(x + two, "2 + x");
  EXPECT_EQ(x + two + one, "3 + x");
  EXPECT_EQ(x + zero + one, "1 + x");

  EXPECT_EQ(x + one + one, two + x);
  EXPECT_NE(y + one + one, two + x);

  EXPECT_EQ(x + y + one, y + one + x);
  EXPECT_EQ(x + one + one + x, x + two + x);
  EXPECT_EQ(x + y + one + one, y + two + x);
}

TEST(Expression, ConstantSubtraction) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);
  Symbol::Expression n_one(-1);
  Symbol::Expression n_two(-2);

  EXPECT_EQ(zero, "0");
  EXPECT_EQ(one, "1");
  EXPECT_EQ(two, "2");

  EXPECT_EQ(one - one, "0");

  EXPECT_EQ(one - one, zero);

  EXPECT_EQ(two - one, one);
  EXPECT_EQ(two - one - one, zero);

  EXPECT_EQ(zero - zero, zero);
  EXPECT_EQ(one - zero, two - one);
  EXPECT_EQ(one - zero, one);

  EXPECT_NE(two - one, zero);
  EXPECT_NE(two - one - zero, two);
  EXPECT_EQ(two - one - one, zero);

  EXPECT_EQ(one - n_one, two);
}

TEST(Expression, VariableSubtraction) {
  Symbol::Expression v("v");
  Symbol::Expression w("w");
  Symbol::Expression x("x");
  Symbol::Expression y("y");
  Symbol::Expression z("z");
  Symbol::Expression e1 = v - w;
  Symbol::Expression e2 = v - w - x - y - z;

  EXPECT_EQ(e1, " - w + v");

  EXPECT_EQ(e2, " - w - x - y - z + v");

  EXPECT_EQ(e1, v - w);

  EXPECT_NE(e1, w - v);

  EXPECT_EQ(e2, v - w - x - y - z);

  EXPECT_EQ(e2, e1 - x - y - z);

  EXPECT_NE(e2, - v - w - x - y - z);

  EXPECT_NE(e2, e1 - x);
}

TEST(Expression, MixedSubtraction) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);
  Symbol::Expression x("x");
  Symbol::Expression y("y");
  Symbol::Expression z("z");

  EXPECT_EQ(x - zero, "x");
  EXPECT_EQ(x - zero, x);
  EXPECT_EQ(- zero + x, x);
  EXPECT_EQ(- one + x, x - one);

  EXPECT_EQ(y - one + x, x + y - one);
  EXPECT_EQ(z - one + two + x, x + two - one + z);

  EXPECT_EQ(z - one + one + x, x + z);
  EXPECT_EQ(z - two + one + one + x, x + z);

  EXPECT_EQ(zero, x - x);
  EXPECT_NE(zero, x - y);
}

TEST(Expression, ConstantMultiplicatoin) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);

  EXPECT_EQ(zero, zero * 1);
  EXPECT_EQ(zero, zero * one);
  EXPECT_EQ(zero, zero * two);
  EXPECT_EQ(zero, zero * -1);
  EXPECT_EQ(zero, zero * -one);
  EXPECT_EQ(zero, zero * -two);

  EXPECT_EQ(two, one * 2);
  EXPECT_EQ(two, one * two);
  EXPECT_NE(two, one * zero);

  EXPECT_EQ(two * two, 4);
  EXPECT_EQ(two * one, one * two);
  EXPECT_EQ(two * two, two * two);

  EXPECT_EQ(-two * two, -4);
  EXPECT_EQ(-(two * two), -4);
}

TEST(Expression, VariableMultiplicatoin) {
  Symbol::Expression zero(0);
  Symbol::Expression x("x");
  Symbol::Expression y("y");
  Symbol::Expression z("z");

  EXPECT_EQ(x * x, x * x);
  EXPECT_EQ(x * y, y * x);
  EXPECT_EQ((x * y) * z, x * (y * z));
  EXPECT_EQ(x * y * z, z * y * x);
  EXPECT_EQ(x * x * y * z, z * y * x * x);

  EXPECT_NE(zero, x - 1.01 * x);

  EXPECT_EQ(x, 2 * x - x);
  EXPECT_EQ(x * 3, 2 * x + x);
  EXPECT_EQ(x * 3 * y, y * (2 * x + x));
  EXPECT_EQ(x * 3 * y - x * y, y * (2 * x + x) - x * y);

  EXPECT_EQ(-x * y * z, -(z * y * x));
  EXPECT_EQ(-3 * x * y * z, -(3 * z * y * x));
  EXPECT_EQ(-2 * x, -x -x);
  EXPECT_EQ(-(2 * -x), 4 * x - x -x);
  EXPECT_EQ(-2 * x * y, -(x * y) -(x * y));
  EXPECT_EQ(-3 * x * y * z, -(2 * z * y * x) - x * z * y);

  EXPECT_EQ(-3 * (x * y  * (-3 * z)), 9 * z * y * x);
  EXPECT_EQ(-3 * (x * y - z), 3 * (x * -y) + z + z + z);

  EXPECT_EQ(-3 * (x * x - z * z), 3 * (x * -x) + z * 3 * z);

  EXPECT_EQ(-3 * ((x + z) * (x - z)), 3 * (x * -x) + z * 3 * z);

}

TEST(Expression, ConstantPower) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);
  Symbol::Expression three(3);
  Symbol::Expression four(4);
  Symbol::Expression nine(9);

  EXPECT_EQ(one, zero ^ 0);
  EXPECT_EQ(one, one ^ 0);
  EXPECT_EQ(one, two ^ 0);
  EXPECT_EQ(one, three ^ 0);

  EXPECT_EQ(one, zero ^ zero);
  EXPECT_EQ(one, one ^ zero);
  EXPECT_EQ(one, two ^ zero);
  EXPECT_EQ(one, three ^ zero);

  EXPECT_EQ(one, zero ^ (-zero));
  EXPECT_EQ(one, one ^ (-zero));
  EXPECT_EQ(one, two ^ (-zero));
  EXPECT_EQ(one, three ^ (-zero));

  EXPECT_EQ(zero, zero ^ 1);
  EXPECT_EQ(one, one ^ 1);
  EXPECT_EQ(two, two ^ 1);
  EXPECT_EQ(three, three ^ 1);

  EXPECT_EQ(zero, zero ^ one);
  EXPECT_EQ(one, one ^ one);
  EXPECT_EQ(two, two ^ one);
  EXPECT_EQ(three, three ^ one);

  EXPECT_EQ(one, one ^ -1);
  EXPECT_NE(two, two ^ -1);
  EXPECT_NE(three, three ^ -1);

  EXPECT_EQ(zero, zero ^ 2);
  EXPECT_EQ(one, one ^ 2);
  EXPECT_EQ(four, two ^ 2);
  EXPECT_EQ(nine, three ^ 2);

  EXPECT_EQ(zero, zero ^ two);
  EXPECT_EQ(one, one ^ two);
  EXPECT_EQ(four, two ^ two);
  EXPECT_EQ(nine, three ^ two);

  EXPECT_EQ(one, one ^ -two);
  EXPECT_NE(four, two ^ -two);
  EXPECT_NE(nine, three ^ -two);
}

TEST(Expression, VariablePower) {
  Symbol::Expression x("x");
  Symbol::Expression y("y");
  Symbol::Expression z("z");

  EXPECT_EQ(x ^ 0, 1);
  EXPECT_EQ(x ^ 1, x);
  EXPECT_EQ(x ^ 2, x * x);
  EXPECT_EQ(x ^ 3, x * x * x);
  EXPECT_EQ(x ^ 4, x * x * x * x);

  EXPECT_EQ((x * y) ^ 0, 1);
  EXPECT_EQ((x * y) ^ 1, x * y);
  EXPECT_EQ((x * y) ^ 2, (x ^ 2) * (y ^ 2));
  EXPECT_EQ((x + y) ^ 2, x * x + x * y + x * y + y * y);

  EXPECT_EQ(x ^ (x - x), 1);

  EXPECT_EQ((x ^ y) * (z ^ y), (x * z) ^ y);

  EXPECT_EQ(x ^ (y + z), (x ^ y)* (x ^ z));
}

TEST(Expression, MixedPower) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);
  Symbol::Expression three(3);
  Symbol::Expression six(6);

  Symbol::Expression x("x");
  Symbol::Expression y("y");

  EXPECT_EQ(x ^ zero, one);
  EXPECT_EQ(x ^ one, x);
  EXPECT_EQ(x ^ two, x * x);
  EXPECT_EQ(x ^ three, x * x * x);

  EXPECT_EQ(x ^ y, x ^ (2 * y - y));
  EXPECT_EQ((x ^ 2) * (4 * x), x * ((2 * x) ^ 2));

  EXPECT_EQ("((x ^ x) ^ x) ^ x", x ^ x ^ x ^ x);
  EXPECT_EQ((((x ^ one) ^ two) ^ three), x ^ six);

  EXPECT_EQ("x ^ (2 * y)", (x ^ y) ^ two);
  EXPECT_EQ((x ^ y) ^ two, x ^ (y + y));
}

TEST(Expression, Log) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression x("x");
  Symbol::Expression y("y");
  Symbol::Expression z("z");

  EXPECT_EQ(log(one), zero);
  EXPECT_EQ(log(x * y), log(x) + log(y));
  EXPECT_EQ(log(x ^ y), log(x) * y);
  EXPECT_EQ(log(x ^ (-one)), -log(x));
  EXPECT_EQ(log(one ^ (-x)), zero);
}

TEST(Expression, Differentiate) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);

  Symbol::Expression x("x");
  Symbol::Expression y("y");
  Symbol::Expression z("z");

  EXPECT_EQ(x.differentiate(x), one);
  EXPECT_EQ(x.differentiate(y), zero);
  EXPECT_EQ((x + y).differentiate(y), one);
  EXPECT_EQ((x - y).differentiate(y), -one);

  EXPECT_EQ((x + x).differentiate(x), 2);
  EXPECT_EQ((2 * x).differentiate(x), 2);
  EXPECT_EQ((x * x).differentiate(x), 2 * x);

  EXPECT_EQ((x * y + y).differentiate(y), x + one);

  EXPECT_EQ((log(x)).differentiate(x), 1 / x);
}

TEST(Expression, Evaluate) {
  Symbol::Expression zero(0);

  EXPECT_EQ(zero.evaluate(), 0);

  zero.assign(1);

  EXPECT_NE(zero.evaluate(), 0);

  zero.assign(-0);

  EXPECT_EQ(zero.evaluate(), 0);

  Symbol::Expression x("x", 3);
  Symbol::Expression y("y", 10);

  EXPECT_EQ(x.evaluate(), 3);
  EXPECT_EQ(y.evaluate(), 10);

  EXPECT_EQ((x + y).evaluate(), 13);

  y.assign(5);

  EXPECT_EQ((x + y).evaluate(), 8);

  EXPECT_EQ((y - x).evaluate(), 2);

  EXPECT_EQ((x - y).evaluate(), -2);
}
