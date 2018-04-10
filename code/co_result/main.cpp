#include <iomanip>
#include <iostream>
#include <utility>

#include "CoResult.h"

auto operator<<(std::ostream &o, Errors e)
    -> std::ostream & {
    switch (e) {
    case Errors::ODD: return o << "ODD";
    case Errors::SYSTEM_UNSTABLE: return o << "UNSTABLE";
    }
    return o << "<default>";
}

auto operator<<(std::ostream &o, const HalfResult &hr)
    -> std::ostream & {
    if (hr)
        return o << "Ok: " << hr.ok();
    else
        return o << "Err: " << hr.err();
}

void coutHalfTest(uint64_t v) {
    auto h = half(v);
    std::cout << "half(" << v << ") = " << h << "\n";
}

void coutQuadTest(uint64_t v) {
    auto h = quad(v);
    std::cout << "quad(" << v << ") = " << h << "\n";
}

auto main() -> int {
    coutHalfTest(4);
    coutHalfTest(5);
    coutHalfTest(6);

    coutQuadTest(0);
    coutQuadTest(4);
    coutQuadTest(5);
    coutQuadTest(6);
    coutQuadTest(7);
    coutQuadTest(8);
}
