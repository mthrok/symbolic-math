#include "symbol.hpp"

int main() {

  Symbol::Variable x("x");
  Symbol::Variable y("y");
  Symbol::Variable z("z");

  std::cout << x << std::endl;
  std::cout << y << std::endl;
  std::cout << "=====" << std::endl;
  std::cout << -x << std::endl;
  std::cout << -y << std::endl;
  std::cout << "=====" << std::endl;
  std::cout << x + y << std::endl;
  std::cout << y + x << std::endl;
  std::cout << "=====" << std::endl;
  std::cout << - x + y << std::endl;
  std::cout << - y + x << std::endl;
  std::cout << "=====" << std::endl;
  std::cout << x - y << std::endl;
  std::cout << y - x << std::endl;
  std::cout << - x - y << std::endl;
  std::cout << - y - x << std::endl;
  std::cout << "=====" << std::endl;
  std::cout << x * y << std::endl;
  std::cout << y * x << std::endl;
  std::cout << - x * y << std::endl;
  std::cout << - y * x << std::endl;
  std::cout << x * y + z << std::endl;
  std::cout << y * x + z<< std::endl;
  std::cout << - x * y + z<< std::endl;
  std::cout << - y * x + z<< std::endl;
  std::cout << x + z * y << std::endl;
  std::cout << y + z * x << std::endl;
  std::cout << - x + z * y << std::endl;
  std::cout << - y + z * x << std::endl;
  std::cout << "=====" << std::endl;

  Symbol::Variable a("a");
  Symbol::Variable b("b");
  Symbol::Variable c("c");

  std::cout << (x).differentiate(x) << std::endl;
  std::cout << (x).differentiate(y) << std::endl;
  std::cout << (x + y).differentiate(x) << std::endl;
  std::cout << (x + y).differentiate(y) << std::endl;
  std::cout << (x + y).differentiate(x + y) << std::endl;
  std::cout << (y + x).differentiate(x + y) << std::endl; // Wrong!
  std::cout << (x * y).differentiate(x) << std::endl;
  std::cout << (x * y).differentiate(y) << std::endl;

  std::cout << (a * x + b * y + c * z).differentiate(x) << std::endl;
  std::cout << (a * x + b * y + c * z).differentiate(y) << std::endl;
  std::cout << (a * x + b * y + c * z).differentiate(z) << std::endl;


  return 0;
}
