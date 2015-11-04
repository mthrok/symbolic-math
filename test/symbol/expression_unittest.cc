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

  ASSERT_EQ("0", zero);
  ASSERT_EQ("1", one);
  ASSERT_EQ("2", two);

  ASSERT_EQ("0", n_zero);
  ASSERT_EQ(" - 1", n_one);
  ASSERT_EQ(" - 2", n_two);

  ASSERT_EQ(zero, 0);
  ASSERT_EQ(one, 1);
  ASSERT_EQ(two, 2);

  ASSERT_EQ(n_zero, 0);
  ASSERT_EQ(n_one, -1);
  ASSERT_EQ(n_two, -2);

  ASSERT_NE(zero, 1);
  ASSERT_NE(one,  2);
  ASSERT_NE(two,  0);

  ASSERT_NE(n_zero, -1);
  ASSERT_NE(n_one,  1);
  ASSERT_NE(n_two,  2);
}

TEST(Expression, VariableInitialization) {
  Symbol::Expression x("x", 3);
  Symbol::Expression y("y", 5);

  ASSERT_EQ("x", x);
  ASSERT_EQ("y", y);

  ASSERT_NE("y", x);
  ASSERT_NE("x", y);
}

TEST(Expression, ConstantEquality) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);

  Symbol::Expression n_zero(-0);
  Symbol::Expression n_one(-1);
  Symbol::Expression n_two(-2);

  ASSERT_EQ(zero, zero);
  ASSERT_EQ(one, one);
  ASSERT_EQ(two, two);

  ASSERT_EQ(zero, n_zero);
  ASSERT_NE(one, n_one);
  ASSERT_NE(two, n_two);
}

TEST(Expression, VariableEquality) {
  Symbol::Expression x("x", 3);
  Symbol::Expression y1("y", 5);
  Symbol::Expression y2("y");
  // Normally we don't initialize
  // variables with duplicated names

  ASSERT_EQ(x, 3);

  ASSERT_EQ(y1, 5);

  ASSERT_EQ(y2, NAN);

  ASSERT_NE(y1, NAN);

  ASSERT_EQ(x, x);

  ASSERT_NE(x, y1);

  ASSERT_EQ(y1, y2);
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

  ASSERT_EQ("0", -zero);
  ASSERT_EQ(" - 1", -one);
  ASSERT_EQ(" - 2", -two);

  ASSERT_EQ(zero, -zero);
  ASSERT_EQ(-one, nOne);
  ASSERT_EQ(-two, nTwo);

  ASSERT_NE(nOne, one);
  ASSERT_NE(two, nTwo);
}

TEST(Expression, VariableNegation) {
  Symbol::Expression x("x");
  Symbol::Expression y = -x;
  Symbol::Expression z = -x;

  ASSERT_EQ(" - x", y);
  ASSERT_EQ(" - x", z);

  ASSERT_NE(x, y);
  ASSERT_EQ(y, z);

  ASSERT_EQ(x, -y);
  ASSERT_EQ(-y, -z);
}

TEST(Expression, ConstantAddition) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);

  ASSERT_EQ(2, one + one);
  ASSERT_EQ(3, one + two);

  ASSERT_EQ(two, one + one);
  ASSERT_EQ(one + one, two + zero);
  ASSERT_EQ(one + two, two + one);
  ASSERT_EQ(one + one + zero, two + zero);
  ASSERT_EQ(two, (one + one) + zero);
  ASSERT_EQ(two, one + (one + zero));
}

TEST(Expression, VariableAddition) {
  Symbol::Expression x("x");
  Symbol::Expression y("y");
  Symbol::Expression z = x + y;

  ASSERT_EQ(x + y, y + x);
  ASSERT_EQ(z, y + x);
  ASSERT_EQ(z, x + y);
  ASSERT_EQ(z, z);

  ASSERT_NE(x + y, z + x);
  ASSERT_NE(z, x);

  ASSERT_EQ((x + y) + x, x + (y + x));
  ASSERT_EQ((x + y) + x, x + (x + y));
}

