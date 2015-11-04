#include <map>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <functional>

#include "easylogging++.h"

////////////////////////////////////////////////////////////////////////////////
#include <cmath>
inline bool isNearlyEqual(double x, double y, const double epsilon = 1e-5) {
  return std::abs(x - y) <= epsilon; // * std::abs(x);
}

////////////////////////////////////////////////////////////////////////////////
// Data structure
namespace Symbol {
  namespace Impl_ {
    enum class Operator;

    class Exp;
  };

  class Expression;
};

#define MAKE_SHARED_EXP std::make_shared<Impl_::Exp>
typedef std::shared_ptr<Symbol::Impl_::Exp> Operand;
typedef std::shared_ptr<Symbol::Impl_::Exp> pExpression;
typedef std::vector<Operand> Operands;

////////////////////////////////////////////////////////////////////////////////
// Operations
namespace Symbol {
  namespace Impl_ {
    pExpression constructZero();
    pExpression constructOne();
    pExpression constructCONST(const double c);
    pExpression constructVARIABLE(const std::string name, const double c);
    pExpression constructNEGATE(const Operand o);
    pExpression constructADD(const Operands ops);
    pExpression constructMULTIPLY(const Operands ops);
    pExpression constructPOWER(const Operands ops);
    pExpression constructInverse(const Operand o);
    pExpression constructLOG(const Operand o);

    pExpression simplify(pExpression pExp);
    pExpression flatten(pExpression pExp);
    pExpression flattenNEGATE(pExpression pExp);
    pExpression flattenMultiOperands(pExpression pExp);
    pExpression sort(pExpression pExp);
    // pExpression factor(pExpression pExp);
    pExpression expand(pExpression pExp);
    pExpression expandNEGATE(pExpression pExp);
    pExpression expandMULTIPLY(pExpression pExp);
    pExpression expandPOWER(pExpression pExp);
    pExpression expandLOG(pExpression pExp);
    pExpression merge(pExpression pExp);
    pExpression mergeADD(pExpression pExp);
    pExpression mergeMULTIPLY(pExpression pExp);
    pExpression mergePOWER(pExpression pExp);
    pExpression mergeLOG(pExpression pExp);

    pExpression differentiate(pExpression dy, Operand dx);

    Operands decompose2(Operand o);
    Operands decompose3(Operand o);

    pExpression operator - (const Operand o);
    pExpression operator + (const Operand o1, const Operand o2);
    pExpression operator - (const Operand o1, const Operand o2);
    pExpression operator * (const Operand o1, const Operand o2);
    pExpression operator ^ (const Operand o1, const Operand o2);
    pExpression operator / (const Operand o1, const Operand o2);
    pExpression log (const Operand o);
  };

  bool operator == (const Expression &e1, const Expression &e2);
  bool operator == (const Expression &e, const std::string strExp);
  bool operator == (const std::string strExp, const Expression &e);
  bool operator == (const Expression &e, const double c);
  bool operator == (const double c, const Expression &e);
  bool operator != (const Expression &e1, const Expression &e2);
  bool operator != (const Expression &e, const std::string strExp);
  bool operator != (const std::string strExp, const Expression &e);
  Expression operator - (const Expression &e);
  Expression operator + (const Expression &e1, const Expression &e2);
  Expression operator + (const Expression &e, const double c);
  Expression operator + (const double c, const Expression &e);
  Expression operator - (const Expression &e1, const Expression &e2);
  Expression operator - (const Expression &e, const double c);
  Expression operator - (const double c, const Expression &e);
  Expression operator * (const Expression &e1, const Expression &e2);
  Expression operator * (const Expression &e, const double c);
  Expression operator * (const double c, const Expression &e);
  Expression operator ^ (const Expression &e1, const Expression &e2);
  Expression operator ^ (const Expression &e, const double c);
  Expression operator ^ (const double c, const Expression &e);
  Expression operator / (const Expression &e1, const Expression &e2);
  Expression operator / (const Expression &e, const double c);
  Expression operator / (const double c, const Expression &e);
  Expression log (const Expression &o);
  Expression log (const double o);

  std::ostream& operator << (std::ostream& o, const Expression &e);
};

////////////////////////////////////////////////////////////////////////////////
enum class Symbol::Impl_::Operator {
  CONST, VARIABLE, NEGATE, ADD, MULTIPLY, POWER, LOG
};

