#include <map>
#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <functional>

#include "easylogging++.h"

#define LOG_AND_THROW(MSG)  \
  LOG(ERROR) << MSG; throw std::runtime_error(MSG)


////////////////////////////////////////////////////////////////////////////////
#include <cmath>
inline bool isNearlyEqual(double x, double y, const double epsilon = 1e-5) {
  return std::abs(x - y) <= epsilon; // * std::abs(x);
}

inline bool isInteger(double value) {
  double intpart = 0;
  modf(value, &intpart);
  return isNearlyEqual(value, intpart, 1e-5);
}

////////////////////////////////////////////////////////////////////////////////
// Data structure
namespace Symbol {
  namespace Impl_ {

    enum class Type;

    class IndexMapper;

    class Buffer;

    class Tensor;

    enum class Operator;

    class Exp;
  };

  class Expression;

  typedef std::shared_ptr<Symbol::Impl_::Exp> Operand;
  typedef std::shared_ptr<Symbol::Impl_::Exp> pExp;
  typedef std::vector<Operand> Operands;
  typedef std::vector<uint32_t> Shape;
};

#define MAKE_SHARED_EXP std::make_shared<Symbol::Impl_::Exp>

////////////////////////////////////////////////////////////////////////////////
// Operations
namespace Symbol {
  namespace Impl_ {
    // Support functions for Buffer class
    std::string type2str(const Type &t);
    size_t byteSize(const Type& type, size_t nElems=1);
    std::shared_ptr<void> constructSharedArray(Type type, size_t nElems=1);
    size_t numel(Shape shape);

    // Support functions for Tensor class
    template<typename type> std::ostream& printArray1D
    (std::ostream& o, type* pBuffer, size_t numel, bool encloseBracket);

    template<typename type> std::ostream& printArray2D
    (std::ostream& o, type* pBuffer, size_t row, size_t col, bool encloseBracket);

    template<typename type> std::ostream& printArray3D
    (std::ostream& o, type* pBuffer, size_t row, size_t col, size_t  channel, bool encloseBracket);

    // Support functions for Exp
    pExp constructZero();
    pExp constructOne();
    pExp constructCONST(const double c);
    pExp constructVARIABLE(const std::string name);
    pExp constructVARIABLE(const std::string name, const double c);
    pExp constructNEGATE(const Operand o);
    pExp constructADD(const Operands ops);
    pExp constructMULTIPLY(const Operands ops);
    pExp constructPOWER(const Operands ops);
    pExp constructInverse(const Operand o);
    pExp constructLOG(const Operand o);

    pExp simplify(pExp e);
    pExp flatten(pExp e);
    pExp flattenNEGATE(pExp e);
    pExp flattenMultiOperands(pExp e);
    pExp sort(pExp e);
    // pExp factor(pExp e);
    pExp expand(pExp e);
    pExp expandNEGATE(pExp e);
    pExp expandMULTIPLY(pExp e);
    pExp expandPOWER(pExp e);
    pExp expandLOG(pExp e);
    pExp merge(pExp e);
    pExp mergeADD(pExp e);
    pExp mergeMULTIPLY(pExp e);
    pExp mergePOWER(pExp e);
    pExp mergeLOG(pExp e);

    pExp differentiate(pExp dy, Operand dx);

    Operands decompose2(Operand o);
    Operands decompose3(Operand o);

    struct compareOperands;

    pExp operator - (const Operand o);
    pExp operator + (const Operand o1, const Operand o2);
    pExp operator - (const Operand o1, const Operand o2);
    pExp operator * (const Operand o1, const Operand o2);
    pExp operator ^ (const Operand o1, const Operand o2);
    pExp operator / (const Operand o1, const Operand o2);
    pExp log (const Operand o);

    std::ostream& operator << (std::ostream& o, const Symbol::Impl_::Tensor &t);
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
enum class Symbol::Impl_::Type {
  NONE, INT8, UINT8, INT16, UINT16, INT32, UINT32, INT64, UINT64, FLOAT, DOUBLE
};

class Symbol::Impl_::IndexMapper {
  std::vector<size_t> indices_;

  void initIndices(size_t start, size_t stop, size_t step);
public:
  IndexMapper(size_t numel=0);
  IndexMapper(size_t start, size_t stop, size_t step);
  IndexMapper(std::vector<size_t> indices);

  size_t operator[] (int32_t) const;

  std::vector<size_t>::const_iterator begin() const;
  std::vector<size_t>::const_iterator end() const;
};

class Symbol::Impl_::Buffer {
  Type type_;
  size_t nElems_;
  std::shared_ptr<void> pData_;
public:
  Buffer();
  Buffer(Shape shape, Type type);
  Buffer(size_t nElems, Type type=Type::DOUBLE);