TEST(Expression, MixedAddition) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);

  Symbol::Expression x("x");
  Symbol::Expression y("y");

  ASSERT_EQ("2 + x", x + two);
  ASSERT_EQ("3 + x", x + two + one);
  ASSERT_EQ("1 + x", x + zero + one);

  ASSERT_EQ(x + two, two + x);
  ASSERT_EQ(x + zero, x);

  ASSERT_EQ(x + one + one, two + x);
  ASSERT_NE(y + one + one, two + x);

  ASSERT_EQ(x + y + one, y + one + x);
  ASSERT_EQ(x + one + one + x, x + two + x);
  ASSERT_EQ(x + y + one + one, y + two + x);
}

TEST(Expression, ConstantSubtraction) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);
  Symbol::Expression n_one(-1);
  Symbol::Expression n_two(-2);

  ASSERT_EQ(0, one - one);
  ASSERT_EQ("0", one - one);
  ASSERT_EQ(zero, one - one);

  ASSERT_EQ(0, - one + one);
  ASSERT_EQ("0", - one + one);
  ASSERT_EQ(zero, - one + one);

  ASSERT_EQ(zero, zero - zero);
  ASSERT_EQ(zero, - zero + zero);

  ASSERT_EQ(zero, two - one - one);
  ASSERT_EQ(zero, - one + two - one);

  ASSERT_EQ(one, two - one);
  ASSERT_EQ(one, - one + two);

  ASSERT_EQ(one, one - zero);
  ASSERT_EQ(one - zero, two - one);

  ASSERT_NE(zero, two - one);
  ASSERT_NE(two, two - one - zero);
  ASSERT_EQ(zero, two - one - one);

  ASSERT_EQ(two, one - n_one);
  ASSERT_EQ(two, - n_one + one);
}

TEST(Expression, VariableSubtraction) {
  Symbol::Expression v("v");
  Symbol::Expression w("w");
  Symbol::Expression x("x");
  Symbol::Expression y("y");
  Symbol::Expression z("z");
  Symbol::Expression e1 = v - w;
  Symbol::Expression e2 = v - w - x - y - z;

  ASSERT_EQ(e1, " - w + v");

  ASSERT_EQ(e2, " - w - x - y - z + v");

  ASSERT_EQ(e1, v - w);

  ASSERT_NE(e1, w - v);

  ASSERT_EQ(e2, v - w - x - y - z);

  ASSERT_EQ(e2, e1 - x - y - z);

  ASSERT_NE(e2, - v - w - x - y - z);

  ASSERT_NE(e2, e1 - x);
}

TEST(Expression, MixedSubtraction) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);
  Symbol::Expression x("x");
  Symbol::Expression y("y");
  Symbol::Expression z("z");

  ASSERT_EQ(x - zero, "x");
  ASSERT_EQ(x - zero, x);
  ASSERT_EQ(- zero + x, x);
  ASSERT_EQ(- one + x, x - one);

  ASSERT_EQ(y - one + x, x + y - one);
  ASSERT_EQ(z - one + two + x, x + two - one + z);

  ASSERT_EQ(z - one + one + x, x + z);
  ASSERT_EQ(z - two + one + one + x, x + z);

  ASSERT_EQ(zero, x - x);
  ASSERT_NE(zero, x - y);
}

TEST(Expression, ConstantMultiplicatoin) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);

  ASSERT_EQ(zero, zero * 1);
  ASSERT_EQ(zero, zero * one);
  ASSERT_EQ(zero, zero * two);
  ASSERT_EQ(zero, zero * -1);
  ASSERT_EQ(zero, zero * -one);
  ASSERT_EQ(zero, zero * -two);

  ASSERT_EQ(two, one * 2);
  ASSERT_EQ(two, one * two);
  ASSERT_NE(two, one * zero);

  ASSERT_EQ(two * two, 4);
  ASSERT_EQ(two * one, one * two);
  ASSERT_EQ(two * two, two * two);

  ASSERT_EQ(-two * two, -4);
  ASSERT_EQ(-(two * two), -4);
}

