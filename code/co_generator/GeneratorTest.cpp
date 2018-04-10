#include "Generator.h"
#include <utility>

template<size_t N>
auto iota() -> Generator<size_t> {
    for (auto i = 0u; i < N; ++i) co_yield i;
}

auto iota(size_t n) -> Generator<size_t> {
    for (auto i = 0u; i < n; ++i) co_yield i;
}

template<size_t N>
auto fixedSum() {
    auto sum = 0u;
    for (auto v : iota<N>()) sum += v;
    return sum;
}

auto dynSum(size_t n) {
    auto sum = 0u;
    for (auto v : iota(n)) sum += v;
    return sum;
}

#include <iostream>

int main() { //
    std::cout << fixedSum<101>() << '\n';
    std::cout << dynSum(101) << '\n';
}
