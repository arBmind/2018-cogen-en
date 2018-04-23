#include <array>
#include <iostream>

// tag::slide[]
auto generate() {
  // nest++
  return std::array<long, 2>{23, 42};
  // nest--
}

// nest++
void iterate_it() {
  // nest++
  for (auto v : generate()) {
    // nest++
    std::cout << v << '\n';
    // nest--
  }
  // nest--
}
// nest--
// end::slide[]

int main() {
    iterate_it();
}