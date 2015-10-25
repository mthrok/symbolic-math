#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "easylogging++.h"

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
/** Main classes */
namespace Symbol {

  namespace Impl_ {

    enum class Predicate;

    class Operation;

    typedef std::shared_ptr<Operation> pOperation;

    typedef std::shared_ptr<Operation> Operand;

    typedef std::vector<Operand> Operands;
  }

  typedef std::shared_ptr<Impl_::Operation> pExpression;

  class Scalar;
};

/** Support functions */
namespace Symbol{

  namespace Impl_{

    /** Support functions for facilitating Operation construction */
    std::string pred2str(const Predicate& p);

    Predicate str2pred(const std::string& s);

    std::string genStringExpression(const Predicate& p, const Operands& o);

    pOperation makeOperation(const std::string pred, const Operands opreands);

    pOperation makeOperation(const Predicate pred, const Operands opreands);

    pOperation makeOperation(const double constant);

    pOperation makeOperation(const std::string name);

    /** Equality */
    bool operator == (const pOperation& pOp1, const pOperation& pOp2);

    bool operator != (const pOperation& pOp1, const pOperation& pOp2);

    /** Arithmetic operations */
    pOperation operator - (const pOperation& pOp);
    pOperation operator + (const pOperation& pOp1, const pOperation& pOp2);
    pOperation operator - (const pOperation& pOp1, const pOperation& pOp2);
    pOperation operator * (const pOperation& pOp1, const pOperation& pOp2);
    pOperation operator / (const pOperation& pOp1, const pOperation& pOp2);
    pOperation operator ^ (const pOperation& pOp1, const pOperation& pOp2);

    pOperation log (const pOperation& pOp);

    /** Optimizations */
    pOperation reduce(const pOperation& pOp);

    pOperation simplify(const pOperation& pOp);

    pOperation differentiate(const pOperation& dy, const pOperation& dx);

    /** Misc */
    std::ostream& operator << (std::ostream& o, const Operation& e);
  }

  /** Arithmeric operations */
  Scalar log(const Scalar& s);

  /** Optimization */
  Scalar differentiate (const Scalar& dy, const Scalar& dx);

  /** Misc */
  std::ostream& operator << (std::ostream& o, const Scalar& t);
};

////////////////////////////////////////////////////////////////////////////////
// Prototype Declarations
////////////////////////////////////////////////////////////////////////////////
enum class Symbol::Impl_::Predicate {
  CONST, VARIABLE, NEGATE, ADD, MULTIPLY, POWER, LOG
};

class Symbol::Impl_::Operation {
  const std::string name_;
  const Predicate   predicate_;
  const Operands    operands_;
  double            constant_;

  bool assertOperationConsistency() const;

public:
  // Construct operations with zero operand. (= variable)
  Operation(const std::string name);

  // Construct operations with constant value. (= constant symbol)
  Operation(const double constant);

  // Construct operations with one operand.
  Operation(const std::string op, Operand operand);
  Operation(const Predicate& preditace, Operand operand);

  // Construct operations with more that one operand.
  Operation(const std::string op, Operands operands);
  Operation(const Predicate& predicate, Operands operands);

  /** Generate the unique name of this opearation. */
  std::string name() const;

  /** Array-like access to operands */
  pOperation operator[] (const size_t index) const;

  /** For optimization */
  bool isOne() const;
  bool isZero() const;

  /** Support functions for arithmetic operations */
  friend pOperation operator - (const pOperation& pOp);

  /** Support functions for optimzations */
  friend pOperation reduce(const pOperation& pOp);

  friend pOperation simplify(const pOperation& pOp);

  friend pOperation differentiate(const pOperation& dy, const pOperation& dx);

  /** Misc support functions */
  friend std::ostream& operator << (std::ostream& o, const Operation& e);

  friend std::ostream& operator << (std::ostream& o, Operation e);
};

////////////////////////////////////////////////////////////////////////////////
class Symbol::Scalar {

  pExpression pExp_;

  Scalar(pExpression pExp);

public:
  /** Constructors */
  Scalar(const std::string name);
  Scalar(const double constant);

  /** Identifier */
  std::string name() const;

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
  friend Scalar differentiate (const Scalar& dy, const Scalar& dx);

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
  case Impl_::Predicate::LOG:
    return "log";
  case Impl_::Predicate::POWER:
    return "power";
  default:
    LOG(FATAL) << "Not implemented.";
  }
}

