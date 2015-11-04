#define TEST_IMPL_
#include "symbol.hpp"
#include "gtest/gtest.h"

INITIALIZE_EASYLOGGINGPP

using namespace Symbol::Impl_;

TEST(Impl_, ConstantConstruction) {
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
}

TEST(Impl_, VariableConstruction) {
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
}

TEST(Impl_, NegationConstruction) {
  auto x = constructVARIABLE("x", 0);
  auto ten = constructCONST(10);

  auto n_x = constructNEGATE(x);
  auto n_ten = constructNEGATE(ten);

  EXPECT_EQ(" - x", n_x->id());
  EXPECT_EQ(Operator::NEGATE, n_x->operator_);
  EXPECT_EQ(1, n_x->operands_.size());
  EXPECT_EQ(x, n_x->operands_[0]);

  EXPECT_EQ(" - 10", n_ten->id());
  EXPECT_EQ(Operator::NEGATE, n_ten->operator_);
  EXPECT_EQ(1, n_ten->operands_.size());
  EXPECT_EQ(ten, n_ten->operands_[0]);
}

TEST(Impl_, PolynomialConstruction) {
  auto one = constructOne();
  auto zero = constructZero();
  auto three = constructCONST(3);

  auto x = constructVARIABLE("x", 1);
  auto y = constructVARIABLE("y", 2);
  auto z = constructVARIABLE("z", 3);

  auto addxxx = constructADD({x, x, x});

  EXPECT_EQ(Operator::ADD, addxxx->operator_);
  EXPECT_EQ("x + x + x", addxxx->id());
  EXPECT_EQ(x, addxxx->operands_[0]);
  EXPECT_EQ(x, addxxx->operands_[1]);
  EXPECT_EQ(x, addxxx->operands_[2]);

  auto addxyz = constructADD({x, y, z});

  EXPECT_EQ(Operator::ADD, addxyz->operator_);
  EXPECT_EQ("x + y + z", addxyz->id());
  EXPECT_EQ(x, addxyz->operands_[0]);
  EXPECT_EQ(y, addxyz->operands_[1]);
  EXPECT_EQ(z, addxyz->operands_[2]);

  auto addzyx = constructADD({z, y, x});

  EXPECT_EQ(Operator::ADD, addzyx->operator_);
  EXPECT_EQ("z + y + x", addzyx->id());
  EXPECT_EQ(z, addzyx->operands_[0]);
  EXPECT_EQ(y, addzyx->operands_[1]);
  EXPECT_EQ(x, addzyx->operands_[2]);

  auto addxyzxxxzyx = constructADD({addxyz, addxxx, addzyx});

  EXPECT_EQ(Operator::ADD, addxyzxxxzyx->operator_);
  EXPECT_EQ("(x + y + z) + (x + x + x) + (z + y + x)", addxyzxxxzyx->id());
  EXPECT_EQ(addxyz, addxyzxxxzyx->operands_[0]);
  EXPECT_EQ(addxxx, addxyzxxxzyx->operands_[1]);
  EXPECT_EQ(addzyx, addxyzxxxzyx->operands_[2]);

  auto multixyz = constructMULTIPLY({x, y, z});

  EXPECT_EQ(Operator::MULTIPLY, multixyz->operator_);
  EXPECT_EQ("x * y * z", multixyz->id());
  EXPECT_EQ(x, multixyz->operands_[0]);
  EXPECT_EQ(y, multixyz->operands_[1]);
  EXPECT_EQ(z, multixyz->operands_[2]);

  auto multixxx = constructMULTIPLY({x, x, x});

  EXPECT_EQ(Operator::MULTIPLY, multixxx->operator_);
  EXPECT_EQ("x * x * x", multixxx->id());
  EXPECT_EQ(x, multixxx->operands_[0]);
  EXPECT_EQ(x, multixxx->operands_[1]);
  EXPECT_EQ(x, multixxx->operands_[2]);

  auto multizyx = constructMULTIPLY({z, y, x});

  EXPECT_EQ(Operator::MULTIPLY, multizyx->operator_);
  EXPECT_EQ("z * y * x", multizyx->id());
  EXPECT_EQ(z, multizyx->operands_[0]);
  EXPECT_EQ(y, multizyx->operands_[1]);
  EXPECT_EQ(x, multizyx->operands_[2]);

  auto multixyzxxxzyx = constructMULTIPLY({multixyz, multixxx, multizyx});

  EXPECT_EQ(Operator::MULTIPLY, multixyzxxxzyx->operator_);
  EXPECT_EQ("(x * y * z) * (x * x * x) * (z * y * x)", multixyzxxxzyx->id());
  EXPECT_EQ(multixyz, multixyzxxxzyx->operands_[0]);
  EXPECT_EQ(multixxx, multixyzxxxzyx->operands_[1]);
  EXPECT_EQ(multizyx, multixyzxxxzyx->operands_[2]);

  auto powx3 = constructPOWER({x, three});

  EXPECT_EQ(Operator::POWER, powx3->operator_);
  EXPECT_EQ("x ^ 3", powx3->id());
  EXPECT_EQ(x, powx3->operands_[0]);
  EXPECT_EQ(three, powx3->operands_[1]);

  auto powxy = constructPOWER({x, y});

  EXPECT_EQ(Operator::POWER, powxy->operator_);
  EXPECT_EQ("x ^ y", powxy->id());
  EXPECT_EQ(x, powxy->operands_[0]);
  EXPECT_EQ(y, powxy->operands_[1]);

  auto powxaddxyz = constructPOWER({x, addxyz});

  EXPECT_EQ(Operator::POWER, powxaddxyz->operator_);
  EXPECT_EQ("x ^ (x + y + z)", powxaddxyz->id());
  EXPECT_EQ(x, powxaddxyz->operands_[0]);
  EXPECT_EQ(addxyz, powxaddxyz->operands_[1]);

  auto powmultizyxxpowx3 = constructPOWER({multizyx, powx3});

  EXPECT_EQ(Operator::POWER, powmultizyxxpowx3->operator_);
  EXPECT_EQ("(z * y * x) ^ (x ^ 3)", powmultizyxxpowx3->id());
  EXPECT_EQ(multizyx, powmultizyxxpowx3->operands_[0]);
  EXPECT_EQ(powx3, powmultizyxxpowx3->operands_[1]);

  auto invx = constructInverse(x);

  EXPECT_EQ(Operator::POWER, invx->operator_);
  EXPECT_EQ("x ^ ( - 1)", invx->id());
  EXPECT_EQ(x, invx->operands_[0]);

  auto invaddxyz = constructInverse(addxyz);

  EXPECT_EQ(Operator::POWER, invaddxyz->operator_);
  EXPECT_EQ("(x + y + z) ^ ( - 1)", invaddxyz->id());
  EXPECT_EQ(addxyz, invaddxyz->operands_[0]);

  EXPECT_THROW(constructPOWER({}), std::runtime_error);
  EXPECT_THROW(constructPOWER({x}), std::runtime_error);
  EXPECT_THROW(constructPOWER({x, x, y}), std::runtime_error);

  auto logx = constructLOG(x);

  EXPECT_EQ(Operator::LOG, logx->operator_);
  EXPECT_EQ("log(x)", logx->id());
  EXPECT_EQ(x, logx->operands_[0]);

  EXPECT_THROW(constructLOG(zero), std::runtime_error);
  EXPECT_THROW(constructLOG(-zero), std::runtime_error);
  EXPECT_THROW(constructLOG(-one), std::runtime_error);
  EXPECT_NO_THROW(constructLOG(-x));
}
