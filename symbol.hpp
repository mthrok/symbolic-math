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

#define GEN_DEBUG_INFO genErrorString(std::string(__func__), std::to_string(__LINE__)) + " : "

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace Symbol {

  namespace Impl_ {

    enum class Predicate;

    class Expression;

  }

  class Scalar;

  Symbol::Scalar log(const Symbol::Scalar& s);

  std::ostream& operator << (std::ostream& o, const Symbol::Impl_::Expression& e);

  std::ostream& operator << (std::ostream& o, const Symbol::Scalar& t);

  std::string pred2str(const Symbol::Impl_::Predicate& p);

  Symbol::Impl_::Predicate str2pred(const std::string& s);

  typedef std::shared_ptr<Symbol::Impl_::Expression> pExpression;

  typedef std::shared_ptr<Symbol::Impl_::Expression> Operand;

  typedef std::vector<Operand> Operands;
};

////////////////////////////////////////////////////////////////////////////////
// Prototype Declarations
////////////////////////////////////////////////////////////////////////////////
enum class Symbol::Impl_::Predicate {
  ZERO, ONE, VARIABLE, NEGATE, ADD, MULTIPLY, DIVIDE, LOG
};

class Symbol::Impl_::Expression {
  std::string name_;
  Predicate predicate_;
  Operands operands_;

  void assertPredicate(const std::string info) const;
public:
  // Construct operations with zero operand (= variable).
  Expression(const std::string name);
  Expression(const Predicate& preditace);
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
  const pExpression optimize() const;

  /** Helper Functions */
  std::string string() const;

  friend std::ostream& operator << (std::ostream& o, const Expression& e);
};

////////////////////////////////////////////////////////////////////////////////
class Symbol::Scalar {

  std::shared_ptr<Impl_::Expression> pExp_;

  Scalar(std::shared_ptr<Impl_::Expression> pExp);

public:
  Scalar(const std::string name);

  /** Arithmetic Operations */
  Scalar operator - () const;
  Scalar operator + (const Scalar& other) const;
  Scalar operator - (const Scalar& other) const;
  Scalar operator * (const Scalar& other) const;
  Scalar operator / (const Scalar& other) const;

  /** Differentiation */
  Scalar differentiate (Scalar dx) const;

  /** Optimization */
  Scalar optimize() const;

  /** Helper functions */
  friend std::ostream& operator << (std::ostream& o, const Scalar& t);

  friend Symbol::Scalar log(const Symbol::Scalar& s);
};

////////////////////////////////////////////////////////////////////////////////
// Implemantations
////////////////////////////////////////////////////////////////////////////////
Symbol::Impl_::Expression::Expression(const std::string name)
  : name_(name)
  , predicate_(Predicate::VARIABLE)
  , operands_()
{
  assertPredicate(GEN_DEBUG_INFO);
}

Symbol::Impl_::Expression::Expression(const Predicate& predicate)
  : name_()
  , predicate_(predicate)
  , operands_()
{
  assertPredicate(GEN_DEBUG_INFO);
}

Symbol::Impl_::Expression::Expression(const Predicate& pred, Operand operand)
  : predicate_(pred)
  , operands_({operand})
{
  assertPredicate(GEN_DEBUG_INFO);
}

Symbol::Impl_::Expression::Expression(const std::string pred, Operand operand)
  : predicate_(str2pred(pred))
  , operands_({operand})
{
  assertPredicate(GEN_DEBUG_INFO);
}

Symbol::Impl_::Expression::Expression(const Predicate& pred, Operands operands)
  : predicate_(pred)
  , operands_(operands)
{
  assertPredicate(GEN_DEBUG_INFO);
};

Symbol::Impl_::Expression::Expression(const std::string pred, Operands operands)
  : predicate_(str2pred(pred))
  , operands_(operands)
{
  assertPredicate(GEN_DEBUG_INFO);
};

