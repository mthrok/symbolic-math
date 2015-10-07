#include <string>
#include <vector>
#include <memory>
#include <iostream>

#define DEBUG

std::string genErrorString(const std::string& message) {
  return message;
}

template<typename... T>
std::string genErrorString(const std::string& message, T... messages) {
  return message + " : " + genErrorString(messages...);
}

#define GEN_DEBUG_INFO genErrorString(std::string(__func__), \
				      std::to_string(__LINE__)) + " : "

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
/** Main classes */
namespace Symbol {

  namespace Impl_ {

    enum class Predicate;

    class Expression;

    typedef std::shared_ptr<Expression> pExpression;
  }

  class Scalar;

  typedef std::shared_ptr<Impl_::Expression> Operand;

  typedef std::vector<Operand> Operands;
};

/** Support functions */
namespace Symbol{

  namespace Impl_{

    std::string pred2str(const Predicate& p);

    Predicate str2pred(const std::string& s);

    std::string expressionName(const Predicate& p, const Operands& o);

    /** Arithmetic operations */
    pExpression operator - (const pExpression& pExp);

    pExpression operator + (const pExpression& pExp1,
			    const pExpression& pExp2);
    pExpression operator - (const pExpression& pExp1,
			    const pExpression& pExp2);
    pExpression operator * (const pExpression& pExp1,
			    const pExpression& pExp2);
    pExpression operator / (const pExpression& pExp1,
			    const pExpression& pExp2);
    pExpression operator ^ (const pExpression& pExp1,
			    const pExpression& pExp2);

    bool operator == (const pExpression& pExp1,
		      const pExpression& pExp2);

    bool operator != (const pExpression& pExp1,
		      const pExpression& pExp2);

    pExpression log (const pExpression& pExp);
  }

  std::ostream& operator << (std::ostream& o, const Impl_::Expression& e);

  std::ostream& operator << (std::ostream& o, const Scalar& t);

  /** Arithmeric operations */
  Scalar log(const Scalar& s);
};


////////////////////////////////////////////////////////////////////////////////
// Prototype Declarations
////////////////////////////////////////////////////////////////////////////////
enum class Symbol::Impl_::Predicate {
  CONST, VARIABLE, NEGATE, ADD, MULTIPLY, DIVIDE, LOG, POWER
};

class Symbol::Impl_::Expression {
  const std::string name_;
  const Predicate   predicate_;
  const Operands    operands_;
  int val_;
  void assertExpressionConsistency(const std::string info) const;
public:
  // Construct operations with zero operand. (= variable)
  Expression(const std::string name);

  // Construct operations with constant value. (= constant symbol)
  Expression(const int val);

  // Construct operations with one operand.
  Expression(const std::string op, Operand operand);
  Expression(const Predicate& preditace, Operand operand);

  // Construct operations with more that one operand.
  Expression(const std::string op, Operands operands);
  Expression(const Predicate& predicate, Operands operands);

  /** Differentiation */
  pExpression differentiate(const pExpression& dx) const;

  /** Optimization */
  bool isOne() const;
  bool isZero() const;
  pExpression reduce() const;
  pExpression optimize() const;

  /** Helper Functions */
  operator int() const;

  std::string name() const;

  friend std::string expressionName(const Predicate& p, const Operands& o);

  friend std::ostream& operator << (std::ostream& o, const Expression& e);

  friend std::ostream& operator << (std::ostream& o, Expression e);
};

////////////////////////////////////////////////////////////////////////////////
class Symbol::Scalar {

  std::shared_ptr<Impl_::Expression> pExp_;

  Scalar(std::shared_ptr<Impl_::Expression> pExp);

public:
  Scalar(const std::string name);
  Scalar(const int val);

  /** Arithmetic Operations */
  Scalar operator - () const;
  Scalar operator + (const Scalar& other) const;
  Scalar operator - (const Scalar& other) const;
  Scalar operator * (const Scalar& other) const;
  Scalar operator / (const Scalar& other) const;
  Scalar operator ^ (const Scalar& other) const;

  /** Equality Check */
  bool operator == (const Scalar& other) const;
  bool operator != (const Scalar& other) const;

  /** Differentiation */
  Scalar differentiate (Scalar dx) const;

  /** Optimization */
  Scalar simplify() const;