class Symbol::Impl_::Exp {
#ifdef TEST_IMPL_
public:
#endif
  std::string name_;
  double value_;
  Operator operator_;
  Operands operands_;

public:
  Exp(std::string name,
      Operator oprtr,
      Operands oprnds,
      double val);

  void assertOperationConsistency() const;
  bool isConst() const;
  bool isNegative() const;
  bool isPositive() const;
  bool isZero() const;
  bool isOne() const;

  std::string id(bool bracket=false) const;

  void assign(double value);
  double evaluate() const;

  friend pExpression simplify(pExpression pExp);
  friend pExpression sort(pExpression pExp);
  friend pExpression flatten(pExpression pExp);
  friend pExpression flattenNEGATE(pExpression pExp);
  friend pExpression flattenMultiOperands(pExpression pExp);
  // friend pExpression factor(pExpression pExp);
  friend pExpression expand(pExpression pExp);
  friend pExpression expandNEGATE(pExpression pExp);
  friend pExpression expandMULTIPLY(pExpression pExp);
  friend pExpression expandPOWER(pExpression pExp);
  friend pExpression expandLOG(pExpression pExp);
  friend pExpression merge(pExpression pExp);
  friend pExpression mergeADD(pExpression pExp);
  friend pExpression mergeMULTIPLY(pExpression pExp);
  friend pExpression mergePOWER(pExpression pExp);
  friend pExpression mergeLOG(pExpression pExp);

  friend Operands decompose2(pExpression pExp);
  friend Operands decompose3(pExpression pExp);

  friend pExpression differentiate(pExpression dy, Operand dx);

  friend Expression;
};

class Symbol::Expression {
  pExpression pExp_;
  Expression(pExpression pExp);

public:
  Expression(double constant);
  Expression(const std::string name, const double c = NAN);

  Expression differentiate(const Expression &dx);

  friend bool operator == (const Expression &e1, const Expression &e2);
  friend bool operator == (const Expression &e, const std::string strExp);
  friend bool operator == (const std::string strExp, const Expression &e);

  friend Expression operator - (const Expression &e);
  friend Expression operator + (const Expression &e1, const Expression &e2);
  friend Expression operator * (const Expression &e1, const Expression &e2);
  friend Expression operator ^ (const Expression &e1, const Expression &e2);
  friend Expression operator / (const Expression &e1, const Expression &e2);
  friend Expression log (const Expression &e);

  Expression& assign(double value);
  double evaluate() const;

  friend std::ostream& operator << (std::ostream& o, const Expression &e);
};

////////////////////////////////////////////////////////////////////////////////
// Default values
namespace {
  struct defaultCompare {
    inline bool operator() (const pExpression& e1, const pExpression& e2) const {
      auto const1 = e1->isConst(), const2 = e2->isConst();
      if (const1 && !const2) {
        return true;
      } else if (!const1 && const2) {
        return false;
      } else {
        return e1->id() < e2->id();
      }
    }
  };
}

////////////////////////////////////////////////////////////////////////////////
Symbol::Impl_::Exp::Exp(std::string name,
		Operator oprtr,
		Operands oprnds,
                double value)
  : name_(name)
  , value_(value)
  , operator_(oprtr)
  , operands_(oprnds)
{
  assertOperationConsistency();
}

void Symbol::Impl_::Exp::assertOperationConsistency() const {
  std::string error_message;
  for (auto& operand : operands_) {
    if (!operand)
      error_message = "NULL operand was given.";
  }
  auto nOperands = operands_.size();
  switch(operator_) {
  case Operator::CONST:
   if (nOperands)
     error_message = "CONST Expression must not have operand.";
   break;
  case Operator::VARIABLE:
   if (nOperands)
     error_message = "VARIABLE Expression must not have operand.";
   break;
  case Operator::NEGATE:
    if (nOperands != 1)
      error_message = "NEGATE Expression must have exactly one operand.";
    break;
  case Operator::POWER:
    if (nOperands != 2)
      error_message = "POWER Expression must have two operands.";
    break;
  case Operator::ADD:
    if (nOperands < 2)
      error_message = "ADD Expression must have at least two operands.";
    break;
  case Operator::MULTIPLY:
    if (nOperands < 2)
      error_message = "MULTIPLY Expression must have at least two operands.";
    break;
  case Operator::LOG:
    if (nOperands != 1)
      error_message = "LOG Expression must have only exactly operand.";
    if (operands_[0]->isConst()) {
      if (!operands_[0]->isPositive()) {
        error_message = "Operand of LOG Expression must greater than zero.";
      }
    }
    break;
  }
  if (!error_message.empty()) {
    LOG(ERROR) << error_message;
    throw std::runtime_error(error_message);
  }
};

