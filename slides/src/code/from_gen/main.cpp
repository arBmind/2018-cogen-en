#include <iostream>

// tag::gen[]
template<class T>
struct FromGenerator {
  // nest++
  T start;
  // nest--

  // nest++
  struct End {};
  auto end() { return End{}; }
  // nest--

  // nest++
  struct Iter;
  auto begin() { return Iter{start}; }
  // nest--
};
// end::gen[]

// tag::iter[]
template<class T>
struct FromGenerator<T>::Iter {
  // nest++
  T n{};
  // nest--

  // nest++
  auto operator*() const { return n; }
  // nest--
  // nest++
  auto operator!=(End) const { return true; }
  // nest--
  // nest++
  auto operator++() {
    ++n;
    return *this;
  }
  // nest--
};
// end::iter[]

template<class T>
auto from(T n) {
  return FromGenerator<T>{n};
}

int main() {
  for (auto v : from(23u)) {
    std::cout << v << '\n';
    break;
  }
}
