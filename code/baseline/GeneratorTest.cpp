#include "Generator.h"
#include <utility>

template<size_t N>
auto iota() -> Generator<size_t> {
    struct Iota {
        size_t n{};
        operator bool() const { return n < N; }
        auto next() -> size_t { return n++; }
    };
    return {Iota{}};
}

auto iota(size_t n) -> Generator<size_t> {
    struct Iota {
        size_t N{};
        size_t n{};
        operator bool() const { return n < N; }
        auto next() -> size_t { return n++; }
    };
    return {Iota{n, {}}};
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