bool Symbol::Impl_::Exp::isConst() const {
  return Operator::CONST == operator_;
}

bool Symbol::Impl_::Exp::isPositive() const {
  return isConst() && !isZero() && value_ > 0.0;
}

bool Symbol::Impl_::Exp::isNegative() const {
  return isConst() && !isZero() && value_ < 0.0;
}

bool Symbol::Impl_::Exp::isZero() const {
  return isConst() && isNearlyEqual(value_, 0.0);
}

bool Symbol::Impl_::Exp::isOne() const {
  return isConst() && isNearlyEqual(value_, 1.0);
}

pExpression Symbol::Impl_::constructOne() {
  return constructCONST(1);
}

pExpression Symbol::Impl_::constructZero() {
  return constructCONST(0);
}

pExpression Symbol::Impl_::constructCONST(const double c) {
  return MAKE_SHARED_EXP("const", Operator::CONST, Operands {}, c);
}

pExpression Symbol::Impl_::constructVARIABLE(const std::string name, const double c) {
  return MAKE_SHARED_EXP(name, Operator::VARIABLE, Operands {}, c);
}

pExpression Symbol::Impl_::constructNEGATE(const Operand v) {
  return MAKE_SHARED_EXP("negate", Operator::NEGATE, Operands {v}, 0);
}

pExpression Symbol::Impl_::constructADD(const Operands ops) {
  switch(ops.size()) {
  case 0:
    return constructZero();
  case 1:
    return ops[0];
  default:
    return MAKE_SHARED_EXP("add", Operator::ADD, ops, 0);
  }
}

pExpression Symbol::Impl_::constructMULTIPLY(const Operands ops) {
  switch(ops.size()) {
  case 0:
    return constructOne();
  case 1:
    return ops[0];
  default:
    return MAKE_SHARED_EXP("multipy", Operator::MULTIPLY, ops, 0);
  }
}

pExpression Symbol::Impl_::constructPOWER(const Operands ops) {
  return MAKE_SHARED_EXP("power", Operator::POWER, ops, 0);
}

pExpression Symbol::Impl_::constructInverse(const Operand o) {
  return constructPOWER({o, constructCONST(-1)});
}

pExpression Symbol::Impl_::constructLOG(const Operand v) {
  return MAKE_SHARED_EXP("log", Operator::LOG, Operands {v}, 0);
}

pExpression Symbol::Impl_::flatten(const pExpression pExp) {
  switch(pExp->operator_) {
  case Operator::NEGATE:
    return flattenNEGATE(pExp);
  case Operator::ADD:
  case Operator::MULTIPLY:
    return flattenMultiOperands(pExp);
  default:
    return pExp;
  }
}

/// -(-X) -> X
/// -C1 -> C2
pExpression Symbol::Impl_::flattenNEGATE(pExpression pExp) {
  if (Operator::NEGATE != pExp->operator_) {
    auto msg = "flattenNEGATE called on non-NEGATE expression.";
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }
  Operand innerOperand = pExp->operands_[0];
  switch (innerOperand->operator_) {
  case Operator::NEGATE:
    return innerOperand->operands_[0];
  case Operator::CONST:
    return constructCONST(-innerOperand->value_);
  default:
    return pExp;
  }
}

/// ((X + Y) + Z) -> (X + Y + Z)
/// (X * (Y * Z)) -> (X * Y * Z)
pExpression Symbol::Impl_::flattenMultiOperands(pExpression pExp) {
  if (Operator::ADD != pExp->operator_ &&
      Operator::MULTIPLY != pExp->operator_) {
    auto msg = "flattenMultiOperands must be called on ADD or MULTIPLY Expression.";
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }
  Operands newOperands;
  for (auto& operand_ : pExp->operands_) {
    if (operand_->operator_ != pExp->operator_) {
      newOperands.push_back(operand_);
    } else {
      newOperands.insert(newOperands.end(),
                         operand_->operands_.begin(),
                         operand_->operands_.end());
    }
  }
  return MAKE_SHARED_EXP(pExp->name_, pExp->operator_, newOperands, 0);
}

