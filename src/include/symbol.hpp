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
  return std::abs(x - y) <= epsilon * std::abs(x);
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
    pExpression constructVARIABLE(const std::string name);
    pExpression constructNEGATE(const Operand o);
    pExpression constructADD(const Operands ops);
    pExpression constructMULTIPLY(const Operands ops);
    pExpression constructPOWER(const Operands ops);

    pExpression flatten(const pExpression &pExp);
    pExpression merge(const pExpression &pExp);
    pExpression sort(const pExpression &pExp,
                     bool(*cmpr)(const pExpression&, const pExpression&)=NULL);
    pExpression expand(const pExpression &pExp);
    pExpression factor(const pExpression &pExp);

    pExpression mergeADD(const pExpression &pExp);

    Operands decompose2(const pExpression &pExp);
    Operands decompose3(const pExpression &pExp);

    pExpression simplify(const pExpression &pExp);

    pExpression operator - (const Operand o);
    pExpression operator + (const Operand o1, const Operand o2);
    pExpression operator - (const Operand o1, const Operand o2);
    pExpression operator * (const Operand o1, const Operand o2);
    pExpression operator ^ (const Operand o1, const Operand o2);
  };

  bool operator == (const Expression &e1, const Expression &e2);
  bool operator == (const Expression &e, const std::string strExp);
  bool operator == (const std::string strExp, const Expression &e);
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

  std::ostream& operator << (std::ostream& o, const Expression &e);
};

////////////////////////////////////////////////////////////////////////////////
enum class Symbol::Impl_::Operator {
  CONST, VARIABLE, NEGATE, ADD, MULTIPLY, POWER
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
  Exp(std::string name = "const",
      Operator oprtr = Operator::CONST,
      Operands oprnds = {},
      double val = 0);

  void assertOperationConsistency() const;
  bool isConst() const;
  bool isZero() const;
  bool isOne() const;

  std::string id() const;

  friend Expression;

  friend pExpression flatten(const pExpression &pExp);
  friend pExpression merge(const pExpression &pExp);
  friend pExpression sort(const pExpression &pExp,
			  bool(*compare)(const pExpression&, const pExpression&));

  friend pExpression expand(const pExpression &pExp);
  friend pExpression factor(const pExpression &pExp);

  friend pExpression mergeADD(const pExpression &pExp);

  friend Operands decompose2(const pExpression &pExp);
  friend Operands decompose3(const pExpression &pExp);
};

class Symbol::Expression {
  pExpression pExp_;
  Expression(pExpression pExp);

public:
  Expression(double constant);
  Expression(std::string name);

  Expression expand() const;
  Expression factor() const;

  friend bool operator == (const Expression &e1, const Expression &e2);
  friend bool operator == (const Expression &e, const std::string strExp);
  friend bool operator == (const std::string strExp, const Expression &e);

  friend Expression operator - (const Expression &e);
  friend Expression operator + (const Expression &e1, const Expression &e2);
  friend Expression operator * (const Expression &e1, const Expression &e2);
  friend Expression operator ^ (const Expression &e1, const Expression &e2);

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
  auto nOperands = operands_.size();
  switch(operator_) {
  case Operator::CONST:
   if (nOperands) {
     LOG(FATAL) << "CONST Expression must not have operand.";
   }
   break;
  case Operator::VARIABLE:
   if (nOperands) {
     LOG(FATAL) << "VARIABLE Expression must not have operand.";
   }
   break;
  case Operator::NEGATE:
    if (nOperands != 1) {
      LOG(FATAL) << "NEGATE Expression must have only one operand.";
    }
    break;
  case Operator::POWER:
    if (nOperands != 2) {
      LOG(FATAL) << "POWER Expression must have two operands.";
    }
    break;
  case Operator::ADD:
    if (nOperands < 2) {
      LOG(FATAL) << "ADD Expression must have at least two operands.";
    }
    break;
  case Operator::MULTIPLY:
    if (nOperands < 2) {
      LOG(FATAL) << "MULTIPLY Expression must have at least two operands.";
    }
    break;
  }
};

