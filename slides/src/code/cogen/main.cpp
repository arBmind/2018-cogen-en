// tag::include[]
#include <experimental/coroutine>

// nest++
namespace coro = std::experimental;
// nest--
// end::include[]
#include <iomanip>
#include <iostream>

#if !defined(__cpp_coroutines) &&                   \
    !defined(_RESUMABLE_FUNCTIONS_SUPPORTED)
#  pragma message("Lacking Coroutine Support!")
#  define co_yield (void)
#  define co_return
#endif

// tag::generator[]
template<class T>
struct Generator {
  // end::generator[]
  /* clang forces constructors on multiple lines
   * therefore it is disabled here:
   */
  // nest++
  struct promise_type;
  // nest--
  Generator() = delete;
  // clang-format off
  // tag::lifecycle[]
  // nest++
  using H = coro::coroutine_handle<promise_type>;
  // nest--
  // nest++
  H h;
  // nest--

  // nest++
  Generator(promise_type &p)
      : h(H::from_promise(p)) {}
  // nest--
  // nest++
  ~Generator() { if (h) h.destroy(); }
  // nest--
  // nest++
  Generator(Generator &&o) : h(o.h) { o.h = {}; }
  // nest--
  // nest++
  Generator(const Generator &) = delete;
  // nest--
  // end::lifecycle[]
  // clang-format on
  Generator &operator=(const Generator &) = delete;
  Generator &operator=(Generator &&) = delete;

  // tag::read[]
  // nest++
  operator bool() const { return !h.done(); }
  // nest--
  // nest++
  auto operator*() const -> const T & {
    // nest++
    return h.promise().value;
    // nest--
  }
  // nest--
  // end::read[]
  // tag::resume[]
  // nest++
  auto operator++() -> Generator & {
    // nest++
    h.resume();
    return *this;
    // nest--
  }
  // nest--
  // end::resume[]

  // tag::iterator[]
  // nest++
  struct End {};
  auto end() -> End { return {}; }
  // nest--

  // nest++
  struct Begin {
    Generator &g;
    // nest++
    bool operator!=(End) const { return g; }
    // nest--
    // nest++
    auto operator*() const -> const T & {
      return *g;
    }
    // nest--
    // nest++
    void operator++() { ++g; }
    // nest--
  };
  auto begin() -> Begin { return {++*this}; }
  // nest--
  // end::iterator[]
};

// tag::promise[]
template<class T>
struct Generator<T>::promise_type {
  // end::promise[]
  // tag::suspend[]
  // nest++
  auto initial_suspend() {
    return coro::suspend_always{};
  }
  // nest--
  // nest++
  auto final_suspend() {
    return coro::suspend_always{};
  }
  // nest--
  // end::suspend[]

  // tag::yield[]
  auto yield_value(T v) {
    // nest++
    value = v;
    // nest--
    // nest++
    return coro::suspend_always{};
    // nest--
  }
  // nest++
  T value{};
  // nest--
  // end::yield[]

  // tag::return[]
  // nest++
  auto get_return_object() {
    return Generator{*this};
  }
  // nest--
  // nest++
  auto return_void() {}
  // nest--
  // nest++
  auto unhandled_exception() {}
  // nest--
  // end::return[]
};

/*
// tag::trait[]
// nest++
namespace std::experimental {

// nest++
template<class T, class... Vs>
struct coroutine_traits<Generator<T>, Vs...> {
  // nest++
  using promise_type =
      typename Generator<T>::Promise;
  // nest--
};
// nest--

} // namespace std::experimental
// nest--
// end::trait[]
*/

// tag::fun[]
auto fun() -> Generator<long> {
  // nest++
  co_yield 23;
  // nest--
}
// end::fun[]

void test_fun() {
  auto f = fun();
  std::cout << std::boolalpha << "-- test_fun()\n";
  std::cout << static_cast<bool>(f) << '\n';
  ++f;
  std::cout << *f << '\n';
  std::cout << static_cast<bool>(f) << '\n';
}

// tag::23_42[]
auto generate() -> Generator<long> {
  // nest++
  co_yield 23;
  // nest--
  // nest++
  co_yield 42;
  // nest--
}
// end::23_42[]

// tag::23_42_generate[]
void test_generate() {
  // nest++
  auto f = generate();
  // nest--
  // end::23_42_generate[]
  using std::cout;
  std::cout << std::boolalpha
            << "-- test_generate()\n";
  // clang-format off
  // tag::23_42_usage[]
  // nest++
  cout << static_cast<bool>(f) << '\n'; // true
  // nest--
  // nest++
  ++f;
  // nest--
  // nest++
  cout << static_cast<bool>(f) << '\n'; // true
  // nest--
  // nest++
  cout << *f << '\n';                   // 23
  // nest--
  // nest++
  ++f;
  // nest--
  // nest++
  cout << static_cast<bool>(f) << '\n'; // true
  // nest--
  // nest++
  cout << *f << '\n';                   // 42
  // nest--
  // nest++
  ++f;
  // nest--
  // nest++
  cout << static_cast<bool>(f) << '\n'; // false
  // nest--
  // end::23_42_usage[]
}

// tag::iota[]
auto iota(long n) -> Generator<long> {
  // nest++
  for (auto i = 0u; i < n; ++i) {
    // nest++
    co_yield i;
    // nest--
  }
  // nest--
}
// end::iota[]

// tag::test_iota_head[]
void test_iota() {
  // end::test_iota_head[]
  std::cout << "-- test_iota()\n";
  // tag::test_iota_body[]
  // nest++
  for (auto i : iota(5)) {
    // nest++
    std::cout << i << '\n';
    // nest--
  }
  // nest--
}
// end::test_iota_body[]

// tag::from[]
auto from(long n) -> Generator<long> {
  // nest++
  while (true) {
    // nest++
    co_yield n++;
    // nest--
  }
  // nest--
}
// end::from[]

void test_from() {
  auto f = from(10);
  std::cout << "-- test_from()\n";
  while (++f) {
    std::cout << *f << '\n';
    ++f;
    std::cout << *f << '\n';
    break;
  }
}

int main() {
  test_fun();
  test_generate();
  test_iota();
  test_from();
}
