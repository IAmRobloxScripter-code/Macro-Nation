#include <iostream>
#if defined(_WIN32) || defined(_WIN64)
std::cout << "Pack it up microslop user!" << std::endl;
exit(1);
#endif
#ifndef __linux__
std::cout << "This does not support any other operating system besides linux!"
          << std::endl;
exit(1);
#endif

#include "application.hpp"

int main() {
  APPLICATION app;
  return 0;
}