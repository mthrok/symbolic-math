#include "symbol.hpp"

void checkEq(Symbol::Scalar sym1, Symbol::Scalar sym2) {
  std::cout << "=====" << "\n"
	    << "LHR:" << sym1 << "\n"
	    << "RHR:" << sym2 << "\n";
  sym1 = sym1.simplify();
  sym2 = sym2.simplify();
  std::cout << "  =>" << "\n"
	    << "LHR: " << sym1 << "\n"
	    << "RHR: " << sym2 << "\n";
  if (sym1 == sym2) {
    std::cout << "  Equal.\n";
  } else {
    std::cout << "  NOT Equal.\n\n";
  }
}


void testAddition() {
  Symbol::Scalar zero(0);
  Symbol::Scalar one(1);
  Symbol::Scalar two(2);

  checkEq(two, one + one);

  checkEq(two, one + one - two + one + one);

  Symbol::Scalar x1("x1");
  Symbol::Scalar x2("x2");
  Symbol::Scalar y1("y1");
  Symbol::Scalar y2("y2");

  y1 = x1 + x2;

  y2 = x1 + x2 + x1;

  checkEq(x1 + x2, x2 + x1);

  checkEq(x1 + x2, x2 + x1);

  checkEq(- x1 - x2, - x2 - x1);

  // Wrong because x1 - x1 -> 0 is not implemented yet.
  checkEq(- x1 - x2, - x2 - x1 + x1 - x1);

  checkEq(y1, x2 + x1);

  checkEq(y1, x1 + x2);

  checkEq(y2, x1 + x2 + x1);

  checkEq(y2, x1 + x1 + x2);

  checkEq(y2, x2 + x1 + x1);
}

void testMultiplication() {
  Symbol::Scalar zero(0);
  Symbol::Scalar one(1);
  Symbol::Scalar two(2);

  checkEq(two, one * two);
  checkEq(two, two * one);
  checkEq(two, one * two * one);
  checkEq(two * two, two * two * one);
  checkEq(zero, one * zero);
  checkEq(zero, zero * two);
  checkEq(zero, one * -zero);
  checkEq(zero, zero * -two);

  Symbol::Scalar x1("x1");
  Symbol::Scalar x2("x2");
  Symbol::Scalar y1("y1");
  Symbol::Scalar y2("y2");

  y1 = x1 * x2;

  y2 = x1 * x2 * x1;

  checkEq(x1 * x2, x2 * x1);

  checkEq(x1 * x2, x2 * x1);

  checkEq(y1, x2 * x1);

  checkEq(y1, x1 * x2);

  checkEq(y2, x1 * x2 * x1);

  checkEq(y2, x1 * x1 * x2);

  checkEq(y2, x2 * x1 * x1);
}



int main() {

  testAddition();
  testMultiplication();

  Symbol::Scalar x1("x1");
  Symbol::Scalar x2("x2");
  Symbol::Scalar y1("y1");
  Symbol::Scalar y2("y2");

  y1 = - x1 * x2 - x2 * x1;
  y2 = - x1 * x2 - x2 * x1;

  std::cout << x2 << std::endl;
  std::cout << "=====" << std::endl;
  std::cout << (-x1 -x2).simplify() << std::endl;
  std::cout << y1 << std::endl;
  std::cout << log(y1) << std::endl;

  std::cout << "=====" << std::endl;
  std::cout << y2 << std::endl;
  std::cout << y2.simplify() << std::endl;

  std::cout << "=====" << std::endl;
  std::cout << - -x2 << std::endl;
  std::cout << (- -x2).simplify() << std::endl;

  std::cout << "=====" << std::endl;
  std::cout << (x1.differentiate(x1)) / x2 << std::endl;
  std::cout << ((x1.differentiate(x1)) / x2).differentiate(x2) << std::endl;

  std::cout << "=====" << std::endl;
  std::cout << x1.differentiate(x1) << std::endl;
  std::cout << x2.differentiate(x1) << std::endl;
  std::cout << y1.differentiate(x1) << std::endl;
  std::cout << y2.differentiate(x1) << std::endl;
  std::cout << y1.differentiate(y2) << std::endl;
  std::cout << y2.differentiate(y2) << std::endl;
  std::cout << log(x1).differentiate(x1) << std::endl;

  std::cout << "=====" << std::endl;
  std::cout << log(y1) << std::endl;
  std::cout << log(y1).differentiate(x1) << std::endl;
  std::cout << log(y1).differentiate(y1) << std::endl;
  std::cout << (x1 + x2).differentiate(x1 + x2) << std::endl;
  std::cout << (x1 + x2).differentiate(x2 + x1) << std::endl;
}