pExpression Symbol::Impl_::sort(pExpression pExp) {
  if (Operator::POWER != pExp->operator_) {
    std::sort(pExp->operands_.begin(), pExp->operands_.end(), defaultCompare());
  }
  return pExp;
}

pExpression Symbol::Impl_::expand(pExpression pExp) {
  switch(pExp->operator_) {
  case Operator::NEGATE:
    return expandNEGATE(pExp);
  case Operator::MULTIPLY:
    return expandMULTIPLY(pExp);
  case Operator::POWER:
    return expandPOWER(pExp);
  case Operator::LOG:
    return expandLOG(pExp);
  default:
    return pExp;
  }
}

/// -(X + Y + Z) -> -X -Y -Z
pExpression Symbol::Impl_::expandNEGATE(pExpression pExp) {
  if (Operator::NEGATE != pExp->operator_) {
    auto msg = "expandNEGATE was called on non-NEGATE Expression.";
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }
  Operand innerOperand = pExp->operands_[0];
  switch(innerOperand->operator_) {
  case Operator::ADD: {
    Operands newOperands;
    for (auto& operand_ : innerOperand->operands_) {
      newOperands.push_back(constructNEGATE(operand_));
    }
    return MAKE_SHARED_EXP(innerOperand->name_,
                           innerOperand->operator_,
                           newOperands, innerOperand->value_);
  }
  default:
    return pExp;
  }
}

/// (X + Y) * (A + B)  -> X*A + X*B + Y*A + Y*B
pExpression Symbol::Impl_::expandMULTIPLY(pExpression pExp) {
  if (Operator::MULTIPLY != pExp->operator_) {
    auto msg = "expandMULTIPLY was called on non-MULTIPLY Expression.";
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }
  // Classify operands to ADD type and non-ADD type.
  Operands nonAddOperands;
  std::vector<Operands> addOperandsSet;
  for (auto& operand_ : pExp->operands_) {
    if (operand_->operator_ == Operator::ADD) {
      addOperandsSet.push_back(operand_->operands_);
    } else {
      nonAddOperands.push_back(operand_);
    }
  }
  // If no operand is ADD type, then it's expanded.
  if (0 == addOperandsSet.size()) {
    return pExp;
  }
  // If all the operands are ADD type, moeve one to nonAddOperands.
  if (0 == nonAddOperands.size()) {
    nonAddOperands = addOperandsSet.back();
    addOperandsSet.pop_back();
  }
  // Expand ADD operands one by one onto nonAddOperands.
  for (auto& addOperands : addOperandsSet) {
    Operands newNonAddOperands;
    for (auto& addOperand: addOperands) {
      for (auto& nonAddOperand : nonAddOperands) {
        auto newNonAddOperand = constructMULTIPLY({nonAddOperand, addOperand});
        newNonAddOperands.push_back(newNonAddOperand);
      }
    }
    nonAddOperands = newNonAddOperands;
  }
  // Reconstruct as addition.
  return constructADD(nonAddOperands);
}

/// (X * Y) ^ A -> (X ^ A) * (Y ^ A)
pExpression Symbol::Impl_::expandPOWER(pExpression pExp) {
  if (Operator::POWER != pExp->operator_) {
    auto msg = "expandPOWER was called on non-POWER Expression.";
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }
  auto base = pExp->operands_[0];
  auto expo = pExp->operands_[1];
  if (Operator::MULTIPLY == base->operator_) {
      Operands operands;
      for (auto& operand_ : base->operands_) {
        operands.push_back(constructPOWER({operand_, expo}));
      }
      return constructMULTIPLY(operands);
  }
  if (Operator::CONST == expo->operator_) {
    unsigned int exponent = std::round(expo->value_);
    if (isNearlyEqual(expo->value_, exponent)) {
      Operands operands;
      for (unsigned int i = 0; i < exponent; ++i) {
        auto operand = base;
          operands.push_back(operand);
      }
      return constructMULTIPLY(operands);
    }
  }
  return pExp;
}

