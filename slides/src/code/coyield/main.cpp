#include <experimental/coroutine>
namespace coro = std::experimental;

#include <iomanip>
#include <iostream>

// tag::coyield[]
template<class Out, class In>
struct CoYield {
  // end::coyield[]
  struct Awaitable;
  struct Promise;
  using promise_type = Promise;
  using H = coro::coroutine_handle<Promise>;
  H h;

  CoYield() = delete;
  ~CoYield() {
    if (h && h.done()) h.destroy();
  }

  CoYield(CoYield &&o) noexcept
      : h(o.h) {
    o.h = {};
  }
  CoYield &operator=(CoYield &&o) = delete;
  CoYield(const CoYield &) = delete;
  CoYield &operator=(const CoYield &) = delete;

  operator bool() const { return h && !h.done(); }
  auto operator*() const -> Out {
    if (!h) return {};
    return h.promise().out;
  }

  void resume(In in) noexcept {
    if (!h) return;
    if (h.done()) return;
    h.promise().awaitable.in = std::move(in);
    h.resume();
  }

private:
  CoYield(Promise &p)
      : h(H::from_promise(p)) {}
};

// tag::promise[]
template<class Out, class In>
struct CoYield<Out, In>::Promise {
  // end::promise[]
  // tag::suspend[]
  // nest++
  auto initial_suspend() {
    return coro::suspend_never{};
  }
  // nest--
  // nest++
  auto final_suspend() {
    return coro::suspend_always{};
  }
  // nest--
  // end::suspend[]

  // tag::yield[]
  auto yield_value(Out o) -> Awaitable & {
    // nest++
    out = o;
    // nest--
    // nest++
    return awaitable;
    // nest--
  }
  // nest++
  Out out;
  Awaitable awaitable;
  // nest--
  // end::yield[]

  // tag::return[]
  // nest++
  auto get_return_object() { return CoYield{*this}; }
  // nest--
  // nest++
  auto return_void() {}
  // nest--
  // nest++
  auto unhandled_exception() {}
  // nest--
  // end::return[]
};

// tag::awaitable[]
template<class Out, class In>
struct CoYield<Out, In>::Awaitable {
  // nest++
  In in;
  // nest--

  // nest++
  auto await_ready() const noexcept { return false; }
  // nest--
  // nest++
  auto await_resume() noexcept { return in; }
  // nest--
  // nest++
  void await_suspend(const H &) noexcept {}
  // nest--
};
// end::awaitable[]

inline auto runner(int v) noexcept
    -> CoYield<int, int> {
  while (true) {
    v = 2 * co_yield v;
  }
}

#include <iostream>

auto co_yield_test() {
  auto h = runner(23);
  std::cout << *h << '\n';
  h.resume(42);
  std::cout << *h << '\n';
  h.resume(65);
  std::cout << *h << '\n';
}

int main() { //
  co_yield_test();
}