  friend Tensor;
  friend std::ostream& Symbol::Impl_::operator << (std::ostream& o, const Tensor &d);
};

enum class Symbol::Impl_::Operator {
  CONST, VARIABLE, NEGATE, ADD, MULTIPLY, POWER, LOG
};

class Symbol::Impl_::Tensor {
  Type type_;
  Shape shape_;
  Buffer buffer_;
  Tensor(Type type, Shape shape, Buffer buffer);
public:
  Tensor();
  Tensor(Shape shape, Type type=Type::DOUBLE);

  friend std::ostream& operator << (std::ostream& o, const Tensor &d);
};


class Symbol::Impl_::Exp {
#ifdef TEST_IMPL_
public:
#endif
  std::string name_;
  std::shared_ptr<double> pVal_;
  Operator operator_;
  Operands operands_;
public:
  // Constant constructor
  Exp(double value);

  // Variable constructor
  Exp(std::string name);
  Exp(std::string name, double value);

  // Operation constructor
  Exp(Operator oprtr, Operands oprnds);

  void assertOperationConsistency() const;

  bool isConst() const;
  bool isNegative() const;
  bool isPositive() const;
  bool isZero() const;
  bool isOne() const;

  std::string toStr(bool encloseBracket=false) const;
  double value() const;

  void assign(double value);
  double evaluate() const;

  friend pExp simplify(pExp e);
  friend pExp sort(pExp e);
  friend pExp flatten(pExp e);
  friend pExp flattenNEGATE(pExp e);
  friend pExp flattenMultiOperands(pExp e);
  // friend pExp factor(pExp e);
  friend pExp expand(pExp e);
  friend pExp expandNEGATE(pExp e);
  friend pExp expandMULTIPLY(pExp e);
  friend pExp expandPOWER(pExp e);
  friend pExp expandLOG(pExp e);
  friend pExp merge(pExp e);
  friend pExp mergeADD(pExp e);
  friend pExp mergeMULTIPLY(pExp e);
  friend pExp mergePOWER(pExp e);
  friend pExp mergeLOG(pExp e);

  friend Operands decompose2(pExp e);
  friend Operands decompose3(pExp e);

  friend pExp differentiate(pExp dy, Operand dx);