/// log(X * Y) -> log(X) + log(Y)
/// log(X ^ Y) -> Y * log(X)
pExpression Symbol::Impl_::expandLOG(pExpression pExp) {
  auto innerOperand = pExp->operands_[0];
  switch(innerOperand->operator_) {
  case Operator::MULTIPLY: {
    Operands operands;
    for (auto& operand : innerOperand->operands_) {
      operands.push_back(constructLOG(operand));
    }
    return constructADD(operands);
  }
  case Operator::POWER:
    return constructMULTIPLY({innerOperand->operands_[1],
          constructLOG(innerOperand->operands_[0])});
  default:
    return pExp;
  }
}

pExpression Symbol::Impl_::merge(pExpression pExp) {
  switch(pExp->operator_) {
  case Operator::ADD:
    return mergeADD(pExp);
  case Operator::MULTIPLY:
    return mergeMULTIPLY(pExp);
  case Operator::POWER:
    return mergePOWER(pExp);
  case Operator::LOG:
    return mergeLOG(pExp);
  default:
    return pExp;
  }
}

/// Merge constant terms and the coefficients of non-constant terms
/// ex) C1 + X + X + Y + C2 -> (2 * X) + Y + (C1 + C2)
pExpression Symbol::Impl_::mergeADD(pExpression pExp) {
  if (Operator::ADD != pExp->operator_) {
    auto msg = "mergeADD was called ont non ADD Expression.";
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }
  std::map<pExpression, double, defaultCompare> operandCounts;
  // Classify operands to coefficient and non-coefficient parts
  auto constTerm = constructZero();
  for (auto& operand_ : pExp->operands_) {
    if (operand_->isConst()) {
      constTerm->value_ += operand_->value_;
    } else {
      auto elems = decompose2(operand_);
      auto coeff = elems[0];
      auto nonCoeff = elems[1];
      operandCounts[nonCoeff] += coeff->value_;
    }
  }
  if (!constTerm->isZero()) {
    operandCounts[constTerm] = 1;
  }
  // Reconstruct Expression
  Operands operands;
  for (auto& entry : operandCounts) {
    if (isNearlyEqual(entry.second, 0.0)) {
      // coeff == 0: X - X -> 0
      continue;
    } else if (isNearlyEqual(entry.second, 1.0)) {
      // coeff == 1: X + X - X -> X
      operands.push_back(entry.first);
    } else if (isNearlyEqual(entry.second, -1.0)) {
      // coeff == -1: X - X - X -> - X
      operands.push_back(constructNEGATE(entry.first));
    } else {
      Operand coeff = constructCONST(entry.second);
      operands.push_back(constructMULTIPLY({coeff, entry.first}));
    }
  }
  return constructADD(operands);
}

/// Merge constant terms and the exponents of non-constant terms.
/// ex) C1 * X * Y * X * C2 -> (C1 * C2) * (X ^ 2) * Y
pExpression Symbol::Impl_::mergeMULTIPLY(pExpression pExp) {
  if (Operator::MULTIPLY != pExp->operator_) {
    auto msg = "mergeMULTIPLY was called on non-MULTIPLY Expression.";
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }
  auto constOperand = constructOne();
  std::map<pExpression, pExpression, defaultCompare> nonConstOperands;
  // Classify operands to coefficient, base and exponent
  for (auto& operand_ : pExp->operands_) {
    auto elems = decompose3(operand_);
    auto coeff = elems[0]->value_;
    auto base = elems[1];
    auto exponent = elems[2];
    constOperand->value_ *= coeff;
    if (nonConstOperands[base]) {
      nonConstOperands[base] = nonConstOperands[base] + exponent;
    } else {
      nonConstOperands[base] = exponent;
    }
  }
  if (isNearlyEqual(constOperand->value_, 0,0)) {
    return constructZero();
  }
  // Reconstruct Expression.
  Operands operands;
  // Push back the constant term
  if (!isNearlyEqual(constOperand->value_, 1.0)) {
    operands.push_back(constOperand);
  }
  // Push back the other term
  for (auto& entry : nonConstOperands) {
    if (entry.second->isZero()) {
      // exponent == 0: X ^ 0 -> 1
      continue;
    } else if (entry.second->isOne()) {
      // exponent == 1: X ^ 1 -> X
      operands.push_back(entry.first);
    } else {
      operands.push_back(constructPOWER({entry.first, entry.second}));
    }
  }
  return constructMULTIPLY(operands);
}