  /** Helper functions */
  friend std::ostream& operator << (std::ostream& o, const Scalar& t);

  friend Symbol::Scalar log(const Symbol::Scalar& s);
};

////////////////////////////////////////////////////////////////////////////////
// Implemantations
////////////////////////////////////////////////////////////////////////////////
std::string Symbol::Impl_::pred2str(const Predicate& p) {
  switch(p) {
  case Impl_::Predicate::CONST:
    return "const";
  case Impl_::Predicate::VARIABLE:
    return "variable";
  case Impl_::Predicate::NEGATE:
    return "negate";
  case Impl_::Predicate::ADD:
    return "add";
  case Impl_::Predicate::MULTIPLY:
    return "multiply";
  case Impl_::Predicate::DIVIDE:
    return "divide";
  case Impl_::Predicate::LOG:
    return "log";
  case Impl_::Predicate::POWER:
    return "power";
  }
  throw std::runtime_error(GEN_DEBUG_INFO + "Not implemented.");
}

Symbol::Impl_::Predicate Symbol::Impl_::str2pred(const std::string& s) {
  if ("const" == s) { return Impl_::Predicate::CONST; }
  if ("variable" == s) { return Impl_::Predicate::VARIABLE; }
  if ("negate" == s) { return  Impl_::Predicate::NEGATE; }
  if ("add" == s) { return Impl_::Predicate::ADD; }
  if ("multiply" == s) { return Impl_::Predicate::MULTIPLY; }
  if ("divide" == s) { return Impl_::Predicate::DIVIDE; }
  if ("log" == s) { return Impl_::Predicate::LOG; }
  if ("power" == s) { return Impl_::Predicate::POWER; }
  throw std::runtime_error(GEN_DEBUG_INFO + "Not implemented:" + s);
}

std::string Symbol::Impl_::expressionName(
                         const Predicate& predicate, const Operands& operands) {
  std::vector<std::string> strings(operands.size());
  std::transform(operands.begin(), operands.end(), strings.begin(),
		 [] (Operand operand) { return operand->name(); });

  if (Predicate::ADD == predicate || Predicate::MULTIPLY == predicate) {
    std::sort(strings.begin(), strings.end());
  }

  std::string ret = pred2str(predicate);
  ret += "(";
  for (size_t i = 0; i < strings.size(); ++i) {
    ret += strings[i];
    if (i != strings.size() - 1) { ret += ", "; }
  }
  ret += ")";
  return ret;
}

Symbol::Impl_::Expression::Expression(const std::string name)
  : name_(name)
  , predicate_(Predicate::VARIABLE)
  , operands_()
  , val_()
{
  assertExpressionConsistency(GEN_DEBUG_INFO);
}

Symbol::Impl_::Expression::Expression(const int v)
  : name_("const")
  , predicate_(Predicate::CONST)
  , operands_()
  , val_(v)
{
  assertExpressionConsistency(GEN_DEBUG_INFO);
}

Symbol::Impl_::Expression::Expression(const Predicate& pred, Operand operand)
  : name_(Impl_::expressionName(pred, {operand}))
  , predicate_(pred)
  , operands_({operand})
  , val_()
{
  assertExpressionConsistency(GEN_DEBUG_INFO);
}

Symbol::Impl_::Expression::Expression(const std::string pred, Operand operand)
  : name_(expressionName(str2pred(pred), {operand}))
  , predicate_(str2pred(pred))
  , operands_({operand})
  , val_()
{
  assertExpressionConsistency(GEN_DEBUG_INFO);
}

Symbol::Impl_::Expression::Expression(const Predicate& pred, Operands operands)
  : name_(expressionName(pred, operands))
  , predicate_(pred)
  , operands_(operands)
  , val_()
{
  assertExpressionConsistency(GEN_DEBUG_INFO);
};

Symbol::Impl_::Expression::Expression(const std::string pred, Operands operands)
  : name_(expressionName(str2pred(pred), operands))
  , predicate_(str2pred(pred))
  , operands_(operands)
  , val_()
{
  assertExpressionConsistency(GEN_DEBUG_INFO);
};