TEST(Expression, VariableMultiplicatoin) {
  Symbol::Expression zero(0);
  Symbol::Expression x("x");
  Symbol::Expression y("y");
  Symbol::Expression z("z");

  ASSERT_EQ(x * x, x * x);
  ASSERT_EQ(x * y, y * x);
  ASSERT_EQ((x * y) * z, x * (y * z));
  ASSERT_EQ(x * y * z, z * y * x);
  ASSERT_EQ(x * x * y * z, z * y * x * x);

  ASSERT_NE(zero, x - 1.01 * x);

  ASSERT_EQ(x, 2 * x - x);
  ASSERT_EQ(x * 3, 2 * x + x);
  ASSERT_EQ(x * 3 * y, y * (2 * x + x));
  ASSERT_EQ(x * 3 * y - x * y, y * (2 * x + x) - x * y);

  ASSERT_EQ(-x * y * z, -(z * y * x));
  ASSERT_EQ(-3 * x * y * z, -(3 * z * y * x));
  ASSERT_EQ(-2 * x, -x -x);
  ASSERT_EQ(-(2 * -x), 4 * x - x -x);
  ASSERT_EQ(-2 * x * y, -(x * y) -(x * y));
  ASSERT_EQ(-3 * x * y * z, -(2 * z * y * x) - x * z * y);

  ASSERT_EQ(-3 * (x * y  * (-3 * z)), 9 * z * y * x);
  ASSERT_EQ(-3 * (x * y - z), 3 * (x * -y) + z + z + z);

  ASSERT_EQ(-3 * (x * x - z * z), 3 * (x * -x) + z * 3 * z);

  ASSERT_EQ(-3 * ((x + z) * (x - z)), 3 * (x * -x) + z * 3 * z);

}

TEST(Expression, ConstantPower) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);
  Symbol::Expression three(3);
  Symbol::Expression four(4);
  Symbol::Expression nine(9);

  ASSERT_EQ(one, zero ^ 0);
  ASSERT_EQ(one, one ^ 0);
  ASSERT_EQ(one, two ^ 0);
  ASSERT_EQ(one, three ^ 0);

  ASSERT_EQ(one, zero ^ zero);
  ASSERT_EQ(one, one ^ zero);
  ASSERT_EQ(one, two ^ zero);
  ASSERT_EQ(one, three ^ zero);

  ASSERT_EQ(one, zero ^ (-zero));
  ASSERT_EQ(one, one ^ (-zero));
  ASSERT_EQ(one, two ^ (-zero));
  ASSERT_EQ(one, three ^ (-zero));

  ASSERT_EQ(zero, zero ^ 1);
  ASSERT_EQ(one, one ^ 1);
  ASSERT_EQ(two, two ^ 1);
  ASSERT_EQ(three, three ^ 1);

  ASSERT_EQ(zero, zero ^ one);
  ASSERT_EQ(one, one ^ one);
  ASSERT_EQ(two, two ^ one);
  ASSERT_EQ(three, three ^ one);

  ASSERT_EQ(one, one ^ -1);
  ASSERT_NE(two, two ^ -1);
  ASSERT_NE(three, three ^ -1);

  ASSERT_EQ(zero, zero ^ 2);
  ASSERT_EQ(one, one ^ 2);
  ASSERT_EQ(four, two ^ 2);
  ASSERT_EQ(nine, three ^ 2);

  ASSERT_EQ(zero, zero ^ two);
  ASSERT_EQ(one, one ^ two);
  ASSERT_EQ(four, two ^ two);
  ASSERT_EQ(nine, three ^ two);

  ASSERT_EQ(one, one ^ -two);
  ASSERT_NE(four, two ^ -two);
  ASSERT_NE(nine, three ^ -two);
}

