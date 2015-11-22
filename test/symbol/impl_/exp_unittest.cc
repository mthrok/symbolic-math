#define TEST_IMPL_
#include "symbol.hpp"
#include "gtest/gtest.h"

INITIALIZE_EASYLOGGINGPP

using namespace Symbol::Impl_;

TEST(Impl_, ConstantConstruction) {
  auto one = constructOne();
  auto zero = constructZero();
  auto three = constructCONST(3);

  ASSERT_EQ(1, one->value());
  ASSERT_EQ("1", one->toStr());

  ASSERT_EQ(0, zero->value());
  ASSERT_EQ("0", zero->toStr());

  ASSERT_EQ(3, three->value());
  ASSERT_EQ("3", three->toStr());
}

TEST(Impl_, FloatConstantConstruction) {
  auto c = constructCONST(2.5);

  ASSERT_EQ("2.500", c->toStr());
}

TEST(Impl_, VariableConstruction) {
  auto v = constructVARIABLE("v");
  auto w = constructVARIABLE("w");

  ASSERT_EQ("v", v->name_);
  ASSERT_EQ("v", v->toStr());
  ASSERT_TRUE(std::isnan(w->value()));

  ASSERT_EQ("w", w->name_);
  ASSERT_EQ("w", w->toStr());
  ASSERT_TRUE(std::isnan(w->value()));
}

TEST(Impl_, VariableConstructionWithInitialValue) {
  auto x = constructVARIABLE("x", 1);
  auto y = constructVARIABLE("y", 2);
  auto z = constructVARIABLE("z", 3);

  ASSERT_EQ("x", x->name_);
  ASSERT_EQ("x", x->toStr());
  ASSERT_EQ(1, x->value());

  ASSERT_EQ("y", y->name_);
  ASSERT_EQ("y", y->toStr());
  ASSERT_EQ(2, y->value());

  ASSERT_EQ("z", z->name_);
  ASSERT_EQ("z", z->toStr());
  ASSERT_EQ(3, z->value());
}

TEST(Impl_, NegationConstruction) {
  auto x = constructVARIABLE("x", 0);
  auto ten = constructCONST(10);

  auto n_x = constructNEGATE(x);
  auto n_ten = constructNEGATE(ten);

  ASSERT_EQ(" - x", n_x->toStr());
  ASSERT_EQ(Operator::NEGATE, n_x->operator_);
  ASSERT_EQ(x, n_x->operands_[0]);

  ASSERT_EQ(" - 10", n_ten->toStr());
  ASSERT_EQ(Operator::NEGATE, n_ten->operator_);
  ASSERT_EQ(ten, n_ten->operands_[0]);
}

