#include "CoYield.h"
#include <utility>

inline auto runner(int v) noexcept -> CoYield<int, int> {
    while (true) {
        v = 2 * co_yield v;
    }
}

#include <string>

inline auto bot() -> CoYield<std::string, std::string> {
    auto response = "Hello";
    while (true) {
        auto request = co_yield response;
        auto is = [&](auto q, auto r) {
            auto b = q == request;
            if (b) response = r;
            return b;
        };
        auto reply = [&](auto r) {
            response = r;
            return true;
        };
        is("Hello", "How are you?") ||
            is("fine", "Great! Want to be friends?") ||
            is("do not ask", "Ok, See you later!") ||
            reply("I do not get it!");
    }
}

#include <iostream>

auto co_yield_test() {
    auto h = runner(23);
    std::cout << h.value() << '\n';
    h.resume(42);
    std::cout << h.value() << '\n';
    h.resume(65);
    std::cout << h.value() << '\n';
}

auto test_bot() {
    auto b = bot();
    std::cout << b.value() << "\n> Hello\n";
    b.resume("Hello");
    std::cout << b.value() << "\n> fine\n";
    b.resume("fine");
    std::cout << b.value() << '\n';
}

int main() { //
    co_yield_test();
    test_bot();
}