void Symbol::Impl_::Expression::assertExpressionConsistency(
						 const std::string info) const {
  auto nOperands = operands_.size();
  auto nOpStr = std::to_string(nOperands);
  switch(predicate_) {
  case Predicate::CONST:
  case Predicate::VARIABLE:
    if (nOperands)  {
      auto what = info + "Inconsistent Expression. " +
	"predicate:" + pred2str(predicate_) + " #ops:" + nOpStr;
      throw std::invalid_argument(what);
    }
    return;
  case Predicate::LOG:
  case Predicate::NEGATE:
    if (1 != nOperands) {
      auto what = info + "Inconsistent Expression. " +
	"predicate:" + pred2str(predicate_) + " #ops:" + nOpStr;
      throw std::invalid_argument(what);
    }
    return;
  case Predicate::DIVIDE:
  case Predicate::POWER:
    if (2 != nOperands) {
      auto what = info + "Inconsistent Expression. " +
	"predicate:" + pred2str(predicate_) + " #ops:" + nOpStr;
      throw std::invalid_argument(what);
    }
    return;
  case Predicate::ADD:
  case Predicate::MULTIPLY:
    if (2 > nOperands) {
      auto what = info + "Inconsistent Expression. " +
	"predicate:" + pred2str(predicate_) + " #ops:" + nOpStr;
      throw std::invalid_argument(what);
    }
    return;
  }
};

std::string Symbol::Impl_::Expression::name() const {
  if (Predicate::CONST == predicate_) {
    return std::to_string(val_);
  }
  return name_;
};

Symbol::Impl_::pExpression Symbol::Impl_::operator - (
    const pExpression& pExp) {
  return std::make_shared<Expression>("negate", pExp);
}

Symbol::Impl_::pExpression Symbol::Impl_::operator + (
    const pExpression& pExp1, const pExpression& pExp2) {
  return std::make_shared<Expression>("add", Operands {pExp1, pExp2});
}

Symbol::Impl_::pExpression Symbol::Impl_::operator - (
    const pExpression& pExp1, const pExpression& pExp2) {
  return std::make_shared<Expression>("add", Operands {pExp1, -pExp2});
}

Symbol::Impl_::pExpression Symbol::Impl_::operator * (
    const pExpression& pExp1, const pExpression& pExp2) {
  return std::make_shared<Expression>("multiply", Operands {pExp1, pExp2});
}

Symbol::Impl_::pExpression Symbol::Impl_::operator / (
    const pExpression& pExp1, const pExpression& pExp2) {
  return std::make_shared<Expression>("divide", Operands {pExp1, pExp2});
}

Symbol::Impl_::pExpression Symbol::Impl_::operator ^ (
    const pExpression& pExp1, const pExpression& pExp2) {
  return std::make_shared<Expression>("power", Operands {pExp1, pExp2});
}

Symbol::Impl_::pExpression Symbol::Impl_::log (const pExpression& pExp) {
  return std::make_shared<Expression>("log", pExp);
}

bool Symbol::Impl_::operator == (
    const pExpression& pExp1, const pExpression& pExp2) {
  return pExp1->name() == pExp2->name();
}

bool Symbol::Impl_::operator != (
    const pExpression& pExp1, const pExpression& pExp2) {
  return !(pExp1 == pExp2);
}

bool Symbol::Impl_::Expression::isOne() const {
  return (Predicate::CONST == predicate_ && 1 == val_);
}

bool Symbol::Impl_::Expression::isZero() const {
  return (Predicate::CONST == predicate_ && 0 == val_);
}