TEST(Impl_, PolynomialConstruction) {
  auto one = constructOne();
  auto zero = constructZero();
  auto three = constructCONST(3);

  auto x = constructVARIABLE("x", 1);
  auto y = constructVARIABLE("y", 2);
  auto z = constructVARIABLE("z", 3);

  auto addxxx = constructADD({x, x, x});

  ASSERT_EQ(Operator::ADD, addxxx->operator_);
  ASSERT_EQ("x + x + x", addxxx->toStr());
  ASSERT_EQ(x, addxxx->operands_[0]);
  ASSERT_EQ(x, addxxx->operands_[1]);
  ASSERT_EQ(x, addxxx->operands_[2]);

  auto addxyz = constructADD({x, y, z});

  ASSERT_EQ(Operator::ADD, addxyz->operator_);
  ASSERT_EQ("x + y + z", addxyz->toStr());
  ASSERT_EQ(x, addxyz->operands_[0]);
  ASSERT_EQ(y, addxyz->operands_[1]);
  ASSERT_EQ(z, addxyz->operands_[2]);

  auto addzyx = constructADD({z, y, x});

  ASSERT_EQ(Operator::ADD, addzyx->operator_);
  ASSERT_EQ("z + y + x", addzyx->toStr());
  ASSERT_EQ(z, addzyx->operands_[0]);
  ASSERT_EQ(y, addzyx->operands_[1]);
  ASSERT_EQ(x, addzyx->operands_[2]);

  auto addxyzxxxzyx = constructADD({addxyz, addxxx, addzyx});

  ASSERT_EQ(Operator::ADD, addxyzxxxzyx->operator_);
  ASSERT_EQ("(x + y + z) + (x + x + x) + (z + y + x)", addxyzxxxzyx->toStr());
  ASSERT_EQ(addxyz, addxyzxxxzyx->operands_[0]);
  ASSERT_EQ(addxxx, addxyzxxxzyx->operands_[1]);
  ASSERT_EQ(addzyx, addxyzxxxzyx->operands_[2]);

  auto multixyz = constructMULTIPLY({x, y, z});

  ASSERT_EQ(Operator::MULTIPLY, multixyz->operator_);
  ASSERT_EQ("x * y * z", multixyz->toStr());
  ASSERT_EQ(x, multixyz->operands_[0]);
  ASSERT_EQ(y, multixyz->operands_[1]);
  ASSERT_EQ(z, multixyz->operands_[2]);

  auto multixxx = constructMULTIPLY({x, x, x});

  ASSERT_EQ(Operator::MULTIPLY, multixxx->operator_);
  ASSERT_EQ("x * x * x", multixxx->toStr());
  ASSERT_EQ(x, multixxx->operands_[0]);
  ASSERT_EQ(x, multixxx->operands_[1]);
  ASSERT_EQ(x, multixxx->operands_[2]);

  auto multizyx = constructMULTIPLY({z, y, x});

  ASSERT_EQ(Operator::MULTIPLY, multizyx->operator_);
  ASSERT_EQ("z * y * x", multizyx->toStr());
  ASSERT_EQ(z, multizyx->operands_[0]);
  ASSERT_EQ(y, multizyx->operands_[1]);
  ASSERT_EQ(x, multizyx->operands_[2]);

  auto multixyzxxxzyx = constructMULTIPLY({multixyz, multixxx, multizyx});

  ASSERT_EQ(Operator::MULTIPLY, multixyzxxxzyx->operator_);
  ASSERT_EQ("(x * y * z) * (x * x * x) * (z * y * x)", multixyzxxxzyx->toStr());
  ASSERT_EQ(multixyz, multixyzxxxzyx->operands_[0]);
  ASSERT_EQ(multixxx, multixyzxxxzyx->operands_[1]);
  ASSERT_EQ(multizyx, multixyzxxxzyx->operands_[2]);

  auto powx3 = constructPOWER({x, three});

  ASSERT_EQ(Operator::POWER, powx3->operator_);
  ASSERT_EQ("x ^ 3", powx3->toStr());
  ASSERT_EQ(x, powx3->operands_[0]);
  ASSERT_EQ(three, powx3->operands_[1]);

  auto powxy = constructPOWER({x, y});

  ASSERT_EQ(Operator::POWER, powxy->operator_);
  ASSERT_EQ("x ^ y", powxy->toStr());
  ASSERT_EQ(x, powxy->operands_[0]);
  ASSERT_EQ(y, powxy->operands_[1]);

  auto powxaddxyz = constructPOWER({x, addxyz});

  ASSERT_EQ(Operator::POWER, powxaddxyz->operator_);
  ASSERT_EQ("x ^ (x + y + z)", powxaddxyz->toStr());
  ASSERT_EQ(x, powxaddxyz->operands_[0]);
  ASSERT_EQ(addxyz, powxaddxyz->operands_[1]);

  auto powmultizyxxpowx3 = constructPOWER({multizyx, powx3});

  ASSERT_EQ(Operator::POWER, powmultizyxxpowx3->operator_);
  ASSERT_EQ("(z * y * x) ^ (x ^ 3)", powmultizyxxpowx3->toStr());
  ASSERT_EQ(multizyx, powmultizyxxpowx3->operands_[0]);
  ASSERT_EQ(powx3, powmultizyxxpowx3->operands_[1]);

  auto invx = constructInverse(x);

  ASSERT_EQ(Operator::POWER, invx->operator_);
  ASSERT_EQ("x ^ ( - 1)", invx->toStr());
  ASSERT_EQ(x, invx->operands_[0]);

  auto invaddxyz = constructInverse(addxyz);

  ASSERT_EQ(Operator::POWER, invaddxyz->operator_);
  ASSERT_EQ("(x + y + z) ^ ( - 1)", invaddxyz->toStr());
  ASSERT_EQ(addxyz, invaddxyz->operands_[0]);

  ASSERT_THROW(constructPOWER({}), std::runtime_error);
  ASSERT_THROW(constructPOWER({x}), std::runtime_error);
  ASSERT_THROW(constructPOWER({x, x, y}), std::runtime_error);

  auto logx = constructLOG(x);

  ASSERT_EQ(Operator::LOG, logx->operator_);
  ASSERT_EQ("log(x)", logx->toStr());
  ASSERT_EQ(x, logx->operands_[0]);

  ASSERT_THROW(constructLOG(zero), std::runtime_error);
  ASSERT_THROW(constructLOG(-zero), std::runtime_error);
  ASSERT_THROW(constructLOG(-one), std::runtime_error);
  ASSERT_NO_THROW(constructLOG(-x));
}

TEST(Data, Print) {
  ASSERT_THROW(Data(0.0, Type::NONE), std::runtime_error);
  Data abstract;
  Data scalar(3.0);
  Data vec1({2}, 2);
  Data vec2({11}, 2);
  Data mat2({2, 3}, 23);
  Data mat3({5, 3, 4}, 534);
  Data mat4({2, 3, 4, 5}, 2345);

  std::cout << abstract << "\n";
  std::cout << scalar << "\n";
  std::cout << vec1 << "\n";
  std::cout << vec2 << "\n";
  std::cout << mat2 << "\n";
  std::cout << mat3 << "\n";
  std::cout << mat4 << "\n";

}
