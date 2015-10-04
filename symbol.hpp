#include <map>
#include <vector>
#include <string>
#include <iostream>

// #define DEBUG

namespace Symbol {

  namespace Impl_ {
    // Symbolic Operation Implementation.
    class Operation_;

    // Symbolic Operator
    enum class Operator_;
  }

  // Symbol Interface.
  class Variable;

  // Helper functions
  std::ostream& operator<< (std::ostream &o, const Symbol::Variable &v);
}

typedef std::shared_ptr<Symbol::Impl_::Operation_> pOperation_;

enum class Symbol::Impl_::Operator_ {
  CONST, NEGATE, ADD, SUBTRACT, MULTIPLY
};

////////////////////////////////////////////////////////////////////////////////
class Symbol::Impl_::Operation_ {
  static int counter_;

  std::string name_;

  Operator_ operator_;

  std::vector<pOperation_> operands_;
public:
  /** Constructor */
  // Variable construction
  Operation_(const std::string name);
  // Operation construction
  Operation_(Operator_ op, std::vector<pOperation_> operands);

  /** Destructor */
  ~Operation_();

  /** Differentiate */
  pOperation_ differentiate(const pOperation_& dx);

  /** Getter */
  const std::string id() const;

private:
  void printCounter(const std::string &func);
};
int Symbol::Impl_::Operation_:: counter_ = 0;

////////////////////////////////////////////////////////////////////////////////
Symbol::Impl_::Operation_::Operation_(const std::string name)
  : name_(name)
  , operator_(Operator_::CONST)
  , operands_()
{
  ++counter_;
  printCounter(__func__);
}

Symbol::Impl_::Operation_::Operation_(Operator_ op, std::vector<pOperation_> operands)
  : name_()
  , operator_(op)
  , operands_(operands)
{
  ++counter_;
  printCounter(__func__);
}

Symbol::Impl_::Operation_::~Operation_() {
  --counter_;
  printCounter(__func__);
}

pOperation_ Symbol::Impl_::Operation_::differentiate(const pOperation_ &dx) {
  if (this->id() == dx.get()->id()) {
    return std::make_shared<Operation_>("one");
  }
  bool found;
  std::vector<pOperation_> operands;
  switch(operator_) {
  case Operator_::CONST:
    return std::make_shared<Operation_>("zero");
  case Operator_::MULTIPLY:
    found = false;
    for (auto operand_ : operands_) {
      if (operand_->id() == dx.get()->id()) {
	found = true;
	operands.push_back(std::make_shared<Operation_>("one"));
      } else {
	operands.push_back(operand_);
      }
    }
    if (found) {
      return std::make_shared<Operation_>(operator_, operands);
    } else {
      return std::make_shared<Operation_>("zero");
    }
  default:
    for (auto& operand_ : operands_) {
      operands.push_back(operand_->differentiate(dx));
    }
    return std::make_shared<Operation_>(operator_, operands);
  }
}

const std::string Symbol::Impl_::Operation_::id() const {
  if ("" != name_) {
    return name_;
  }
  std::string ret;
  switch(operator_) {
  case Operator_::CONST:
    ret += "CONST("; break;
  case Operator_::NEGATE:
    ret += "NOT("; break;
  case Operator_::ADD:
    ret += "ADD("; break;
  case Operator_::SUBTRACT:
    ret += "SUBTRACT("; break;
  case Operator_::MULTIPLY:
    ret += "MULTIPLY("; break;
  default:
    break;
  }

  for (size_t i = 0; i < operands_.size(); ++i) {
    ret += operands_[i]->id();
    if (i != operands_.size() -1) {
      ret += ", ";
    }
  }
  ret += ")";
  return ret;
}

void Symbol::Impl_::Operation_::printCounter(const std::string& func) {
#ifdef DEBUG
  std::cout << "    " << func << "(" << id() << "):"
	    << " counter_ = " << counter_ << std::endl;
#endif
}

////////////////////////////////////////////////////////////////////////////////
// Symbol::Variable
////////////////////////////////////////////////////////////////////////////////
class Symbol::Variable {
  std::shared_ptr<Symbol::Impl_::Operation_> pVar_;

public:
  // Constructors
  Variable(const std::string id);

  Variable(const Variable &other);

  Variable(pOperation_ pOperation);

  // Arithmetic operations
  Variable operator - ();

  Variable operator + (const Variable& other);

  Variable operator - (const Variable& other);

  Variable operator * (const Variable& other);

  // Diff
  Variable differentiate(const Variable& dx);

  // print out
  friend std::ostream& operator<< (std::ostream &o, const Symbol::Variable &v);
};
/** Constructors **/
// New variable
Symbol::Variable::Variable(const std::string id)
  : pVar_(std::make_shared<Symbol::Impl_::Operation_>(id))
{}
// Copy construction
Symbol::Variable::Variable(const Variable &other)
  : pVar_(other.pVar_)
{}
Symbol::Variable::Variable(pOperation_ pOperation)
  : pVar_(pOperation)
{}

/** Arithmetic operations **/
// Negation
Symbol::Variable Symbol::Variable::operator - () {
  std::vector<pOperation_> operands;
  operands.push_back(this->pVar_);

  auto pOp = std::make_shared<Impl_::Operation_>(Impl_::Operator_::NEGATE, operands);
  return Variable(pOp);
}

// Addition
Symbol::Variable Symbol::Variable::operator + (const Variable& other) {
  std::vector<pOperation_> operands;
  operands.push_back(this->pVar_);
  operands.push_back(other.pVar_);

  auto pOp = std::make_shared<Impl_::Operation_>(Impl_::Operator_::ADD, operands);
  return Variable(pOp);
}

// Subtraction
Symbol::Variable Symbol::Variable::operator - (const Variable& other) {
  std::vector<pOperation_> operands;
  operands.push_back(this->pVar_);
  operands.push_back(other.pVar_);

  auto pOp = std::make_shared<Impl_::Operation_>(Impl_::Operator_::SUBTRACT, operands);
  return Variable(pOp);
}

// Multiplication
Symbol::Variable Symbol::Variable::operator * (const Variable& other) {
  std::vector<pOperation_> operands;
  operands.push_back(this->pVar_);
  operands.push_back(other.pVar_);

  auto pOp = std::make_shared<Impl_::Operation_>(Impl_::Operator_::MULTIPLY, operands);
  return Variable(pOp);
}

/** Differentiate */
Symbol::Variable Symbol::Variable::differentiate(const Variable& dx) {
  auto pOp = pVar_->differentiate(dx.pVar_);
  return Variable(pOp);
}

/** Print **/
std::ostream& Symbol::operator << (std::ostream &o, const Symbol::Variable &v) {
  return o << "Symbolic Variable: " << v.pVar_->id();
}