/// Merger POWER expression
/// ex)
///   C1 ^ C2 -> C3
///   1 ^ X -> 1
///   X ^ 0 -> 1
///   X ^ 1 -> X
pExpression Symbol::Impl_::mergePOWER(pExpression pExp) {
  if (Operator::POWER != pExp->operator_) {
    auto msg = "mergePOWER called on non-POWER Expression.";
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }
  auto base = pExp->operands_[0];
  auto expo = pExp->operands_[1];
  if (Operator::CONST == base->operator_ &&
      Operator::CONST == expo->operator_) {
    return constructCONST(std::pow(base->value_, expo->value_));
  }
  if (base->isOne() || expo->isZero()) {
    return constructOne();
  }
  if (expo->isOne()) {
    return base;
  }
  return pExp;
}

/// log(1) -> 0
pExpression Symbol::Impl_::mergeLOG(pExpression pExp) {
  if (Operator::LOG != pExp->operator_) {
    auto msg = "mergeLOG called on non-LOG Expression.";
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }
  if (pExp->operands_[0]->isOne()) {
    return constructZero();
  } else {
    return pExp;
  }
}

/// Decompose an Expression into coefficient and non-coefficient part
/// such that pExp == coeff * non-coeff
/// This function is made for simplifying the mergeADD.
/// This decomposition is not necessarily generally apprecable.
Operands Symbol::Impl_::decompose2(Operand o) {
  switch (o->operator_) {
  case Operator::CONST:
    return {o, constructOne()};
  case Operator::ADD:
  case Operator::POWER:
  case Operator::VARIABLE:
  case Operator::LOG:
    return {constructOne(), o};
  case Operator::NEGATE: {
    Operands ret = decompose2(o->operands_[0]);
    ret[0]->value_ *= -1;
    return ret;
  }
  case Operator::MULTIPLY: {
    double constant = 1;
    Operands operands;
    for (auto& operand : o->operands_) {
      if (operand->isConst()) {
        constant *= operand->value_;
      } else {
        operands.push_back(operand);
      }
    }
    return {constructCONST(constant), constructMULTIPLY(operands)};
  }
  }
};

/// Decompose an Expression into coefficient, base and exponent
/// so that pExp == coeff * (base ^ expo)
/// This function is made for simplifying the mergeMULTIPLY.
/// This decomposition is not necessarily generally apprecable.
Operands Symbol::Impl_::decompose3(Operand o) {
  switch (o->operator_) {
  case Operator::CONST:
    return {o, constructZero(), constructZero()};
  case Operator::VARIABLE:
  case Operator::LOG:
    return {constructOne(), o, constructOne()};
  case Operator::NEGATE: {
    Operands ret = decompose3(o->operands_[0]);
    ret[0]->value_ *= -1;
    return ret;
  }
  case Operator::ADD:
    return {constructOne(), o, constructOne()};
  case Operator::MULTIPLY: {
    double constant = 1;
    Operands operands;
    for (auto& operand : o->operands_) {
      if (operand->isConst()) {
        constant *= operand->value_;
      } else {
        operands.push_back(operand);
      }
    }
    return {constructCONST(constant), constructMULTIPLY(operands), constructOne()};
  }
  case Operator::POWER:
    return {constructOne(), o->operands_[0], o->operands_[1]};
  }
};

pExpression Symbol::Impl_::simplify(pExpression pExp) {
  std::transform(pExp->operands_.begin(), pExp->operands_.end(),
		 pExp->operands_.begin(), Impl_::simplify);
  std::string before;
  do {
    before = pExp->id();
    pExp = flatten(pExp);
    pExp = expand(pExp);
  }
  while(before != pExp->id());
  do {
    before = pExp->id();
    pExp = merge(pExp);
  }
  while(before != pExp->id());
  pExp = sort(pExp);
  return pExp;
}