  friend Expression;
};

class Symbol::Expression {
  pExp pExp_;
  Expression(pExp e);
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
Symbol::Impl_::IndexMapper::IndexMapper(size_t numel)
  : indices_()
{
  initIndices(0, numel, 1);
}

Symbol::Impl_::IndexMapper::IndexMapper(size_t start, size_t stop, size_t step)
  : indices_()
{
  initIndices(start, stop, step);
}

Symbol::Impl_::IndexMapper::IndexMapper(std::vector<size_t> indices)
  : indices_(indices)
{}

void Symbol::Impl_::IndexMapper::IndexMapper::initIndices(size_t start, size_t stop, size_t step) {
  indices_.clear();
  for (size_t i=start; i < stop; i+= step) {
    indices_.push_back(i);
  }
}

size_t Symbol::Impl_::IndexMapper::operator[] (int32_t ind) const {
  size_t n_indices = indices_.size();
  while (index < 0) {
    ind += n_indices;
  }
  size_t index = ind;
  if (index >= n_indices) {
    index = index % n_indices;
  }
  return indices_[index];
}

std::vector<size_t>::const_iterator Symbol::Impl_::IndexMapper::begin() const {
  return indices_.begin();
}

std::vector<size_t>::const_iterator Symbol::Impl_::IndexMapper::end() const {
  return indices_.end();
}

std::string Symbol::Impl_::type2str(const Symbol::Impl_::Type &t) {
  switch(t) {
  case Type::NONE:
    return "NONE";
  case Type::INT8:
    return "INT8";
  case Type::UINT8:
    return "UINT8";
  case Type::INT16:
    return "INT16";
  case Type::UINT16:
    return "UINT16";
  case Type::INT32:
    return "INT32";
  case Type::UINT32:
    return "UINT32";
  case Type::INT64:
    return "INT64";
  case Type::UINT64:
    return "UINT64";
  case Type::FLOAT:
    return "FLOAT";
  case Type::DOUBLE:
    return "DOUBLE";
  }
}

size_t Symbol::Impl_::byteSize(const Type& type, size_t nElems) {
  switch(type) {
  case Type::NONE:
    return 0;
  case Type::INT8:
  case Type::UINT8:
    return nElems;
  case Type::INT16:
  case Type::UINT16:
    return nElems * 2;
  case Type::INT32:
  case Type::UINT32:
  case Type::FLOAT:
    return nElems * 4;
  case Type::INT64:
  case Type::UINT64:
  case Type::DOUBLE:
    return nElems * 8;
  }
}

std::shared_ptr<void> Symbol::Impl_::constructSharedArray(Type type, size_t nElems) {
  auto destractor = [](void* p){ delete[] (uint8_t*)p; };
  return std::shared_ptr<void>((void*)new uint8_t[byteSize(type, nElems)], destractor);
}

size_t Symbol::Impl_::numel(Shape shape) {
  size_t ret = 1;
  for (auto& s : shape) {
    ret *= s;
  }
  return ret;
};

Symbol::Impl_::Buffer::Buffer()
  : type_(Type::NONE)
  , nElems_(0)
  , pData_()
{}

Symbol::Impl_::Buffer::Buffer(Shape shape, Type type)
  : type_(type)
  , nElems_(numel(shape))
  , pData_(constructSharedArray(type_, nElems_))
{}

Symbol::Impl_::Buffer::Buffer(size_t nElems, Type type)
  : type_(type)
  , nElems_(nElems)
  , pData_(constructSharedArray(type_, nElems_))
{}

Symbol::Impl_::Tensor::Tensor()
  : type_(Type::NONE)
  , shape_()
  , buffer_()
{}

Symbol::Impl_::Tensor::Tensor(Shape shape, Type type)
  : type_(type)
  , shape_(shape)
  , buffer_()
{}


/*
Symbol::Impl_::Buffer Symbol::Impl_::Buffer::slice(int32_t index) const {
  Shape shape;
  std::vector<IndexMapper> indices;
  shape.push_back(1);
  indices.push_back(IndexMapper(index, index+1, 1));
  for (size_t i = 1; i < indices_.size(); ++i) {
    indices.push_back(indices_[i]);
  }
  for (size_t i = 1; i < shape_.size(); ++i) {
    shape.push_back(shape_[i]);
  }
  return Buffer(type_, shape, indices, pDataRoot_);
}

Symbol::Impl_::Buffer Symbol::Impl_::Buffer::slice(int32_t start, int32_t stop, int32_t step) const {
  // Create range object.
  auto ret = *this;
  ret.indices_[0] = IndexMapper(start, stop, step);
}
*/
template <typename type>
std::ostream& Symbol::Impl_::printArray1D(std::ostream& o, type* pData, size_t numel, bool encloseBracket) {
  if (encloseBracket) {
    o << "[";
  }
  if (numel < 11) {
    for (size_t i = 0; i < numel; ++i) {
      o << *pData;
      if ( i != numel - 1) {
        o << ", ";
      }
      pData++;
    }
  } else {
    for (size_t i = 0; i < numel; ++i) {
      if (i == 0 || i == 1 || i == numel -1) {
        o << *pData;
        if ( i != numel - 1) {
          o << ", ";
        }
        pData++;
      } else if (i == 2) {
        o << "..., ";
      }
    }
  }
  if (encloseBracket) {
    o << "]";
  }
  return o;
};

template <typename type>
std::ostream& Symbol::Impl_::printArray2D(std::ostream& o, type* pData, size_t row, size_t col, bool encloseBracket) {
  if (encloseBracket) {
    o << "[\n";
  }
  for (size_t r = 0; r < row; ++r) {
    o << "  ";
    printArray1D(o, pData, col, false);
    o << "\n";
  }
  if (encloseBracket) {
    o << "]\n";
  }
  return o;
};

template <typename type>
std::ostream& Symbol::Impl_::printArray3D(std::ostream& o, type* pData, size_t row, size_t col, size_t channel, bool encloseBracket) {
  if (encloseBracket) {
    o << "[\n";
  }
  for (size_t c = 0; c < channel; ++c) {
    o << "  (:, :, " << c << ")\n";
    printArray2D(o, pData, row, col, false);
    if (c != channel -1) {
      o << "\n";
    }
  }
  if (encloseBracket) {
    o << "]\n";
  }
  return o;
};

std::ostream& Symbol::Impl_::operator << (std::ostream& o, const Symbol::Impl_::Tensor &t) {
  if (t.type_ == Type::NONE) {
    o << "Abstract Tensor";
    return o;
  }
  auto nAxis = t.shape_.size();
  o << "Tensor {dtype: " << type2str(t.type_) << ", shape: (";
  for (size_t i = 0; i < nAxis; ++i) {
    o << t.shape_[i];
    if (i != t.shape_.size() -1) {
      o << ", ";
    }
  }
  o << ")}\n";
  auto b = t.buffer_;
  if (1 == nAxis) {
    switch(t.type_) {
    case Type::NONE:
      break;
    case Type::INT8:
      printArray1D<int8_t>(o, (int8_t*)b.pData_.get(), b.nElems_, true);
      break;
    case Type::UINT8:
      printArray1D<uint8_t>(o, (uint8_t*)b.pData_.get(), b.nElems_, true);
      break;
    case Type::INT16:
      printArray1D<int16_t>(o, (int16_t*)b.pData_.get(), b.nElems_, true);
      break;
    case Type::UINT16:
      printArray1D<uint16_t>(o, (uint16_t*)b.pData_.get(), b.nElems_, true);
      break;
    case Type::INT32:
      printArray1D<int32_t>(o, (int32_t*)b.pData_.get(), b.nElems_, true);
      break;
    case Type::UINT32:
      printArray1D<uint32_t>(o, (uint32_t*)b.pData_.get(), b.nElems_, true);
      break;
    case Type::INT64:
      printArray1D<int64_t>(o, (int64_t*)b.pData_.get(), b.nElems_, true);
      break;
    case Type::UINT64:
      printArray1D<uint64_t>(o, (uint64_t*)b.pData_.get(), b.nElems_, true);
      break;
    case Type::FLOAT:
      printArray1D<float>(o, (float*)b.pData_.get(), b.nElems_, true);
      break;
    case Type::DOUBLE:
      printArray1D<double>(o, (double*)b.pData_.get(), b.nElems_, true);
      break;
    }
  }
  if (2 == nAxis) {
    switch(t.type_) {
    case Type::NONE:
      break;
    case Type::INT8:
      printArray2D<int8_t>(o, (int8_t*)b.pData_.get(), t.shape_[0], t.shape_[1], true);
      break;
    case Type::UINT8:
      printArray2D<uint8_t>(o, (uint8_t*)b.pData_.get(), t.shape_[0], t.shape_[1], true);
      break;
    case Type::INT16:
      printArray2D<int16_t>(o, (int16_t*)b.pData_.get(), t.shape_[0], t.shape_[1], true);
      break;
    case Type::UINT16:
      printArray2D<uint16_t>(o, (uint16_t*)b.pData_.get(), t.shape_[0], t.shape_[1], true);
      break;
    case Type::INT32:
      printArray2D<int32_t>(o, (int32_t*)b.pData_.get(), t.shape_[0], t.shape_[1], true);
      break;
    case Type::UINT32:
      printArray2D<uint32_t>(o, (uint32_t*)b.pData_.get(), t.shape_[0], t.shape_[1], true);
      break;
    case Type::INT64:
      printArray2D<int64_t>(o, (int64_t*)b.pData_.get(), t.shape_[0], t.shape_[1], true);
      break;
    case Type::UINT64:
      printArray2D<uint64_t>(o, (uint64_t*)b.pData_.get(), t.shape_[0], t.shape_[1], true);
      break;
    case Type::FLOAT:
      printArray2D<float>(o, (float*)b.pData_.get(), t.shape_[0], t.shape_[1], true);
      break;
    case Type::DOUBLE:
      printArray2D<double>(o, (double*)b.pData_.get(), t.shape_[0], t.shape_[1], true);
      break;
    }
  }
  if (3 == nAxis) {
    switch(t.type_) {
    case Type::NONE:
      break;
    case Type::INT8:
      printArray3D<int8_t>(o, (int8_t*)b.pData_.get(), t.shape_[0], t.shape_[1], t.shape_[2], true);
      break;
    case Type::UINT8:
      printArray3D<uint8_t>(o, (uint8_t*)b.pData_.get(), t.shape_[0], t.shape_[1], t.shape_[2], true);
      break;
    case Type::INT16:
      printArray3D<int16_t>(o, (int16_t*)b.pData_.get(), t.shape_[0], t.shape_[1], t.shape_[2], true);
      break;
    case Type::UINT16:
      printArray3D<uint16_t>(o, (uint16_t*)b.pData_.get(), t.shape_[0], t.shape_[1], t.shape_[2], true);
      break;
    case Type::INT32:
      printArray3D<int32_t>(o, (int32_t*)b.pData_.get(), t.shape_[0], t.shape_[1], t.shape_[2], true);
      break;
    case Type::UINT32:
      printArray3D<uint32_t>(o, (uint32_t*)b.pData_.get(), t.shape_[0], t.shape_[1], t.shape_[2], true);
      break;
    case Type::INT64:
      printArray3D<int64_t>(o, (int64_t*)b.pData_.get(), t.shape_[0], t.shape_[1], t.shape_[2], true);
      break;
    case Type::UINT64:
      printArray3D<uint64_t>(o, (uint64_t*)b.pData_.get(), t.shape_[0], t.shape_[1], t.shape_[2], true);
      break;
    case Type::FLOAT:
      printArray3D<float>(o, (float*)b.pData_.get(), t.shape_[0], t.shape_[1], t.shape_[2], true);
      break;
    case Type::DOUBLE:
      printArray3D<double>(o, (double*)b.pData_.get(), t.shape_[0], t.shape_[1], t.shape_[2], true);
      break;
    }
  }
  return o;
}

/*
Symbol::Impl_::Tensor Symbol::Impl_::zeros(Shape shape, Type type) {
  return Tensor(shape, 0, type);
}

Symbol::Impl_::Tensor Symbol::Impl_::ones(Shape shape, Type type) {
  return Tensor(shape, 1, type);
}

Symbol::Impl_::Tensor Symbol::Impl_::eye(Shape shape, Type type) {
  auto ret = Tensor(shape, 0, type);
  // TODO
}
*/
////////////////////////////////////////////////////////////////////////////////
struct Symbol::Impl_::compareOperands {
  inline bool operator() (const pExp& e1, const pExp& e2) const {
    auto const1 = e1->isConst(), const2 = e2->isConst();
    if (const1 && !const2) {
      return true;
    } else if (!const1 && const2) {
        return false;
    } else {
      return e1->toStr(false) < e2->toStr(false);
    }
  }
};

Symbol::Impl_::Exp::Exp(double value)
  : name_()
  , pVal_(std::make_shared<double>(value))
  , operator_(Operator::CONST)
  , operands_()
{
  assertOperationConsistency();
}

Symbol::Impl_::Exp::Exp(std::string name)
  : name_(name)
  , pVal_()
  , operator_(Operator::VARIABLE)
  , operands_()
{
  assertOperationConsistency();
}

Symbol::Impl_::Exp::Exp(std::string name, double value)
  : name_(name)
  , pVal_(std::make_shared<double>(value))
  , operator_(Operator::VARIABLE)
  , operands_()
{
  assertOperationConsistency();
}

Symbol::Impl_::Exp::Exp(Operator oprtr, Operands oprnds)
  : name_()
  , pVal_()
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
   if (!pVal_)
     error_message = "CONST Expression must have valid value.";
   break;
  case Operator::VARIABLE:
   if (nOperands)
     error_message = "VARIABLE Expression must not have operand.";
   break;
  case Operator::NEGATE:
    if (nOperands != 1)
      error_message = "NEGATE Expression must have exactly one operand.";
    if (pVal_)
      error_message = "NEGATE Expression must not have value.";
    break;
  case Operator::POWER:
    if (nOperands != 2)
      error_message = "POWER Expression must have two operands.";
    if (pVal_)
      error_message = "POWER Expression must not have value.";
    break;
  case Operator::ADD:
    if (nOperands < 2)
      error_message = "ADD Expression must have at least two operands.";
    if (pVal_)
      error_message = "ADD Expression must not have value.";
    break;
  case Operator::MULTIPLY:
    if (nOperands < 2)
      error_message = "MULTIPLY Expression must have at least two operands.";
    if (pVal_)
      error_message = "MULTIPLY Expression must not have value.";
    break;
  case Operator::LOG:
    if (nOperands != 1)
      error_message = "LOG Expression must have only exactly operand.";
    if (pVal_)
      error_message = "LOG Expression must not have value.";
    if (operands_[0]->isConst()) {
      if (!operands_[0]->isPositive()) {
        error_message = "Operand of LOG Expression must greater than zero.";
      }
    }
    break;
  }
  if (!error_message.empty()) {
    LOG_AND_THROW(error_message);
  }
};

double Symbol::Impl_::Exp::value() const {
  if (pVal_)
    return *pVal_;
  return NAN;
}

bool Symbol::Impl_::Exp::isConst() const {
  return Operator::CONST == operator_;
}

bool Symbol::Impl_::Exp::isZero() const {
  return isConst() && pVal_ && isNearlyEqual(*pVal_, 0.0);
}

bool Symbol::Impl_::Exp::isOne() const {
  return isConst() && pVal_ && isNearlyEqual(*pVal_, 1.0);
}

bool Symbol::Impl_::Exp::isPositive() const {
  return isConst() && !isZero() && value() > 0.0;
}

bool Symbol::Impl_::Exp::isNegative() const {
  return isConst() && !isZero() && value() < 0.0;
}

Symbol::pExp Symbol::Impl_::constructOne() {
  return constructCONST(1);
}

Symbol::pExp Symbol::Impl_::constructZero() {
  return constructCONST(0);
}

Symbol::pExp Symbol::Impl_::constructCONST(const double c) {
  return MAKE_SHARED_EXP(c);
}

Symbol::pExp Symbol::Impl_::constructVARIABLE(const std::string name) {
  return MAKE_SHARED_EXP(name);
}

Symbol::pExp Symbol::Impl_::constructVARIABLE(const std::string name, const double c) {
  return MAKE_SHARED_EXP(name, c);
}

Symbol::pExp Symbol::Impl_::constructNEGATE(const Operand v) {
  return MAKE_SHARED_EXP(Operator::NEGATE, Operands {v});
}

Symbol::pExp Symbol::Impl_::constructADD(const Operands ops) {
  switch(ops.size()) {
  case 0:
    return constructZero();
  case 1:
    return ops[0];
  default:
    return MAKE_SHARED_EXP(Operator::ADD, ops);
  }
}

Symbol::pExp Symbol::Impl_::constructMULTIPLY(const Operands ops) {
  switch(ops.size()) {
  case 0:
    return constructOne();
  case 1:
    return ops[0];
  default:
    return MAKE_SHARED_EXP(Operator::MULTIPLY, ops);
  }
}

Symbol::pExp Symbol::Impl_::constructPOWER(const Operands ops) {
  return MAKE_SHARED_EXP(Operator::POWER, ops);
}

Symbol::pExp Symbol::Impl_::constructInverse(const Operand o) {
  return constructPOWER({o, constructCONST(-1)});
}

Symbol::pExp Symbol::Impl_::constructLOG(const Operand v) {
  return MAKE_SHARED_EXP(Operator::LOG, Operands {v});
}

Symbol::pExp Symbol::Impl_::flatten(const pExp e) {
  switch(e->operator_) {
  case Operator::NEGATE:
    return flattenNEGATE(e);
  case Operator::ADD:
  case Operator::MULTIPLY:
    return flattenMultiOperands(e);
  default:
    return e;
  }
}

/// -(-X) -> X
/// -C1 -> C2
Symbol::pExp Symbol::Impl_::flattenNEGATE(pExp e) {
  if (Operator::NEGATE != e->operator_) {
    LOG_AND_THROW("flattenNEGATE called on non-NEGATE expression.");
  }
  Operand innerOperand = e->operands_[0];
  switch (innerOperand->operator_) {
  case Operator::NEGATE:
    return innerOperand->operands_[0];
  case Operator::CONST:
    return constructCONST(-innerOperand->value());
  default:
    return e;
  }
}

/// ((X + Y) + Z) -> (X + Y + Z)
/// (X * (Y * Z)) -> (X * Y * Z)
Symbol::pExp Symbol::Impl_::flattenMultiOperands(pExp e) {
  if (Operator::ADD != e->operator_ &&
      Operator::MULTIPLY != e->operator_) {
    LOG_AND_THROW("flattenMultiOperands must be called on ADD or MULTIPLY Expression.");
  }
  Operands newOperands;
  for (auto& operand_ : e->operands_) {
    if (operand_->operator_ != e->operator_) {
      newOperands.push_back(operand_);
    } else {
      newOperands.insert(newOperands.end(),
                         operand_->operands_.begin(),
                         operand_->operands_.end());
    }
  }
  return MAKE_SHARED_EXP(e->operator_, newOperands);
}

Symbol::pExp Symbol::Impl_::sort(pExp e) {
  if (Operator::POWER != e->operator_) {
    std::sort(e->operands_.begin(), e->operands_.end(), compareOperands());
  }
  return e;
}

Symbol::pExp Symbol::Impl_::expand(pExp e) {
  switch(e->operator_) {
  case Operator::NEGATE:
    return expandNEGATE(e);
  case Operator::MULTIPLY:
    return expandMULTIPLY(e);
  case Operator::POWER:
    return expandPOWER(e);
  case Operator::LOG:
    return expandLOG(e);
  default:
    return e;
  }
}

/// -(X + Y + Z) -> -X -Y -Z
Symbol::pExp Symbol::Impl_::expandNEGATE(pExp e) {
  if (Operator::NEGATE != e->operator_) {
    LOG_AND_THROW("expandNEGATE was called on non-NEGATE Expression.");
  }
  auto innerOperand = e->operands_[0];
  auto innerOperator = innerOperand->operator_;
  switch(innerOperator) {
  case Operator::ADD: {
    Operands newOperands;
    for (auto& operand_ : innerOperand->operands_) {
      newOperands.push_back(constructNEGATE(operand_));
    }
    return MAKE_SHARED_EXP(innerOperator, newOperands);
  }
  default:
    return e;
  }
}

/// (X + Y) * (A + B)  -> X*A + X*B + Y*A + Y*B
Symbol::pExp Symbol::Impl_::expandMULTIPLY(pExp e) {
  if (Operator::MULTIPLY != e->operator_) {
    LOG_AND_THROW("expandMULTIPLY was called on non-MULTIPLY Expression.");
  }
  // Classify operands to ADD type and non-ADD type.
  Operands nonAddOperands;
  std::vector<Operands> addOperandsSet;
  for (auto& operand_ : e->operands_) {
    if (operand_->operator_ == Operator::ADD) {
      addOperandsSet.push_back(operand_->operands_);
    } else {
      nonAddOperands.push_back(operand_);
    }
  }
  // If no operand is ADD type, then it's expanded.
  if (0 == addOperandsSet.size()) {
    return e;
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
Symbol::pExp Symbol::Impl_::expandPOWER(pExp e) {
  if (Operator::POWER != e->operator_) {
    LOG_AND_THROW("expandPOWER was called on non-POWER Expression.");
  }
  auto base = e->operands_[0];
  auto expo = e->operands_[1];
  if (Operator::MULTIPLY == base->operator_) {
      Operands operands;
      for (auto& operand_ : base->operands_) {
        operands.push_back(constructPOWER({operand_, expo}));
      }
      return constructMULTIPLY(operands);
  }
  if (Operator::CONST == expo->operator_) {
    double dExpo = expo->value();
    if (dExpo > 0 && isInteger(dExpo)) {
      uint32_t iExpo = std::round(dExpo);
      Operands operands;
      for (uint32_t i = 0; i < iExpo; ++i) {
        auto operand = base;
          operands.push_back(operand);
      }
      return constructMULTIPLY(operands);
    }
  }
  return e;
}

/// log(X * Y) -> log(X) + log(Y)
/// log(X ^ Y) -> Y * log(X)
Symbol::pExp Symbol::Impl_::expandLOG(pExp e) {
  auto innerOperand = e->operands_[0];
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
    return e;
  }
}

Symbol::pExp Symbol::Impl_::merge(pExp e) {
  switch(e->operator_) {
  case Operator::ADD:
    return mergeADD(e);
  case Operator::MULTIPLY:
    return mergeMULTIPLY(e);
  case Operator::POWER:
    return mergePOWER(e);
  case Operator::LOG:
    return mergeLOG(e);
  default:
    return e;
  }
}

/// Merge constant terms and the coefficients of non-constant terms
/// ex) C1 + X + X + Y + C2 -> (2 * X) + Y + (C1 + C2)
Symbol::pExp Symbol::Impl_::mergeADD(pExp e) {
  if (Operator::ADD != e->operator_) {
    LOG_AND_THROW("mergeADD was called ont non ADD Expression.");
  }
  std::map<pExp, double, compareOperands> operandCounts;
  // Classify operands to coefficient and non-coefficient parts
  auto constTerm = constructZero();
  for (auto& operand_ : e->operands_) {
    if (operand_->isConst()) {
      *(constTerm->pVal_) += operand_->value();
    } else {
      auto elems = decompose2(operand_);
      auto coeff = elems[0];
      auto nonCoeff = elems[1];
      operandCounts[nonCoeff] += coeff->value();
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
Symbol::pExp Symbol::Impl_::mergeMULTIPLY(pExp e) {
  if (Operator::MULTIPLY != e->operator_) {
    LOG_AND_THROW("mergeMULTIPLY was called on non-MULTIPLY Expression.");
  }
  auto constOperand = constructOne();
  std::map<pExp, pExp, compareOperands> nonConstOperands;
  // Classify operands to coefficient, base and exponent
  for (auto& operand_ : e->operands_) {
    auto elems = decompose3(operand_);
    auto coeff = elems[0]->value();
    auto base = elems[1];
    auto exponent = elems[2];
    (*constOperand->pVal_) *= coeff;
    if (nonConstOperands[base]) {
      nonConstOperands[base] = nonConstOperands[base] + exponent;
    } else {
      nonConstOperands[base] = exponent;
    }
  }
  if (isNearlyEqual(constOperand->value(), 0,0)) {
    return constructZero();
  }
  // Reconstruct Expression.
  Operands operands;
  // Push back the constant term
  if (!isNearlyEqual(constOperand->value(), 1.0)) {
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
Symbol::pExp Symbol::Impl_::mergePOWER(pExp e) {
  if (Operator::POWER != e->operator_) {
    LOG_AND_THROW("mergePOWER called on non-POWER Expression.");
  }
  auto base = e->operands_[0];
  auto expo = e->operands_[1];
  if (base->isConst() && expo->isConst()) {
    return constructCONST(std::pow(base->value(), expo->value()));
  }
  if (base->isOne() || expo->isZero()) {
    return constructOne();
  }
  if (expo->isOne()) {
    return base;
  }
  return e;
}

/// log(1) -> 0
Symbol::pExp Symbol::Impl_::mergeLOG(pExp e) {
  if (Operator::LOG != e->operator_) {
    LOG_AND_THROW("mergeLOG called on non-LOG Expression.");
  }
  if (e->operands_[0]->isOne()) {
    return constructZero();
  } else {
    return e;
  }
}

/// Decompose an Expression into coefficient and non-coefficient part
/// such that pExp == coeff * non-coeff
/// This function is made for simplifying the mergeADD.
/// This decomposition is not necessarily generally apprecable.
Symbol::Operands Symbol::Impl_::decompose2(Operand o) {
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
    *ret[0]->pVal_ *= -1;
    return ret;
  }
  case Operator::MULTIPLY: {
    double constant = 1;
    Operands operands;
    for (auto& operand : o->operands_) {
      if (operand->isConst()) {
        constant *= operand->value();
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
Symbol::Operands Symbol::Impl_::decompose3(Operand o) {
  switch (o->operator_) {
  case Operator::CONST:
    return {o, constructZero(), constructZero()};
  case Operator::VARIABLE:
  case Operator::LOG:
    return {constructOne(), o, constructOne()};
  case Operator::NEGATE: {
    Operands ret = decompose3(o->operands_[0]);
    *ret[0]->pVal_ *= -1;
    return ret;
  }
  case Operator::ADD:
    return {constructOne(), o, constructOne()};
  case Operator::MULTIPLY: {
    double constant = 1;
    Operands operands;
    for (auto& operand : o->operands_) {
      if (operand->isConst()) {
        constant *= *operand->pVal_;
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

Symbol::pExp Symbol::Impl_::simplify(pExp e) {
  std::transform(e->operands_.begin(), e->operands_.end(),
		 e->operands_.begin(), Impl_::simplify);
  std::string before;
  do {
    before = e->toStr(false);
    e = flatten(e);
    e = expand(e);
  }
  while(before != e->toStr(false));
  do {
    before = e->toStr(false);
    e = merge(e);
  }
  while(before != e->toStr(false));
  e = sort(e);
  return e;
}

Symbol::pExp Symbol::Impl_::differentiate(pExp y, Operand x) {
  if (Operator::CONST == x->operator_) {
    LOG_AND_THROW("Cannot differentiate with CONST Expression.");
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

std::string Symbol::Impl_::Exp::toStr(bool bracket) const {
  std::string ret;
  switch(operator_) {
  case Operator::CONST:
    if (isZero()) {
      return "0";
    } else if (isOne()) {
      return "1";
    } else {
      std::stringstream ss;
      auto val = value();
      if (isInteger(val)) {
        ss << std::fixed << std::setprecision(0) << std::abs(val);
      } else {
        ss << std::fixed << std::setprecision(3) << std::abs(val);
      }
      if (isNegative()) {
        ret = " - " + ss.str();
      } else {
        // We don't need to bracket the positive constant, so return here.
        return ss.str();
      }
    }
    break;
  case Operator::VARIABLE:
    return name_;
  case Operator::NEGATE:
    ret = " - " + operands_[0]->toStr(true);
    break;
  case Operator::ADD:
    for (auto& operand : operands_) {
      std::string append;
      if (operand->isNegative() ||
          operand->operator_ == Operator::NEGATE) {
        append = operand->toStr(false);
        ret += append;
      } else {
        append = operand->toStr(true);
        if (0 != ret.size())
          ret += " + ";
        ret += append;
      }
    }
    break;
  case Operator::MULTIPLY:
    for (auto& operand : operands_) {
      auto append = operand->toStr(true);
      if (0 != ret.size()) {
        ret += " * ";
      }
      ret += append;
    }
    break;
  case Operator::POWER: {
    auto base = operands_[0]->toStr(true);
    auto expo = operands_[1]->toStr(true);
    std::string ret = base + " ^ " + expo;
    if (bracket) {
      ret = "(" + ret + ")";
    }
    return ret;
  }
  case Operator::LOG:
    return "log(" + operands_[0]->toStr(false) + ")";
  }
  if (bracket) {
    ret = "(" + ret + ")";
  }
  return ret;
}

void Symbol::Impl_::Exp::assign(double val) {
  switch(operator_) {
  case Operator::VARIABLE:
    if (!pVal_)
      pVal_ = std::make_shared<double>(val);
  case Operator::CONST:
    *pVal_ = val;
    break;
  default:
    LOG_AND_THROW("Cannot assign value to compound expressions.");
  }
}

double Symbol::Impl_::Exp::evaluate() const {
  switch(operator_) {
  case Operator::CONST:
  case Operator::VARIABLE:
    return value();
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

Symbol::pExp Symbol::Impl_::operator - (const Operand v) {
  return simplify(constructNEGATE(v));
}

Symbol::pExp Symbol::Impl_::operator + (const Operand v1, const Operand v2) {
  return simplify(constructADD({v1, v2}));
}

Symbol::pExp Symbol::Impl_::operator - (const Operand v1, const Operand v2) {
  return simplify(constructADD({v1, -v2}));
}

Symbol::pExp Symbol::Impl_::operator * (const Operand o1, const Operand o2) {
  return simplify(constructMULTIPLY({o1, o2}));
}

Symbol::pExp Symbol::Impl_::operator ^ (const Operand o1, const Operand o2) {
  return simplify(constructPOWER({o1, o2}));
}

Symbol::pExp Symbol::Impl_::operator / (const Operand o1, const Operand o2) {
  return simplify(constructMULTIPLY({o1, constructInverse(o2)}));
}

Symbol::pExp Symbol::Impl_::log (const Operand o) {
  return simplify(constructLOG(o));
}

////////////////////////////////////////////////////////////////////////////////
Symbol::Expression::Expression(double c)
  : pExp_(Impl_::constructCONST(c))
{}

Symbol::Expression::Expression(const std::string name, const double c)
  : pExp_(Impl_::constructVARIABLE(name, c))
{}

Symbol::Expression::Expression(pExp exp)
  : pExp_(exp)
{}

bool Symbol::operator == (const Expression& e1, const Expression& e2) {
  return (e1.pExp_ - e2.pExp_)->isZero();
}

bool Symbol::operator == (const Expression& e, const std::string strExp) {
  return e.pExp_->toStr(false) == strExp;
}

bool Symbol::operator == (const std::string strExp, const Expression& e) {
  return e == strExp;
}

bool Symbol::operator == (const Expression &e, const double c) {
  if (std::isnan(c)) {
    return std::isnan(e.evaluate());
  }
  return isNearlyEqual(e.evaluate(), c);
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
  return o << e.pExp_->toStr(false);
}
