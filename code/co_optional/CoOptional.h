#include <experimental/coroutine>
namespace coro = std::experimental;

#include <cinttypes>
#include <optional>

#if !defined(__cpp_coroutines) &&                          \
    !defined(_RESUMABLE_FUNCTIONS_SUPPORTED)
#    pragma message("No Coroutine Support!")
#    define co_await +
#    define co_return (void)
#endif

using None = std::nullopt_t;
template<class V>
using Optional = std::optional<V>;

template<class V>
auto operator co_await(Optional<V> &&o) noexcept {
    using Handle = coro::coroutine_handle<void>;
    struct Awaitable {
        Optional<V> &o;

        auto await_ready() const noexcept -> bool {
            return o.has_value();
        }
        auto await_resume() noexcept -> V {
            return std::move(o).value();
        }

        static void await_suspend(Handle h) noexcept {
            h.destroy();
        }
    };
    return Awaitable{o};
}

template<class V>
struct CoOptional {
    using Opt = Optional<V>;
    struct Promise;
    using Handle = coro::coroutine_handle<Promise>;

    struct Promise {
        Opt opt{};

        auto get_return_object() noexcept { //
            return CoOptional(Handle::from_promise(*this));
        }
        constexpr static auto initial_suspend() noexcept {
            return coro::suspend_never{};
        }
        constexpr static auto final_suspend() noexcept {
            return coro::suspend_always{};
        }

        template<class U>
        void return_value(U &&u) noexcept {
            opt = std::forward<U>(u);
        }
        void unhandled_exception() { throw; }
    };

    CoOptional() = delete;
    ~CoOptional() {
        if (h && h.done()) h.destroy();
    }

    CoOptional(CoOptional &&o) noexcept
        : h(o.h) {
        o.h = {};
    }
    auto operator=(CoOptional &&o) = delete;
    CoOptional(const CoOptional &) = delete;
    auto operator=(const CoOptional &) = delete;

    operator Opt() const &noexcept {
        if (!h) return {};
        if (!h.done()) return {};
        return h.promise().opt;
    }

private:
    CoOptional(Handle h) noexcept
        : h(h) {}

private:
    Handle h{};
};

namespace std::experimental {
template<class V, class... Vs>
struct coroutine_traits<CoOptional<V>, Vs...> {
    using promise_type = typename CoOptional<V>::Promise;
};
} // namespace std::experimental

auto half(uint64_t x) -> Optional<uint64_t> {
    if ((x & 1) == 0)
        return x >> 1;
    else
        return {};
}
auto quad(uint64_t x) -> Optional<uint64_t> {
    return [=]() -> CoOptional<uint64_t> {
        uint64_t y = co_await half(x);
        uint64_t z = co_await half(y);
        co_return z;
    }();
}

auto optTest() { return quad(8); }
