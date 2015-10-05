#include "symbol.hpp"

int main() {

  Symbol::Scalar x1("x1");
  Symbol::Scalar x2("x2");
  Symbol::Scalar y1("y1");
  Symbol::Scalar y2("y2");

  y1 = - x1 * x2 - x2 * x1;
  y2 = - x1 * x2 - x2 * x1;

  std::cout << x1 << std::endl;
  std::cout << x2 << std::endl;
  std::cout << y1 << std::endl;
  std::cout << log(y1) << std::endl;

  std::cout << y2 << std::endl;
  std::cout << y2.optimize() << std::endl;

  std::cout << x1.differentiate(x1) << std::endl;
  std::cout << x2.differentiate(x1) << std::endl;
  std::cout << y1.differentiate(x1) << std::endl;
  std::cout << y2.differentiate(x1) << std::endl;
  std::cout << y1.differentiate(y2) << std::endl;
  std::cout << y2.differentiate(y2) << std::endl;
  std::cout << log(x1).differentiate(x1) << std::endl;

  std::cout << log(y1) << std::endl;
  std::cout << log(y1).differentiate(x1) << std::endl;
  std::cout << log(y1).differentiate(y1) << std::endl;
  std::cout << (x1 + x2).differentiate(x1 + x2) << std::endl;
  std::cout << (x1 + x2).differentiate(x2 + x1) << std::endl;
}