bool Symbol::Impl_::Exp::isConst() const {
  return Operator::CONST == operator_;
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

pExpression Symbol::Impl_::constructVARIABLE(const std::string name) {
  return MAKE_SHARED_EXP(name, Operator::VARIABLE);
}

pExpression Symbol::Impl_::constructNEGATE(const Operand v) {
  return MAKE_SHARED_EXP("negate", Operator::NEGATE, Operands {v});
}

pExpression Symbol::Impl_::constructADD(const Operands ops) {
  switch(ops.size()) {
  case 0:
    return constructZero();
  case 1:
    return ops[0];
  default:
    return MAKE_SHARED_EXP("add", Operator::ADD, ops);
  }
}

pExpression Symbol::Impl_::constructMULTIPLY(const Operands ops) {
  switch(ops.size()) {
  case 0:
    return constructOne();
  case 1:
    return ops[0];
  default:
    return MAKE_SHARED_EXP("multipy", Operator::MULTIPLY, ops);
  }
}

pExpression Symbol::Impl_::constructPOWER(const Operands ops) {
  return MAKE_SHARED_EXP("power", Operator::POWER, ops);
}

pExpression Symbol::Impl_::flatten(const pExpression& pExp) {
  // Flatten the child operands of this expression.
  pExpression ret = pExp;
  std::transform(ret->operands_.begin(), ret->operands_.end(),
		 ret->operands_.begin(), Impl_::flatten);
  // Flatten this expression.
  switch(ret->operator_) {
  case Operator::NEGATE:
    {
      Operand innerOperand = ret->operands_[0];
      switch (innerOperand->operator_) {
      case Operator::NEGATE:  // Remove double negation.
        return innerOperand->operands_[0];
      case Operator::CONST:   // Put negation into constant
        return constructCONST(-innerOperand->value_);
      default:
        return ret;
      }
    }
  case Operator::ADD:
  case Operator::MULTIPLY:
    {  // Unnest the operands of the same operation type.
      Operands newOperands;
      for (auto& operand_ : ret->operands_) {
	if (operand_->operator_ == ret->operator_) {
	  newOperands.insert(newOperands.end(),
			     operand_->operands_.begin(),
			     operand_->operands_.end());
	} else {
	  newOperands.push_back(operand_);
	}
      }
      return MAKE_SHARED_EXP(ret->name_, ret->operator_, newOperands);
    }
  default:
    return ret;
  }
}

pExpression Symbol::Impl_::merge(const pExpression& pExp) {
  // Merge the child operands of this expression.
  pExpression ret = pExp;
  std::transform(ret->operands_.begin(), ret->operands_.end(),
		 ret->operands_.begin(), Impl_::merge);
  // Merge this expression.
  switch(ret->operator_) {
  case Operator::ADD: {
    // return mergeADD(ret);
    // Merge constant terms and the coefficients of non-constant terms
    std::map<pExpression, double, defaultCompare> operandCounts;
    for (auto& operand_ : ret->operands_) {
      auto elems = decompose2(operand_);
      auto coeff = elems[0];
      auto nonCoeff = elems[1];
      operandCounts[nonCoeff] += coeff->value_;
    }
    // Reconstruct operands
    Operands operands;
    for (auto& entry : operandCounts) {
      if (isNearlyEqual(entry.second, 0.0)) {
        continue;
      }
      if (isNearlyEqual(entry.second, 1.0)) {
        operands.push_back(entry.first);
      } else if (isNearlyEqual(entry.second, -1.0)) {
        operands.push_back(constructNEGATE(entry.first));
      } else {
        operands.push_back(
          constructMULTIPLY({constructCONST(entry.second), entry.first}));
      }
    }
    // Reconstruct Expression.
    return constructADD(operands);
  }
  case Operator::MULTIPLY: {
    // Merge constant terms and the exponents of non-constant terms.
    auto constOperand = constructOne();
    std::map<pExpression, pExpression, defaultCompare> nonConstOperands;
    for (auto& operand_ : ret->operands_) {
      auto elems = decompose3(operand_);
      auto coeff = elems[0]->value_;
      auto base = elems[1];
      auto exponent = elems[2];
      constOperand->value_ *= coeff;
      if (nonConstOperands.count(base)) {
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
    bool isConstNegative = std::signbit(constOperand->value_);
    constOperand->value_ = std::abs(constOperand->value_);
    if (!isNearlyEqual(constOperand->value_, 1.0)) {
      operands.push_back(constOperand);
    }
    // Push back the other term
    for (auto& entry : nonConstOperands) {
      if (entry.second->isZero()) {
        continue;
      } else if (entry.second->isOne()) {
        operands.push_back(entry.first);
      } else {
        operands.push_back(constructPOWER({entry.first, entry.second}));
      }
    }
    switch(operands.size()) {
    case 0:
      if (isConstNegative) {
        return constructCONST(-1);
      } else {
        return constructOne();
      }
    case 1:
      if (isConstNegative) {
        return constructNEGATE(operands[0]);
      }
      return operands[0];
    default:
      ret = constructMULTIPLY(operands);
      if (isConstNegative) {
        ret = constructNEGATE(ret);
      }
      return ret;
    }
  }
  case Operator::POWER: {
    auto base = ret->operands_[0];
    auto exp = ret->operands_[1];
    if (exp->isZero()) {
      return constructOne();
    }
    if (exp->isOne()) {
      return base;
    }
    if (Operator::CONST == base->operator_ &&
        Operator::CONST == base->operator_) {
      return constructCONST(std::pow(base->value_, exp->value_));
    }
    return ret;
  }
  default:
    return ret;
  }
}

pExpression Symbol::Impl_::sort(const pExpression&pExp,
			bool(*cmpr)(const pExpression&, const pExpression&)) {
  // Sort the child operands of this expression.
  pExpression ret = pExp;
  using namespace std::placeholders;
  std::transform(ret->operands_.begin(), ret->operands_.end(),
		 ret->operands_.begin(), std::bind(Impl_::sort, _1, cmpr));
  // Sort this expression
  if (Operator::POWER == pExp->operator_) {
    return ret;
  }
  if (!cmpr) {
    std::sort(ret->operands_.begin(), ret->operands_.end(), defaultCompare());
  } else {
    std::sort(ret->operands_.begin(), ret->operands_.end(), cmpr);
  }
  return ret;
}

// -(X + Y + Z) -> -X -Y -Z
// (X + Y) * (A + B)  -> X*A + X*B + Y*A + Y*B
// (X * Y) ^ A -> (X ^ A) * (Y ^ A)
pExpression Symbol::Impl_::expand(const pExpression& pExp) {
  // Expand each operand of this expression
  pExpression ret = pExp;
  std::transform(ret->operands_.begin(), ret->operands_.end(),
		 ret->operands_.begin(), Impl_::expand);
  // Expand this expression
  switch(ret->operator_) {
  case Operator::NEGATE: {
    Operand innerOperand = ret->operands_[0];
    switch(innerOperand->operator_) {
    case Operator::ADD: {
      Operands newOperands;
      for (auto& operand_ : innerOperand->operands_) {
        newOperands.push_back(constructNEGATE(operand_));
      }
      return MAKE_SHARED_EXP(innerOperand->name_, innerOperand->operator_,
                             newOperands, innerOperand->value_);
    }
    default:
      return ret;
    }
  }
  case Operator::MULTIPLY: {
    // Separate ADD type operands and the other type operands.
    Operands nonAddOperands;
    std::vector<Operands> addOperandsSet;
    for (auto& operand_ : ret->operands_) {
      if (operand_->operator_ == Operator::ADD) {
	addOperandsSet.push_back(operand_->operands_);
      } else {
	nonAddOperands.push_back(operand_);
      }
    }
    // If no operand is ADD type, then it's expanded.
    if (0 == addOperandsSet.size()) {
      return ret;
    }
    // If all the operands are ADD type, moeve one to nonAddOperands.
    if (0 == nonAddOperands.size()) {
      nonAddOperands = addOperandsSet.back();
      addOperandsSet.pop_back();
    }
    // Expand ADD operands one by one.
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
  case Operator::POWER: {
    auto base = ret->operands_[0];
    auto exponent = ret->operands_[1];
    if (Operator::MULTIPLY == base->operator_) {
      Operands operands;
      for (auto& operand_ : base->operands_) {
        operands.push_back(constructPOWER({operand_, exponent}));
      }
      return constructMULTIPLY(operands);
    }
    if (Operator::CONST == exponent->operator_) {
      unsigned int expoConst = std::round(exponent->value_);
      if (isNearlyEqual(exponent->value_, expoConst)) {
        Operands operands;
        for (unsigned int i = 0; i < expoConst; ++i) {
          auto operand = base;
          operands.push_back(operand);
        }
        return constructMULTIPLY(operands);
      }
    }
  }
  default:
    return ret;
  }
}

pExpression Symbol::Impl_::factor(const pExpression& pExp) {
/*
  // Factor each operand of this expression
  pExpression ret = pExp;
  for (size_t i=0; i < ret->operands_.size(); ++i) {
    ret->operands_[i] = factor(ret->operands_[i]);
  }
  ret = flatten(ret);
  // Factor this expression
  switch(ret->operator_) {
  case Operator::ADD:
    {
      for (auto& operand_ : ret->operands_) {
      }
      return ret;
    }
  default:
    return ret;
  }
*/
  return pExp;
}

pExpression Symbol::Impl_::mergeADD(const pExpression& pExp) {
  if (Operator::ADD != pExp->operator_) {
    LOG(FATAL) << "mergeADD was called ont non ADD Expression.";
  }
  // Merge constant terms and the coefficients of non-constant terms
  std::map<pExpression, double, defaultCompare> operandCounts;
  for (auto& operand_ : pExp->operands_) {
    auto elems = decompose2(operand_);
    auto coeff = elems[0];
    auto nonCoeff = elems[1];
    operandCounts[nonCoeff] += coeff->value_;
  }
  // Reconstruct Expression
  Operands operands;
  for (auto& entry : operandCounts) {
    if (isNearlyEqual(entry.second, 0.0)) {
      continue;
    } else if (isNearlyEqual(entry.second, 1.0)) {
      operands.push_back(entry.first);
    } else if (isNearlyEqual(entry.second, -1.0)) {
      operands.push_back(constructNEGATE(entry.first));
    } else {
      Operand coeff = constructCONST(entry.second);
      operands.push_back(constructMULTIPLY({coeff, entry.first}));
    }
  }
  return constructADD(operands);
}


/// Decompose an Expression into coefficient and non-coefficient part
/// such that pExp == coeff * non-coeff
/// This function is made for simplifying the mergeADD.
/// This decomposition is not necessarily generally apprecable.
Operands Symbol::Impl_::decompose2(const pExpression &pExp) {
  switch (pExp->operator_) {
  case Operator::CONST:
    return {pExp, constructOne()};
  case Operator::ADD:
  case Operator::POWER:
  case Operator::VARIABLE:
    return {constructOne(), pExp};
  case Operator::NEGATE: {
    Operands ret = decompose2(pExp->operands_[0]);
    ret[0]->value_ *= -1;
    return ret;
  }
  case Operator::MULTIPLY: {
    double constant = 1;
    Operands operands;
    for (auto& operand : pExp->operands_) {
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
/// This function is made for simplifying the merge method.
/// This decomposition is not necessarily generally apprecable.
Operands Symbol::Impl_::decompose3(const pExpression &pExp) {
  switch (pExp->operator_) {
  case Operator::CONST:
    return {pExp, constructZero(), constructZero()};
  case Operator::VARIABLE:
    return {constructOne(), pExp, constructOne()};
  case Operator::NEGATE: {
    Operands ret = decompose3(pExp->operands_[0]);
    ret[0]->value_ *= -1;
    return ret;
  }
  case Operator::ADD:
    return {constructOne(), pExp, constructOne()};
  case Operator::MULTIPLY: {
    double constant = 1;
    Operands operands;
    for (auto& operand : pExp->operands_) {
      if (operand->isConst()) {
        constant *= operand->value_;
      } else {
        operands.push_back(operand);
      }
    }
    return {constructCONST(constant), constructMULTIPLY(operands), constructOne()};
  }
  case Operator::POWER:
    return {constructOne(), pExp->operands_[0], pExp->operands_[1]};
  }
};

pExpression Symbol::Impl_::simplify(const pExpression& pExp) {
  pExpression ret = pExp;
  std::string before;
  do {
    before = ret->id();
    ret = flatten(ret);
    ret = expand(ret);
    ret = merge(ret);
    ret = sort(ret);
  }
  while(before != ret->id());
  return ret;
}

std::string Symbol::Impl_::Exp::id() const {
  switch(operator_) {
  case Operator::CONST: {
    if (isZero()) {
      return "0";
    } else if (isOne()) {
      return "1";
    } else {
      std::stringstream ss;
      int roundedValue = std::round(value_);
      if (isNearlyEqual(value_, roundedValue)) {
        ss << std::fixed << std::setprecision(0) << value_;
      } else {
        ss << std::fixed << std::setprecision(3) << value_;
      }
      return ss.str();
    }
  }
  case Operator::VARIABLE:
    return name_;
  case Operator::NEGATE:
    return " - " + operands_[0]->id();
  case Operator::ADD:
    {
      std::string ret;
      for (auto& operand : operands_) {
        auto append = operand->id();
	if (0 != ret.size()) {
          if (append.compare(0, 3, " - ")) {
            ret += " + ";
          }
	}
	ret += append;
      }
      return "(" + ret + ")";
    }
  case Operator::MULTIPLY:
    {
      std::string ret;
      for (auto& operand : operands_) {
        auto append = operand->id();
	if (0 != ret.size()) {
          ret += " * ";
	}
	ret += append;
      }
      return "(" + ret + ")";
    }
  case Operator::POWER:
    {
      return "(" + operands_[0]->id() + " ^ " + operands_[1]->id() + ")";
    }
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

////////////////////////////////////////////////////////////////////////////////
Symbol::Expression::Expression(double c)
  : pExp_(Impl_::constructCONST(c))
{}

Symbol::Expression::Expression(std::string name)
  : pExp_(Impl_::constructVARIABLE(name))
{}

Symbol::Expression::Expression(pExpression exp)
  : pExp_(exp)
{}

Symbol::Expression Symbol::Expression::expand() const {
  return Impl_::expand(pExp_);
}

Symbol::Expression Symbol::Expression::factor() const {
  return Impl_::factor(pExp_);
}

bool Symbol::operator == (const Expression& e1, const Expression& e2) {
  return (e1.pExp_ - e2.pExp_)->isZero();
}

bool Symbol::operator == (const Expression& e, const std::string strExp) {
  return e.pExp_->id() == strExp;
}

bool Symbol::operator == (const std::string strExp, const Expression& e) {
  return e == strExp;
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

std::ostream& Symbol::operator <<(std::ostream& o, const Expression &e) {
  return o << e.pExp_->id();
}
