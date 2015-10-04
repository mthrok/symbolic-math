#include <string>
#include <vector>
#include <memory>
#include <iostream>

#define DEBUG

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace Symbol {

  namespace Impl_ {

    class Expression;

  }

  class Scalar;
};

std::ostream& operator << (std::ostream& o, const Symbol::Impl_::Expression& e);

std::ostream& operator << (std::ostream& o, const Symbol::Scalar& t);

typedef std::shared_ptr<Symbol::Impl_::Expression> pExpression;
typedef std::vector<pExpression> Operands;

////////////////////////////////////////////////////////////////////////////////
// Prototype Declarations
////////////////////////////////////////////////////////////////////////////////
class Symbol::Impl_::Expression {
  std::string name_;
  std::string operator_;
  Operands operands_;
public:
  // Initialize as a variable
  Expression(const std::string name="");
  // Initialize as a compound expression
  Expression(const std::string op, Operands operands);
  Expression(const std::string op, pExpression operand);

  /** Differentiation */
  pExpression differentiate(const pExpression& dx) const;

  /** Optimization */
  bool isOne() const;
  bool isZero() const;
  pExpression reduce();

  /** Helper Functions */
  std::string string() const;

  friend std::ostream& ::operator << (std::ostream& o, const Expression& e);
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
  Scalar differentiate (const Scalar& dx) const;

  /** Optimization */
  Scalar& optimize();

  /** Helper functions */
  friend std::ostream& ::operator << (std::ostream& o, const Scalar& t);
};

////////////////////////////////////////////////////////////////////////////////
// Implemantations
////////////////////////////////////////////////////////////////////////////////
Symbol::Impl_::Expression::Expression(const std::string name)
  : name_(name)
  , operator_("const")
  , operands_()
{}

Symbol::Impl_::Expression::Expression(const std::string op, pExpression operand)
  : operator_(op)
  , operands_()
{
  operands_.push_back(operand);
}

Symbol::Impl_::Expression::Expression(const std::string op, Operands operands)
  : operator_(op)
  , operands_(operands)
{};

pExpression Symbol::Impl_::Expression::differentiate(const pExpression& dx) const {
  if (string() == dx->string()) {
    return std::make_shared<Expression>("__ONE__");
  }
  if ("const" == operator_) {
    return std::make_shared<Expression>("__ZERO__");
  }
  Operands operands;
  if ("negate" == operator_ || "add" == operator_) {
    for (auto& operand : operands_) {
      operands.push_back(operand->differentiate(dx));
    }
    return std::make_shared<Expression>(operator_, operands);
  }
  if ("multiply" == operator_) {
    Operands operands0 { operands_[0]->differentiate(dx), operands_[1] };
    Operands operands1 { operands_[0], operands_[1]->differentiate(dx) };
    operands.push_back(std::make_shared<Expression>("multiply", operands0));
    operands.push_back(std::make_shared<Expression>("multiply", operands1));
    return std::make_shared<Expression>("add", operands);
  }
  throw std::runtime_error(std::string(__func__) + ": Not implemented: " + operator_);
}

bool Symbol::Impl_::Expression::isOne() const { return "__ONE__" == name_; }

bool Symbol::Impl_::Expression::isZero() const { return "__ZERO__" == name_; }

pExpression Symbol::Impl_::Expression::reduce() {
  bool changed;
  do {
    changed = false;
    for (size_t i = 0; i < operands_.size(); ++i) {
      pExpression ret = operands_[i]->reduce();
      if (ret) {
	operands_[i] = ret;
	changed = true;
      }
    }
  } while (changed);
  if ("multiply" == operator_) {
    if (operands_[0]->isZero() || operands_[1]->isZero()) {
      return std::make_shared<Expression>("__ZERO__");
    }
    if (operands_[0]->isOne()) { return operands_[1]; }
    if (operands_[1]->isOne()) { return operands_[0]; }
    if ("negate" == operands_[0]->operator_) {
      Operands operands{ operands_[0]->operands_[0], operands_[1] };
      auto tmp = std::make_shared<Expression>("multiply", operands);
      return std::make_shared<Expression>("negate", tmp);
    }
    if ("negate" == operands_[1]->operator_) {
      Operands operands{ operands_[1]->operands_[0], operands_[0] };
      auto tmp = std::make_shared<Expression>("multiply", operands);
      return std::make_shared<Expression>("negate", tmp);
    }
  }
  if ("add" == operator_) {
    if (operands_[0]->isZero()) { return operands_[1]; }
    if (operands_[1]->isZero()) { return operands_[0]; }
    if ("negate" == operands_[0]->operator_ &&
	"negate" == operands_[1]->operator_) {
      Operands operands{ operands_[0]->operands_[0], operands_[1]->operands_[0]};
      auto tmp = std::make_shared<Expression>("add", operands);
      return std::make_shared<Expression>("negate", tmp);
    }
  }
  return NULL;
};

std::string Symbol::Impl_::Expression::string() const {
  if ("const" == operator_) { return name_; }

  std::vector<std::string> strings;
  for (auto& operand : operands_) {
    strings.push_back(operand->string());
  }

  std::sort(strings.begin(), strings.end());


  std::string ret = operator_;
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

std::ostream& operator <<(std::ostream& o, const Symbol::Impl_::Expression& e) {
  return o << e.string();
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

Symbol::Scalar Symbol::Scalar::differentiate (const Scalar& dx_) const {
  auto dy = Scalar(this->pExp_).optimize();
  auto dx = Scalar(dx_.pExp_).optimize();

  auto pExp = dy.pExp_->differentiate(dx.pExp_);
  pExp->reduce();
  return Scalar(pExp);
}

Symbol::Scalar& Symbol::Scalar::optimize() {
  pExp_->reduce(); return *this;
}

std::ostream& operator << (std::ostream& o, const Symbol::Scalar& t) {
  return o << t.pExp_->string();
}