Symbol::Impl_::pExpression Symbol::Impl_::Expression::reduce() const {
  switch (predicate_) {
  case Predicate::NEGATE:
    if (operands_[0]->isZero()){
      return std::make_shared<Expression>(0);
    }
    if (Predicate::NEGATE == operands_[0]->predicate_)
      return operands_[0]->operands_[0];
    break;
  case Predicate::ADD:
    {
      // TODO: add variable merge
      bool allNegate = true;
      auto constOperand = std::make_shared<Expression>(0);
      Operands tmpOperands;
      for (auto& operand : operands_) {
	// Remove zeros
	if (operand->isZero()) {
	  continue;
	}
	// Check if all operands are negate
	if (Predicate::NEGATE != operand->predicate_) {
	  allNegate = false;
	}

	if (Predicate::CONST == operand->predicate_) {
	  // Merge constant operands
	  constOperand->val_ += operand->val_;
	} else if (Predicate::NEGATE == operand->predicate_ &&
		   Predicate::CONST == operand->operands_[0]->predicate_) {
	  constOperand->val_ -= operand->operands_[0]->val_;
	} else if (Predicate::ADD == operand->predicate_) {
	  // Concatenate add operands
	  tmpOperands.insert(tmpOperands.end(),
			     operand->operands_.begin(), operand->operands_.end());
	} else {
	  // Remove zeros
	  tmpOperands.push_back(operand);
	}
      }
      if (!constOperand->isZero()) {
	tmpOperands.push_back(constOperand);
      }
      // All operands were 0
      if (0 == tmpOperands.size()) {
	return std::make_shared<Expression>(0);
      }
      // Only one opernd remains
      if (1 == tmpOperands.size()) {
	return tmpOperands[0];
      }
      // Invert if all operands are negative
      if (allNegate) {
	std::transform(tmpOperands.begin(), tmpOperands.end(), tmpOperands.begin(),
		       [] (pExpression pExp) { return pExp->operands_[0];});
      }
      return std::make_shared<Expression>(Predicate::ADD, tmpOperands);
    }
  case Predicate::MULTIPLY:
    {
      // TODO: add variable merge
      bool negate = false;
      auto constOperand = std::make_shared<Expression>(1);
      Operands tmpOperands;
      for (auto& operand : operands_) {
	if (operand->isZero()) {
	  return std::make_shared<Expression>(0);
	}
	// Remove Ones
	if (operand->isOne()) {
	  continue;
	}
	if (Predicate::MULTIPLY == operand->predicate_) {
	  // Concatenate MULTIPLY operands
	  tmpOperands.insert(tmpOperands.end(),
			     operand->operands_.begin(), operand->operands_.end());
	} else if (Predicate::NEGATE == operand->predicate_) {
	  // Merge negation
	  tmpOperands.push_back(operand->operands_[0]);
	  negate = !negate;
	} else if (Predicate::CONST == operand->predicate_) {
	  // Merge constant operands
	  constOperand->val_ *= operand->val_;
	} else{
	  tmpOperands.push_back(operand);
	}
      }
      if (!constOperand->isOne()) {
	tmpOperands.push_back(constOperand);
      }
      if (0 == tmpOperands.size()) {
	if (negate) {
	  return -std::make_shared<Expression>(1);
	} else {
	  return std::make_shared<Expression>(1);
	}
      }
      if (1 == tmpOperands.size()) {
	if (negate) {
	  return -tmpOperands[0];
	} else {
	  return tmpOperands[0];
	}
      }
      if (negate) {
	return -std::make_shared<Expression>(Predicate::MULTIPLY, tmpOperands);
      } else {
	return std::make_shared<Expression>(Predicate::MULTIPLY, tmpOperands);
      }
    }
  case Predicate::DIVIDE:
    if (operands_[1]->isOne()) {
      return operands_[0];
    }
    if (operands_[0]->isZero()) {
      return std::make_shared<Expression>(0);
    }
    if (Predicate::NEGATE == operands_[0]->predicate_ &&
	Predicate::NEGATE == operands_[1]->predicate_) {
      return (operands_[0]->operands_[0]) / (operands_[1]->operands_[0]);
    }
    if (Predicate::NEGATE == operands_[0]->predicate_) {
      return -(operands_[0]->operands_[0] / operands_[1]);
    }
    if (Predicate::NEGATE == operands_[1]->predicate_) {
      return -(operands_[0] / operands_[1]->operands_[0]);
    }
    break;
  }
  return std::make_shared<Expression>(*this);
};

Symbol::Impl_::pExpression Symbol::Impl_::Expression::optimize() const {
  auto pOpt = std::make_shared<Expression>(*this);
  std::string before;
  do {
    if (Predicate::CONST == pOpt->predicate_ ||
	Predicate::VARIABLE == pOpt->predicate_) {
      return pOpt;
    }

    before = pOpt->name();
    Operands operands;
    for (auto& operand : pOpt->operands_) {
      operands.push_back(operand->optimize());
    }
    pOpt = std::make_shared<Expression>(pOpt->predicate_, operands)->reduce();
  } while (before != pOpt->name());
  return pOpt;
}

