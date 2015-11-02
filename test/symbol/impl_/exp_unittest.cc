#define TEST_IMPL_
#include "symbol.hpp"
#include "gtest/gtest.h"

INITIALIZE_EASYLOGGINGPP

using namespace Symbol::Impl_;

TEST(Impl_, MonomialConstruction) {
  auto one = constructOne();
  auto zero = constructZero();
  auto three = constructCONST(3);

  EXPECT_EQ(1, one->value_);
  EXPECT_EQ("1", one->id());
  EXPECT_EQ(Operator::CONST, one->operator_);
  EXPECT_EQ(0, one->operands_.size());

  EXPECT_EQ(0, zero->value_);
  EXPECT_EQ("0", zero->id());
  EXPECT_EQ(Operator::CONST, zero->operator_);
  EXPECT_EQ(0, zero->operands_.size());

  EXPECT_EQ(3, three->value_);
  EXPECT_EQ("3", three->id());
  EXPECT_EQ(0, three->operands_.size());

  auto x = constructVARIABLE("x", 1);
  auto y = constructVARIABLE("y", 2);
  auto z = constructVARIABLE("z", 3);

  EXPECT_EQ("x", x->name_);
  EXPECT_EQ("x", x->id());
  EXPECT_EQ(1, x->value_);
  EXPECT_EQ(Operator::VARIABLE, x->operator_);
  EXPECT_EQ(0, x->operands_.size());

  EXPECT_EQ("y", y->name_);
  EXPECT_EQ("y", y->id());
  EXPECT_EQ(2, y->value_);
  EXPECT_EQ(Operator::VARIABLE, y->operator_);
  EXPECT_EQ(0, y->operands_.size());

  EXPECT_EQ("z", z->name_);
  EXPECT_EQ("z", z->id());
  EXPECT_EQ(3, z->value_);
  EXPECT_EQ(Operator::VARIABLE, z->operator_);
  EXPECT_EQ(0, z->operands_.size());

  auto n_x = constructNEGATE(x);

  EXPECT_EQ(" - x", n_x->id());
  EXPECT_EQ(Operator::NEGATE, n_x->operator_);
  EXPECT_EQ(1, n_x->operands_.size());
  EXPECT_EQ(x, n_x->operands_[0]);
}

TEST(Impl_, PolynomialConstruction) {
  auto x = constructVARIABLE("x", 1);
  auto y = constructVARIABLE("y", 2);
  auto z = constructVARIABLE("z", 3);

  auto addition = constructADD({x, y, z});

  EXPECT_EQ(Operator::ADD, addition->operator_);
  EXPECT_EQ("(x + y + z)", addition->id());
  EXPECT_EQ(x, addition->operands_[0]);
  EXPECT_EQ(y, addition->operands_[1]);
  EXPECT_EQ(z, addition->operands_[2]);

  auto multi = constructMULTIPLY({x, y, z});

  EXPECT_EQ(Operator::MULTIPLY, multi->operator_);
  EXPECT_EQ("(x * y * z)", multi->id());
  EXPECT_EQ(x, multi->operands_[0]);
  EXPECT_EQ(y, multi->operands_[1]);
  EXPECT_EQ(z, multi->operands_[2]);

  // EXPECT_THROW(constructPOWER({x, y, z}), std::runtime_error);
}