pExpression Symbol::Impl_::differentiate(pExpression y, Operand x) {
  if (Operator::CONST == x->operator_) {
    auto msg = "Cannot differentiate with CONST Expression.";
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }
  if ((y - x)->isZero()) {
    return constructOne();
  }
  switch (y->operator_) {
  case Operator::CONST:
  case Operator::VARIABLE:
    return constructZero();
  case Operator::NEGATE:
    return constructNEGATE(differentiate(y->operands_[0], x));
  case Operator::ADD: {
    Operands operands;
    for (auto& operand : y->operands_) {
      operands.push_back(differentiate(operand, x));
    }
    return constructADD(operands);
  }
  case Operator::MULTIPLY: {
    Operands operandsSet;
    for (size_t i = 0; i < y->operands_.size(); ++i) {
      Operands operands;
      for (size_t j = 0; j < y->operands_.size(); ++j) {
        if (i == j) {
          operands.push_back(differentiate(y->operands_[j], x));
        } else {
          operands.push_back(y->operands_[j]);
        }
      }
      operandsSet.push_back(constructMULTIPLY(operands));
    }
    return constructADD(operandsSet);
  }
  case Operator::POWER: {
    auto f = y->operands_[0];
    auto g = y->operands_[1];
    auto fp = differentiate(f, x);
    auto gp = differentiate(g, x);
    return (f ^ g) * (fp * g / f + gp * log(f));
  }
  case Operator::LOG: {
    auto f = y->operands_[0];
    auto fp = differentiate(f, x);
    return fp / f;
  }
  }
}

std::string Symbol::Impl_::Exp::id(bool bracket) const {
  std::string ret;
  switch(operator_) {
  case Operator::CONST:
    if (isZero()) {
      return "0";
    } else if (isOne()) {
      return "1";
    } else {
      std::stringstream ss;
      int roundedValue = std::round(value_);
      if (isNearlyEqual(value_, roundedValue)) {
        ss << std::fixed << std::setprecision(0) << std::abs(value_);
      } else {
        ss << std::fixed << std::setprecision(3) << std::abs(value_);
      }
      if (isNegative()) {
        ret = " - " + ss.str();
      } else {
        return ss.str();
      }
    }
    break;
  case Operator::VARIABLE:
    return name_;
  case Operator::NEGATE:
    ret = " - " + operands_[0]->id(true);
    break;
  case Operator::ADD:
    for (auto& operand : operands_) {
      std::string append;
      if (operand->isNegative() ||
          operand->operator_ == Operator::NEGATE) {
        append = operand->id(false);
        ret += append;
      } else {
        append = operand->id(true);
        if (0 != ret.size())
          ret += " + ";
        ret += append;
      }
    }
    break;
  case Operator::MULTIPLY:
    for (auto& operand : operands_) {
      auto append = operand->id(true);
      if (0 != ret.size()) {
        ret += " * ";
      }
      ret += append;
    }
    break;
  case Operator::POWER: {
    auto base = operands_[0]->id(true);
    auto expo = operands_[1]->id(true);
    std::string ret = base + " ^ " + expo;
    if (bracket) {
      ret = "(" + ret + ")";
    }
    return ret;
  }
  case Operator::LOG:
    return "log(" + operands_[0]->id(false) + ")";
  }
  if (bracket) {
    ret = "(" + ret + ")";
  }
  return ret;
}

void Symbol::Impl_::Exp::assign(double value) {
  switch(operator_) {
  case Operator::CONST:
  case Operator::VARIABLE:
    value_ = value;
    break;
  default:
    auto msg = "Cannot assign value to compound expressions.";
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }
}

double Symbol::Impl_::Exp::evaluate() const {
  switch(operator_) {
  case Operator::CONST:
  case Operator::VARIABLE:
    return value_;
  case Operator::NEGATE:
    return -operands_[0]->evaluate();
  case Operator::ADD: {
    double ret = 0;
    for (auto& operand_ : operands_) {
      ret += operand_->evaluate();
    }
    return ret;
  }
  case Operator::MULTIPLY: {
    double ret = 1;
    for (auto& operand_ : operands_) {
      ret *= operand_->evaluate();
    }
    return ret;
  }
  case Operator::POWER: {
    return std::pow(operands_[0]->evaluate(),
                    operands_[1]->evaluate());
  }
  case Operator::LOG:
    return std::log(operands_[0]->evaluate());
  }
}

pExpression Symbol::Impl_::operator - (const Operand v) {
  return simplify(constructNEGATE(v));
}

pExpression Symbol::Impl_::operator + (const Operand v1, const Operand v2) {
  return simplify(constructADD({v1, v2}));
}

pExpression Symbol::Impl_::operator - (const Operand v1, const Operand v2) {
  return simplify(constructADD({v1, -v2}));
}

pExpression Symbol::Impl_::operator * (const Operand o1, const Operand o2) {
  return simplify(constructMULTIPLY({o1, o2}));
}