Symbol::Impl_::pExpression Symbol::Impl_::Expression::differentiate(
                                                  const pExpression& dx) const {
  if (name() == dx->name()) {
    return std::make_shared<Expression>(1);
  }

  switch(predicate_) {
  case Predicate::CONST:
  case Predicate::VARIABLE:
    return std::make_shared<Expression>(0);
  case Predicate::NEGATE:
    return -(operands_[0]->differentiate(dx));
  case Predicate::ADD:
    {
      Operands operands;
      for (auto& operand : operands_) {
	operands.push_back(operand->differentiate(dx));
      }
      return std::make_shared<Expression>(predicate_, operands);
    }
  case Predicate::MULTIPLY:
    {
      Operands operands;
      for (size_t i = 0; i < operands_.size(); ++i) {
	Operands tmpOperands;
	for (size_t j = 0; j < operands_.size(); ++j) {
	  if (i == j) {
	    tmpOperands.push_back(operands_[j]->differentiate(dx));
	  } else {
	    tmpOperands.push_back(operands_[j]);
	  }
	}
	auto pTmpExp = std::make_shared<Expression>("multiply", tmpOperands);
	operands.push_back(pTmpExp);
      }
      return std::make_shared<Expression>("add", operands);
    }
  case Predicate::POWER:
    {
      return operands_[1] * (operands_[0] ^ (operands_[1] - std::make_shared<Expression>(1)));
    }
  case Predicate::DIVIDE:
    {
      auto tmp1 = (operands_[1]->differentiate(dx)) * operands_[0];
      auto tmp2 = operands_[1] * (operands_[0]->differentiate(dx));
      return (tmp2 - tmp1) / (operands_[1] * operands_[1]);
    }
  case Predicate::LOG:
    return operands_[0]->differentiate(dx) / operands_[0];
  }
  std::string what = GEN_DEBUG_INFO + "Not Implemented" + pred2str(predicate_);
  throw std::runtime_error(what);
}

Symbol::Impl_::Expression::operator int() const {
  return val_;
};

////////////////////////////////////////////////////////////////////////////////
Symbol::Scalar::Scalar(const std::string name)
  : pExp_(std::make_shared<Impl_::Expression>(name))
{}

Symbol::Scalar::Scalar(const int val)
  : pExp_(std::make_shared<Impl_::Expression>(val))
{}

Symbol::Scalar::Scalar(std::shared_ptr<Impl_::Expression> pExp)
  : pExp_(pExp)
{}

Symbol::Scalar Symbol::Scalar::operator - () const {
  return Scalar(- this->pExp_);
};

Symbol::Scalar Symbol::Scalar::operator + (const Scalar& other) const {
  return Scalar(this->pExp_ + other.pExp_);
};

Symbol::Scalar Symbol::Scalar::operator - (const Scalar& other) const {
  return Scalar(this->pExp_ - other.pExp_);
};

Symbol::Scalar Symbol::Scalar::operator * (const Scalar& other) const {
  return Scalar(this->pExp_ * other.pExp_);
};

Symbol::Scalar Symbol::Scalar::operator / (const Scalar& other) const {
  return Scalar(this->pExp_ / other.pExp_);
};

Symbol::Scalar Symbol::Scalar::operator ^ (const Scalar& other) const {
  return Scalar(this->pExp_ ^ other.pExp_);
};

bool Symbol::Scalar::operator == (const Scalar& other) const {
  return this->pExp_ == other.pExp_;
};

bool Symbol::Scalar::operator != (const Scalar& other) const {
  return !(*this == other);
};

Symbol::Scalar Symbol::Scalar::differentiate (Scalar dx) const {
  auto pExp = this->simplify().pExp_->differentiate(dx.simplify().pExp_);
  return Scalar(pExp->optimize());
}

Symbol::Scalar Symbol::Scalar::simplify() const {
  return Scalar(pExp_->optimize());
}

////////////////////////////////////////////////////////////////////////////////
Symbol::Scalar Symbol::log(const Scalar& s) {
  return Symbol::Scalar(log(s.pExp_));
}

std::ostream& Symbol::operator <<(std::ostream& o, const Impl_::Expression& e) {
  return o << e.name();
}

std::ostream& Symbol::operator << (std::ostream& o, const Scalar& t) {
  return o << t.pExp_->name();
}