TEST(Expression, VariablePower) {
  Symbol::Expression x("x");
  Symbol::Expression y("y");
  Symbol::Expression z("z");

  ASSERT_EQ(x ^ 0, 1);
  ASSERT_EQ(x ^ 1, x);
  ASSERT_EQ(x ^ 2, x * x);
  ASSERT_EQ(x ^ 3, x * x * x);
  ASSERT_EQ(x ^ 4, x * x * x * x);

  ASSERT_EQ((x * y) ^ 0, 1);
  ASSERT_EQ((x * y) ^ 1, x * y);
  ASSERT_EQ((x * y) ^ 2, (x ^ 2) * (y ^ 2));
  ASSERT_EQ((x + y) ^ 2, x * x + x * y + x * y + y * y);

  ASSERT_EQ(x ^ (x - x), 1);

  ASSERT_EQ((x ^ y) * (z ^ y), (x * z) ^ y);

  ASSERT_EQ(x ^ (y + z), (x ^ y)* (x ^ z));
}

TEST(Expression, MixedPower) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression two(2);
  Symbol::Expression three(3);
  Symbol::Expression six(6);

  Symbol::Expression x("x");
  Symbol::Expression y("y");

  ASSERT_EQ(x ^ zero, one);
  ASSERT_EQ(x ^ one, x);
  ASSERT_EQ(x ^ two, x * x);
  ASSERT_EQ(x ^ three, x * x * x);

  ASSERT_EQ(x ^ y, x ^ (2 * y - y));
  ASSERT_EQ((x ^ 2) * (4 * x), x * ((2 * x) ^ 2));

  ASSERT_EQ("((x ^ x) ^ x) ^ x", x ^ x ^ x ^ x);
  ASSERT_EQ((((x ^ one) ^ two) ^ three), x ^ six);

  ASSERT_EQ("x ^ (2 * y)", (x ^ y) ^ two);
  ASSERT_EQ((x ^ y) ^ two, x ^ (y + y));
}

TEST(Expression, Log) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);
  Symbol::Expression x("x");
  Symbol::Expression y("y");
  Symbol::Expression z("z");

  ASSERT_EQ(log(one), zero);
  ASSERT_EQ(log(x * y), log(x) + log(y));
  ASSERT_EQ(log(x ^ y), log(x) * y);
  ASSERT_EQ(log(x ^ (-one)), -log(x));
  ASSERT_EQ(log(one ^ (-x)), zero);
}

TEST(Expression, Differentiate) {
  Symbol::Expression zero(0);
  Symbol::Expression one(1);

  Symbol::Expression x("x");
  Symbol::Expression y("y");
  Symbol::Expression z("z");

  ASSERT_EQ(x.differentiate(x), one);
  ASSERT_EQ(x.differentiate(y), zero);
  ASSERT_EQ((x + y).differentiate(y), one);
  ASSERT_EQ((x - y).differentiate(y), -one);
  ASSERT_EQ((x + x).differentiate(x), 2);

  ASSERT_EQ((2 * x).differentiate(x), 2);
  ASSERT_EQ((x * x).differentiate(x), 2 * x);

  ASSERT_EQ((x * y + y).differentiate(y), x + one);

  ASSERT_EQ((log(x)).differentiate(x), 1 / x);
}

TEST(Expression, Evaluate) {
  Symbol::Expression zero(0);

  ASSERT_EQ(zero.evaluate(), 0);

  zero.assign(1);

  ASSERT_NE(zero.evaluate(), 0);

  zero.assign(-0);

  ASSERT_EQ(zero.evaluate(), 0);

  Symbol::Expression x("x", 3);
  Symbol::Expression y("y", 10);

  ASSERT_EQ(x.evaluate(), 3);
  ASSERT_EQ(y.evaluate(), 10);

  ASSERT_EQ((x + y).evaluate(), 13);

  y.assign(5);

  ASSERT_EQ((x + y).evaluate(), 8);

  ASSERT_EQ((y - x).evaluate(), 2);

  ASSERT_EQ((x - y).evaluate(), -2);
}
