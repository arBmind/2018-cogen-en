#pragma once
#include <experimental/coroutine>
namespace coro = std::experimental;

template<class Out, class In>
struct CoYield {
    struct Promise;
    using promise_type = Promise;
    using Handle = coro::coroutine_handle<Promise>;

    struct Awaitable {
        In in;

        auto await_ready() const noexcept -> bool {
            return false;
        }
        auto await_resume() noexcept -> In {
            return std::move(in);
        }

        void await_suspend(const Handle &) noexcept {}
    };

    struct Promise {
        Out out;
        Awaitable awaitable;

        auto yield_value(Out o) -> Awaitable & {
            out = std::move(o);
            return awaitable;
        }

        auto get_return_object() noexcept { //
            return CoYield(Handle::from_promise(*this));
        }
        constexpr static auto initial_suspend() noexcept {
            return coro::suspend_never{};
        }
        constexpr static auto final_suspend() noexcept {
            return coro::suspend_always{};
        }

        void return_void() noexcept {}
        void unhandled_exception() { throw; }
    };

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

    auto value() const & noexcept -> Out {
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
    CoYield(Handle h) noexcept
        : h(h) {}

private:
    Handle h{};
};
