#include <experimental/coroutine>
namespace coro = std::experimental;

#include <cinttypes>
#include <variant>

#if !defined(__cpp_coroutines) &&                          \
    !defined(_RESUMABLE_FUNCTIONS_SUPPORTED)
#    pragma message("No Coroutine Support!")
#    define co_await +
#    define co_return (void)
#endif

template<class R>
struct Ok {
    R r;
};
template<class R>
auto ok(R &&r) -> Ok<R> {
    return {std::forward<R>(r)};
}
template<class R>
auto ok(R &r) -> Ok<R> {
    return {r};
}
template<class R>
auto ok(const R &r) -> Ok<R> {
    return {r};
}

template<class E>
struct Err {
    E e;
};
template<class E>
auto err(E &&e) -> Err<E> {
    return {std::forward<E>(e)};
}

template<class R, class E>
struct Result {
    using OkT = Ok<R>;
    using ErrT = Err<E>;

    Result() = default;
    Result(OkT r)
        : m(std::move(r)) {}
    Result(ErrT e)
        : m(std::move(e)) {}
    auto operator=(OkT r) & -> Result & {
        m = std::move(r);
        return *this;
    }
    auto operator=(ErrT e) & -> Result & {
        m = std::move(e);
        return *this;
    }

    operator bool() const {
        return std::holds_alternative<OkT>(m);
    }

    auto ok() const & -> const R & {
        return std::get<OkT>(m).r;
    }
    auto ok() && -> R {
        return std::get<OkT>(std::move(m)).r;
    }
    auto err() const & -> const E & {
        return std::get<ErrT>(m).e;
    }
    auto err() && -> E {
        return std::get<ErrT>(std::move(m)).e;
    }

private:
    std::variant<OkT, ErrT> m{};
};

template<class R, class E>
struct CoResult {
    using ResultT = Result<R, E>;
    struct Promise;
    using Handle = coro::coroutine_handle<Promise>;

    struct Promise {
        ResultT *res{};

        auto get_return_object() noexcept { //
            return CoResult(Handle::from_promise(*this));
        }
        constexpr static auto initial_suspend() noexcept {
            return coro::suspend_never{};
        }
        constexpr static auto final_suspend() noexcept {
            return coro::suspend_always{};
        }

        template<class U>
        void return_value(U &&u) noexcept {
            *res = std::forward<U>(u);
        }
        void unhandled_exception() { throw; }
    };

    CoResult() = delete;
    ~CoResult() {
        if (h && h.done()) h.destroy();
    }

    CoResult(CoResult &&o) noexcept
        : h(o.h) {
        o.h = {};
        if (h) h.promise().res = &res;
    }
    auto operator=(CoResult &&o) = delete;
    CoResult(const CoResult &) = delete;
    auto operator=(const CoResult &) = delete;

    operator ResultT() const &noexcept { return res; }

private:
    CoResult(Handle h) noexcept
        : h(h) {
        if (h) h.promise().res = &res;
    }

private:
    ResultT res{};
    Handle h{};
};

template<class R, class E>
auto operator co_await(Result<R, E> &&o) noexcept {
    using CoResultT = CoResult<R, E>;
    using Promise = typename CoResultT::Promise;
    using Handle = coro::coroutine_handle<Promise>;
    struct Awaitable {
        Result<R, E> &o;

        auto await_ready() const noexcept -> bool {
            return o;
        }
        auto await_resume() noexcept -> R {
            return std::move(o).ok();
        }

        void await_suspend(Handle h) {
            h.promise().return_value(o);
            h.destroy();
        }
    };
    return Awaitable{o};
}

namespace std::experimental {
template<class R, class E, class... Vs>
struct coroutine_traits<CoResult<R, E>, Vs...> {
    using promise_type = typename CoResult<R, E>::Promise;
};
} // namespace std::experimental

enum Errors {
    ODD,
    SYSTEM_UNSTABLE,
};
using HalfResult = Result<uint64_t, Errors>;

auto half(uint64_t x) -> HalfResult {
    if ((x & 1) == 0)
        return ok(x >> 1);
    else
        return err(Errors::ODD);
}
auto quad(uint64_t x) -> HalfResult {
    return [=]() -> CoResult<uint64_t, Errors> {
        uint64_t y = co_await half(x);
        uint64_t z = co_await half(y);
        co_return ok(z);
    }();
}

auto optTest() { return quad(8); }