void Symbol::Impl_::Expression::assertPredicate(const std::string info) const {
  auto nOperands = operands_.size();
  auto nOpStr = std::to_string(nOperands);
  switch(predicate_) {
  case Predicate::ONE:
  case Predicate::ZERO:
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

Symbol::pExpression Symbol::Impl_::Expression::differentiate(const pExpression& dx) const {
  if (string() == dx->string()) {
    return std::make_shared<Expression>(Predicate::ONE);
  }
  if (Predicate::ONE == predicate_ ||
      Predicate::ZERO == predicate_ ||
      Predicate::VARIABLE == predicate_) {
    return std::make_shared<Expression>(Predicate::ZERO);
  }
  if (Predicate::ADD == predicate_ ||
      Predicate::NEGATE == predicate_) {
    Operands operands;
    for (auto& operand : operands_) {
      operands.push_back(operand->differentiate(dx));
    }
    return std::make_shared<Expression>(predicate_, operands);
  }
  if (Predicate::MULTIPLY == predicate_) {
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
  if (Predicate::LOG == predicate_) {
    Operands operands{ operands_[0]->differentiate(dx), operands_[0] };
    return std::make_shared<Expression>("divide", operands);
  }
  std::string what = GEN_DEBUG_INFO + "Not Implemented" + pred2str(predicate_);
  throw std::runtime_error(what);
}

bool Symbol::Impl_::Expression::isOne() const {
  return Predicate::ONE == predicate_;
}

bool Symbol::Impl_::Expression::isZero() const {
  return Predicate::ZERO == predicate_;
}

Symbol::pExpression Symbol::Impl_::Expression::reduce() const {
  if (Predicate::MULTIPLY == predicate_) {
    if (operands_[0]->isZero() || operands_[1]->isZero()) {
      return std::make_shared<Expression>(Predicate::ZERO);
    }
    if (operands_[0]->isOne()) { return operands_[1]; }
    if (operands_[1]->isOne()) { return operands_[0]; }
    if (Predicate::NEGATE == operands_[0]->predicate_) {
      Operands operands{ operands_[0]->operands_[0], operands_[1] };
      auto tmp = std::make_shared<Expression>("multiply", operands);
      return std::make_shared<Expression>("negate", tmp);
    }
    if (Predicate::NEGATE == operands_[1]->predicate_) {
      Operands operands{ operands_[1]->operands_[0], operands_[0] };
      auto tmp = std::make_shared<Expression>("multiply", operands);
      return std::make_shared<Expression>("negate", tmp);
    }
  }
  if (Predicate::ADD == predicate_) {
    if (operands_[0]->isZero()) { return operands_[1]; }
    if (operands_[1]->isZero()) { return operands_[0]; }
    if (Predicate::NEGATE == operands_[0]->predicate_ &&
        Predicate::NEGATE == operands_[1]->predicate_) {
      Operands operands{ operands_[0]->operands_[0], operands_[1]->operands_[0]};
      auto tmp = std::make_shared<Expression>("add", operands);
      return std::make_shared<Expression>("negate", tmp);
    }
  }
  return std::make_shared<Expression>(*this);
};

const Symbol::pExpression Symbol::Impl_::Expression::optimize() const {
  auto pExp = std::make_shared<Expression>(*this);
  std::string before;
  do {
    before = pExp->string();
    for (size_t i = 0; i < pExp->operands_.size(); ++i) {
      pExp->operands_[i] = pExp->operands_[i]->optimize();
    }
    pExp = pExp->reduce();
  } while (before != pExp->string());
  return pExp;
}

std::string Symbol::Impl_::Expression::string() const {
  if (Predicate::ONE == predicate_) { return "ONE"; }
  if (Predicate::ZERO == predicate_) { return "ZERO"; }
  if (Predicate::VARIABLE == predicate_) { return name_; }

  std::vector<std::string> strings;
  for (auto& operand : operands_) {
    strings.push_back(operand->string());
  }
  if (Predicate::ADD == predicate_ ||
      Predicate::MULTIPLY == predicate_) {
    std::sort(strings.begin(), strings.end());
  }

  std::string ret = pred2str(predicate_);
  ret += "(";
  for (size_t i = 0; i < strings.size(); ++i) {
    ret += strings[i];
    if (i != strings.size() - 1) {
      ret += ", ";
    }
  }
  ret += ")";
  return ret;
}

////////////////////////////////////////////////////////////////////////////////
Symbol::Scalar::Scalar(const std::string name)
  : pExp_(std::make_shared<Impl_::Expression>(name))
{}

Symbol::Scalar::Scalar(std::shared_ptr<Impl_::Expression> pExp)
  : pExp_(pExp)
{}

Symbol::Scalar Symbol::Scalar::operator - () const {
  auto pExp = std::make_shared<Impl_::Expression>("negate", this->pExp_);
  return Scalar(pExp);
};

Symbol::Scalar Symbol::Scalar::operator + (const Scalar& other) const {
  Operands operands{this->pExp_, other.pExp_};
  auto pExp = std::make_shared<Impl_::Expression>("add", operands);
  return Scalar(pExp);
};

Symbol::Scalar Symbol::Scalar::operator - (const Scalar& other) const {
  Operands operands{this->pExp_, (-other).pExp_};
  auto pExp = std::make_shared<Impl_::Expression>("add", operands);
  return Scalar(pExp);
};

Symbol::Scalar Symbol::Scalar::operator * (const Scalar& other) const {
  Operands operands{this->pExp_, other.pExp_};
  auto pExp = std::make_shared<Impl_::Expression>("multiply", operands);
  return Scalar(pExp);
};

Symbol::Scalar Symbol::Scalar::operator / (const Scalar& other) const {
  Operands operands{this->pExp_, other.pExp_};
  auto pExp = std::make_shared<Impl_::Expression>("divide", operands);
  return Scalar(pExp);
};

Symbol::Scalar Symbol::Scalar::differentiate (Scalar dx) const {
  auto dy = this->optimize();
  dx = dx.optimize();

  auto pExp = dy.pExp_->differentiate(dx.pExp_);
  return Scalar(pExp->optimize());
}

Symbol::Scalar Symbol::Scalar::optimize() const {
  return Scalar(pExp_->optimize());
}

////////////////////////////////////////////////////////////////////////////////
Symbol::Scalar Symbol::log(const Scalar& s) {
  auto pExp = std::make_shared<Symbol::Impl_::Expression>("log", s.pExp_);
  return Symbol::Scalar(pExp);
}

std::ostream& Symbol::operator <<(std::ostream& o, const Symbol::Impl_::Expression& e) {
  return o << e.string();
}

std::ostream& Symbol::operator << (std::ostream& o, const Symbol::Scalar& t) {
  return o << t.pExp_->string();
}

std::string Symbol::pred2str(const Impl_::Predicate& p) {
  switch(p) {
  case Impl_::Predicate::ZERO:
    return "zero";
  case Impl_::Predicate::ONE:
    return "one";
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
  default:
    throw std::runtime_error(std::string(__func__) + "Not implemented.");
  }
}

Symbol::Impl_::Predicate Symbol::str2pred(const std::string& s) {
  if ("zero" == s) { return Impl_::Predicate::ZERO; }
  if ("one" == s) { return  Impl_::Predicate::ONE; }
  if ("variable" == s) { return Impl_::Predicate::VARIABLE; }
  if ("negate" == s) { return  Impl_::Predicate::NEGATE; }
  if ("add" == s) { return Impl_::Predicate::ADD; }
  if ("multiply" == s) { return Impl_::Predicate::MULTIPLY; }
  if ("divide" == s) { return Impl_::Predicate::DIVIDE; }
  if ("log" == s) { return Impl_::Predicate::LOG; }
  throw std::runtime_error(std::string(__func__) + "Not implemented:" + s);
}
