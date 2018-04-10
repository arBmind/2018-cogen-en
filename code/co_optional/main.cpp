#include <iomanip>
#include <iostream>
#include <utility>

#include "CoOptional.h"

void coutHalfTest(int v) {
    auto h = half(v);
    std::cout << "half(" << v << ") = " << h.has_value()
              << " " << (h.has_value() ? h.value() : 0)
              << "\n";
}

void coutQuadTest(int v) {
    auto h = quad(v);
    std::cout << "quad(" << v << ") = " << h.has_value()
              << " " << (h.has_value() ? h.value() : 0)
              << "\n";
}

auto main() -> int {
    std::cout << std::boolalpha;
    coutHalfTest(4);
    coutHalfTest(5);
    coutHalfTest(6);

    coutQuadTest(4);
    coutQuadTest(5);
    //    coutQuadTest(6);
    coutQuadTest(7);
    coutQuadTest(8);
}
