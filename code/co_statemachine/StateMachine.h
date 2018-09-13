#pragma once
#include <experimental/coroutine>
namespace coro = std::experimental;

template<class Event, class State>
struct StateMachine {

    struct Promise;
    using promise_type = Promise;
    using Handle = coro::coroutine_handle<Promise>;

    struct Awaitable {
        Event event;

        auto await_ready() const noexcept -> bool {
            return false;
        }
        auto await_resume() noexcept -> Event {
            return std::move(event);
        }

        void await_suspend(const Handle &) noexcept {}
    };

    struct Promise {
        State state;
        Awaitable awaitable;

        auto yield_value(State s) noexcept -> Awaitable & {
            state = std::move(s);
            return awaitable;
        }
        auto get_return_object() noexcept {
            return StateMachine{
                Handle::from_promise(*this)};
        }

        auto initial_suspend() noexcept {
            return coro::suspend_never{};
        }
        auto final_suspend() noexcept {
            return coro::suspend_always{};
        }

        auto return_void() noexcept {}
        auto unhandled_exception() { throw; }
    };

    StateMachine() = delete;
    ~StateMachine() {
        if (h && h.done()) h.destroy();
    }

    StateMachine(StateMachine &&o) noexcept
        : h(o.h) {
        o.h = {};
    }
    StateMachine &operator=(StateMachine &&o) = delete;
    StateMachine(const StateMachine &) = delete;
    StateMachine &operator=(const StateMachine &) = delete;

    auto state() const -> State {
        if (!h) return {};
        return h.promise().state;
    }

    void handle(Event e) noexcept {
        if (!h) return;
        if (h.done()) return;
        h.promise().awaitable.event = std::move(e);
        h.resume();
    }

private:
    StateMachine(Handle h) noexcept
        : h(h) {}

private:
    Handle h{};
};
