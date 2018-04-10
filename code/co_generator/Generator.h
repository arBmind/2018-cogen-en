#pragma once
#include <experimental/coroutine>
namespace coro = std::experimental;

template<class T>
struct Generator {
    using element_type = T;
    struct Promise;
    using Handle = coro::coroutine_handle<Promise>;

    struct Promise {
        T current_m;

        auto get_return_object() {
            return Generator{Handle::from_promise(*this)};
        }

        auto initial_suspend() {
            return coro::suspend_always{};
        }
        auto final_suspend() {
            return coro::suspend_always{};
        }

        auto yield_value(T value) {
            current_m = std::move(value);
            return coro::suspend_always{};
        }
        auto return_void() {}
        auto unhandled_exception() {}
    };

    auto operator*() const -> const T & {
        return h.promise().current_m;
    }
    auto operator-> () const -> const T * {
        return &h.promise().current_m;
    }
    auto move() -> T {
        return std::move(h.promise().current_m);
    }

    operator bool() const { return h && !h.done(); }
    auto operator++() -> Generator & {
        if (h) h.resume();
        return *this;
    }

    struct End {};
    static auto end() -> End { return {}; }

    auto begin() {
        struct Iterator {
            using iterator_category =
                std::input_iterator_tag;
            using difference_type = ptrdiff_t;
            using value_type = T;
            using reference = T const &;
            using pointer = T const *;

            Generator &gen;

            auto operator*() const -> const T & {
                return *gen;
            }
            auto operator++() -> Iterator & {
                ++gen;
                return *this;
            }
            bool operator==(End) const { return !gen; }
            bool operator!=(End) const { return gen; }
        };
        return Iterator{++(*this)};
    }

#if !defined(__cpp_coroutines) &&                          \
    !defined(_RESUMABLE_FUNCTIONS_SUPPORTED)
#    pragma message("Warning: Lacking Coroutine Support!")
#    define co_yield (void)
#    define co_return
#endif

    ~Generator() {
        if (h) h.destroy();
    }
    Generator() = delete;
    Generator(const Generator &) = delete;
    Generator(Generator &&o) noexcept
        : h(o.h) {
        o.h = {};
    }
    auto operator=(const Generator &) = delete;
    auto operator=(Generator &&) = delete;

private:
    Generator(Handle h)
        : h(h) {}

private:
    Handle h;
};

namespace std::experimental {

template<class T, class... Vs>
struct coroutine_traits<Generator<T>, Vs...> {
    using promise_type = typename Generator<T>::Promise;
};

} // namespace std::experimental