Symbol::Impl_::Predicate Symbol::Impl_::str2pred(const std::string& s) {
  if (s == "const")    { return Impl_::Predicate::CONST; }
  if (s == "variable") { return Impl_::Predicate::VARIABLE; }
  if (s == "negate")   { return Impl_::Predicate::NEGATE; }
  if (s == "add")      { return Impl_::Predicate::ADD; }
  if (s == "multiply") { return Impl_::Predicate::MULTIPLY; }
  if (s == "log")      { return Impl_::Predicate::LOG; }
  if (s == "power")    { return Impl_::Predicate::POWER; }
  LOG(FATAL) << "Not implemented:" + s;
}

std::string Symbol::Impl_::genStringExpression(
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

Symbol::Impl_::pOperation Symbol::Impl_::makeOperation(const Predicate pred,
						       const Operands operands) {
  return simplify(std::make_shared<Operation>(pred, operands));
}

Symbol::Impl_::pOperation Symbol::Impl_::makeOperation(const std::string pred,
						       const Operands operands) {
  return makeOperation(str2pred(pred), operands);
}

Symbol::Impl_::pOperation Symbol::Impl_::makeOperation(const double constant) {
  return simplify(std::make_shared<Operation>(constant));
}

Symbol::Impl_::pOperation Symbol::Impl_::makeOperation(const std::string name) {
  return simplify(std::make_shared<Operation>(name));
}

Symbol::Impl_::Operation::Operation(const std::string name)
  : name_(name)
  , predicate_(Predicate::VARIABLE)
  , operands_()
  , constant_()
{
  ELPP_ASSERT(assertOperationConsistency(), "Invalid Operation pattern. " + name_);
}

Symbol::Impl_::Operation::Operation(const double v)
  : name_("const")
  , predicate_(Predicate::CONST)
  , operands_()
  , constant_(v)
{
  ELPP_ASSERT(assertOperationConsistency(), "Invalid Operation pattern. " + name_);
}

Symbol::Impl_::Operation::Operation(const Predicate& pred, Operand operand)
  : name_(Impl_::genStringExpression(pred, {operand}))
  , predicate_(pred)
  , operands_({operand})
  , constant_()
{
  ELPP_ASSERT(assertOperationConsistency(), "Invalid Operation pattern. " + name_);
}

Symbol::Impl_::Operation::Operation(const std::string pred, Operand operand)
  : name_(genStringExpression(str2pred(pred), {operand}))
  , predicate_(str2pred(pred))
  , operands_({operand})
  , constant_()
{
  ELPP_ASSERT(assertOperationConsistency(), "Invalid Operation pattern. " + name_);
}

Symbol::Impl_::Operation::Operation(const Predicate& pred, Operands operands)
  : name_(genStringExpression(pred, operands))
  , predicate_(pred)
  , operands_(operands)
  , constant_()
{
  ELPP_ASSERT(assertOperationConsistency(), "Invalid Operation pattern. " + name_);
};

Symbol::Impl_::Operation::Operation(const std::string pred, Operands operands)
  : name_(genStringExpression(str2pred(pred), operands))
  , predicate_(str2pred(pred))
  , operands_(operands)
  , constant_()
{
  ELPP_ASSERT(assertOperationConsistency(), "Invalid Operation pattern. " + name_);
};

bool Symbol::Impl_::Operation::assertOperationConsistency() const {
  auto nOperands = operands_.size();
  switch(predicate_) {
  case Predicate::CONST:
  case Predicate::VARIABLE:
    if (nOperands)
      return false;
    return true;
  case Predicate::LOG:
  case Predicate::NEGATE:
    if (1 != nOperands)
      return false;
    return true;
  case Predicate::POWER:
    if (2 != nOperands)
      return false;
    return true;
  case Predicate::ADD:
  case Predicate::MULTIPLY:
    if (2 > nOperands)
      return false;
    return true;
  }
};

std::string Symbol::Impl_::Operation::name() const {
  if (Predicate::CONST == predicate_) {
    return std::to_string(constant_);
  }
  return name_;
};

Symbol::Impl_::pOperation Symbol::Impl_::Operation::operator[]
    (const size_t index) const {
  return operands_[index];
};

bool Symbol::Impl_::operator == (const pOperation& pOp1,
				 const pOperation& pOp2) {
  return pOp1->name() == pOp2->name();
}

bool Symbol::Impl_::operator != (const pOperation& pOp1,
				 const pOperation& pOp2) {
  return !(pOp1 == pOp2);
}

Symbol::Impl_::pOperation Symbol::Impl_::operator - (const pOperation& pOp) {
  if (Predicate::NEGATE == pOp->predicate_) {
    return (*pOp)[0];
  }
  return makeOperation("negate", Operands {pOp});
}

Symbol::Impl_::pOperation Symbol::Impl_::operator + (const pOperation& pOp1,
						     const pOperation& pOp2) {
  return makeOperation("add", Operands {pOp1, pOp2});
}

Symbol::Impl_::pOperation Symbol::Impl_::operator - (const pOperation& pOp1,
						     const pOperation& pOp2) {
  return makeOperation("add", Operands {pOp1, -pOp2});
}

Symbol::Impl_::pOperation Symbol::Impl_::operator * (const pOperation& pOp1,
						     const pOperation& pOp2) {
  return makeOperation("multiply", Operands {pOp1, pOp2});
}

Symbol::Impl_::pOperation Symbol::Impl_::operator / (const pOperation& pOp1,
						     const pOperation& pOp2) {
  auto exponent = makeOperation(-1);
  auto denominator = makeOperation("power", Operands {pOp2, exponent});
  return makeOperation("multiply", Operands {pOp1, denominator});
}

Symbol::Impl_::pOperation Symbol::Impl_::operator ^ (const pOperation& pOp1,
						     const pOperation& pOp2) {
  return makeOperation("power", Operands {pOp1, pOp2});
}

Symbol::Impl_::pOperation Symbol::Impl_::log (const pOperation& pOp) {
  return makeOperation("log", Operands {pOp});
}

std::ostream& Symbol::Impl_::operator <<(std::ostream& o, const Operation& e) {
  return o << e.name();
}

bool Symbol::Impl_::Operation::isOne() const {
  // TODO change to Almost 1
  return (Predicate::CONST == predicate_ && 1 == constant_);
}

bool Symbol::Impl_::Operation::isZero() const {
  // TODO change to Almost 0
  return (Predicate::CONST == predicate_ && 0 == constant_);
}

Symbol::Impl_::pOperation Symbol::Impl_::reduce(const pOperation& pOp) {
  switch (pOp->predicate_) {
  case Predicate::NEGATE:
    {
      auto innerOperand = (*pOp)[0];
      if (innerOperand -> isZero()){
	return std::make_shared<Operation>(0);
      }
      if (Predicate::NEGATE == innerOperand->predicate_)
	return (*innerOperand)[0];
      break;
    }
  case Predicate::ADD:
    {
      // TODO: add variable merge
      bool allNegate = true;
      auto constOperand = std::make_shared<Operation>(0);
      Operands tmpOperands;
      for (auto& operand : pOp->operands_) {
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
	  constOperand->constant_ += operand->constant_;
	} else if (Predicate::NEGATE == operand->predicate_ &&
		   Predicate::CONST == (*operand)[0] -> predicate_) {
	  constOperand->constant_ -= (*operand)[0] -> constant_;
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
	return std::make_shared<Operation>(0);
      }
      // Only one opernd remains
      if (1 == tmpOperands.size()) {
	return tmpOperands[0];
      }
      // Invert if all operands are negative
      if (allNegate) {
	std::transform(tmpOperands.begin(), tmpOperands.end(), tmpOperands.begin(),
		       [] (pOperation pOp) { return (*pOp)[0];});
      }
      return std::make_shared<Operation>(Predicate::ADD, tmpOperands);
    }
  case Predicate::MULTIPLY:
    {
      // TODO: add variable merge
      struct comp_pOperation {
	bool operator()(const pOperation& a, const pOperation& b) const {
	  return a->name() < b->name();
	}
      };
      std::map<pOperation, float, comp_pOperation> variables;

      bool negate = false;
      auto constOperand = std::make_shared<Operation>(1);
      Operands tmpOperands;
      for (auto& operand : pOp->operands_) {
	// Zero
	if (operand->isZero()) {
	  return std::make_shared<Operation>(0);
	}
	// Ignore Ones
	if (operand->isOne()) {
	  continue;
	}
	if (Predicate::MULTIPLY == operand->predicate_) {
	  // Concatenate MULTIPLY operands
	  tmpOperands.insert(tmpOperands.end(),
			     operand->operands_.begin(), operand->operands_.end());
	  for (auto& operand2 : operand->operands_) {
	    ++variables[operand2];
	  }
	} else if (Predicate::NEGATE == operand->predicate_) {
	  // Merge negation
	  tmpOperands.push_back((*operand)[0]);
	  ++variables[operand->operands_[0]];
	  negate = !negate;
	} else if (Predicate::CONST == operand->predicate_) {
	  // Merge constant operands
	  constOperand->constant_ *= operand->constant_;
	} else{
	  tmpOperands.push_back(operand);
	  ++variables[operand];
	}
      }
      std::cout << "DEBUG: " << pOp->name() << std::endl;
      for (auto& var: variables) {
	std::cout << "    " << var.first->name() << " : " << var.second << std::endl;
      }

      if (!constOperand->isOne()) {
	tmpOperands.push_back(constOperand);
      }
      if (0 == tmpOperands.size()) {
	if (negate) {
	  return -std::make_shared<Operation>(1);
	} else {
	  return std::make_shared<Operation>(1);
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
	return -std::make_shared<Operation>(Predicate::MULTIPLY, tmpOperands);
      } else {
	return std::make_shared<Operation>(Predicate::MULTIPLY, tmpOperands);
      }
    }
  }
  return pOp;
};

Symbol::Impl_::pOperation Symbol::Impl_::simplify(const pOperation& pOp) {
  auto pOpt = std::make_shared<Operation>(*pOp);
  std::string before;
  do {
    if (Predicate::CONST == pOpt->predicate_ ||
	Predicate::VARIABLE == pOpt->predicate_) {
      return pOpt;
    }

    before = pOpt->name();
    Operands operands;
    for (auto& operand : pOpt->operands_) {
      operands.push_back(simplify(operand));
    }
    pOpt = reduce(std::make_shared<Operation>(pOpt->predicate_, operands));
  } while (before != pOpt->name());
  return pOpt;
}

Symbol::Impl_::pOperation Symbol::Impl_::differentiate(
    const pOperation& dy, const pOperation& dx) {
  if (dy->name() == dx->name()) {
    return makeOperation(1);
  }

  switch(dy->predicate_) {
  case Predicate::CONST:
  case Predicate::VARIABLE:
    return makeOperation(0);
  case Predicate::NEGATE:
    return -differentiate(dy->operands_[0], dx);
  case Predicate::ADD:
    {
      Operands operands;
      for (auto& operand : dy->operands_) {
	operands.push_back(differentiate(operand, dx));
      }
      return makeOperation(dy->predicate_, operands);
    }
  case Predicate::MULTIPLY:
    {
      Operands operands;
      for (size_t i = 0; i < dy->operands_.size(); ++i) {
	Operands tmpOperands;
	for (size_t j = 0; j < dy->operands_.size(); ++j) {
	  if (i == j) {
	    tmpOperands.push_back(differentiate(dy->operands_[j], dx));
	  } else {
	    tmpOperands.push_back(dy->operands_[j]);
	  }
	}
	auto pTmpOp = makeOperation("multiply", tmpOperands);
	operands.push_back(pTmpOp);
      }
      return makeOperation("add", operands);
    }
  case Predicate::POWER:
    {
      auto& coeff = dy->operands_[1];
      auto& base = dy->operands_[0];
      auto exp = (dy->operands_[1]) - makeOperation(1);
      return coeff * (base ^ exp);
    }
  case Predicate::LOG:
    return differentiate(dy->operands_[0], dx) / dy->operands_[0];
  }
  LOG(FATAL) << "Not Implemented: " << pred2str(dy->predicate_);
}

////////////////////////////////////////////////////////////////////////////////
Symbol::Scalar::Scalar(const std::string name)
  : pExp_(Impl_::makeOperation(name))
{}

Symbol::Scalar::Scalar(const double constant)
  : pExp_(Impl_::makeOperation(constant))
{}

Symbol::Scalar::Scalar(pExpression pExp)
  : pExp_(pExp)
{}

std::string Symbol::Scalar::name() const { return this->pExp_->name(); }

Symbol::Scalar Symbol::Scalar::operator - () const {
  return Scalar(-this->pExp_);
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

////////////////////////////////////////////////////////////////////////////////
Symbol::Scalar Symbol::log(const Scalar& s) {
  return Symbol::Scalar(log(s.pExp_));
}

Symbol::Scalar Symbol::differentiate (const Scalar& dy, const Scalar& dx) {
  return Scalar(simplify(differentiate(dy.pExp_, dx.pExp_)));
}

std::ostream& Symbol::operator << (std::ostream& o, const Scalar& t) {
  return o << t.pExp_->name();
}