pExpression Symbol::Impl_::operator ^ (const Operand o1, const Operand o2) {
  return simplify(constructPOWER({o1, o2}));
}

pExpression Symbol::Impl_::operator / (const Operand o1, const Operand o2) {
  return simplify(constructMULTIPLY({o1, constructInverse(o2)}));
}

pExpression Symbol::Impl_::log (const Operand o) {
  return simplify(constructLOG(o));
}

////////////////////////////////////////////////////////////////////////////////
Symbol::Expression::Expression(double c)
  : pExp_(Impl_::constructCONST(c))
{}

Symbol::Expression::Expression(const std::string name, const double c)
  : pExp_(Impl_::constructVARIABLE(name, c))
{}

Symbol::Expression::Expression(pExpression exp)
  : pExp_(exp)
{}

bool Symbol::operator == (const Expression& e1, const Expression& e2) {
  return (e1.pExp_ - e2.pExp_)->isZero();
}

bool Symbol::operator == (const Expression& e, const std::string strExp) {
  return e.pExp_->id(false) == strExp;
}

bool Symbol::operator == (const std::string strExp, const Expression& e) {
  return e == strExp;
}

bool Symbol::operator == (const Expression &e, const double c) {
  if (std::isnan(c)) {
    return std::isnan(e.evaluate());
  }
  return e.evaluate() == c;
}

bool Symbol::operator == (const double c, const Expression &e) {
  return e == c;
}

bool Symbol::operator != (const Expression& e1, const Expression& e2) {
  return !(e1 == e2);
}

bool Symbol::operator != (const Expression& e, const std::string strExp) {
  return !(e == strExp);
}

bool Symbol::operator != (const std::string strExp, const Expression& e) {
  return !(e == strExp);
}

Symbol::Expression Symbol::operator - (const Expression& e) {
  return - e.pExp_;
}

Symbol::Expression Symbol::operator + (const Expression& e1, const Expression& e2) {
  return e1.pExp_ + e2.pExp_;
}

Symbol::Expression Symbol::operator + (const Expression& e, const double c) {
  return e + Expression(c);
}

Symbol::Expression Symbol::operator + (const double c, const Expression& e) {
  return e + Expression(c);
}

Symbol::Expression Symbol::operator - (const Expression& e1, const Expression& e2) {
  return e1 + (-e2);
}

Symbol::Expression Symbol::operator - (const Expression& e, const double c) {
  return e - Expression(c);
}

Symbol::Expression Symbol::operator - (const double c, const Expression& e) {
  return Expression(c) - e;
}

Symbol::Expression Symbol::operator * (const Expression& e1, const Expression& e2) {
  return e1.pExp_ * e2.pExp_;
}

Symbol::Expression Symbol::operator * (const double c, const Expression& e) {
  return Expression(c) * e;
}

Symbol::Expression Symbol::operator * (const Expression& e, const double c) {
  return Expression(c) * e;
}

Symbol::Expression Symbol::operator ^ (const Expression& e1, const Expression& e2) {
  return e1.pExp_ ^ e2.pExp_;
}

Symbol::Expression Symbol::operator ^ (const double c, const Expression& e) {
  return Expression(c) ^ e;
}

Symbol::Expression Symbol::operator ^ (const Expression& e, const double c) {
  return e ^ Expression(c);
}

Symbol::Expression Symbol::operator / (const Expression& e1, const Expression& e2) {
  return e1.pExp_ / e2.pExp_;
}

Symbol::Expression Symbol::operator / (const double c, const Expression& e) {
  return Expression(c) / e;
}

Symbol::Expression Symbol::operator / (const Expression& e, const double c) {
  return e / Expression(c);
}

Symbol::Expression Symbol::log (const Expression& e) {
  return Impl_::log(e.pExp_);
}

Symbol::Expression Symbol::log (const double c) {
  return log(Expression(c));
}

Symbol::Expression Symbol::Expression::differentiate(const Expression& dx) {
  return Impl_::simplify(Impl_::differentiate(pExp_, dx.pExp_));
}

Symbol::Expression& Symbol::Expression::assign(double value) {
  pExp_->assign(value);
  return *this;
}

double Symbol::Expression::evaluate() const {
  return pExp_->evaluate();
}

std::ostream& Symbol::operator <<(std::ostream& o, const Expression &e) {
  return o << e.pExp_->id(false);
}
