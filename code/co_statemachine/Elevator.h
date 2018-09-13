#pragma once
#include "StateMachine.h"
#include <tuple>
#include <utility>
#include <variant>

template<class... P>
auto pattern(const P &... p) {
    return std::make_tuple(p...);
}

template<class... P, class... C>
auto match(std::tuple<P...> &&pattern, C &&... c) {
    struct Cases : C... {
        using C::operator()...;
    };
    return std::apply(
        std::visit<Cases, P...>,
        std::tuple_cat(
            std::make_tuple(Cases{std::forward<C>(c)...}),
            pattern));
}

namespace Elevator {

struct Called {
    int toFloor;
};
struct FloorSensor {
    int floor;
};
struct AlarmPressed {};

using Event =
    std::variant<Called, FloorSensor, AlarmPressed>;

struct Idle {};
struct Broken {};
enum class Move { Up, Down };
struct Stop {};

using State = std::variant<Idle, Broken, Move>;

inline auto create() -> StateMachine<Event, State> {
    int currentFloor = 0;
    int targetFloor{};
    State state = Idle{};
    while (true) {
        Event event = co_yield state;

        state = match( //
            pattern(state, event),
            [](auto, AlarmPressed) -> State {
                return Broken{};
            },
            [&](Idle, Called c) -> State {
                targetFloor = c.toFloor;
                return c.toFloor < currentFloor ? Move::Down
                                                : Move::Up;
            },
            [&](Move move, FloorSensor f) -> State {
                currentFloor = f.floor;
                if (currentFloor == targetFloor)
                    return Idle{};
                return move;
            },
            [&](auto state, FloorSensor f) -> State {
                if (currentFloor != f.floor)
                    return Broken{};
                return state;
            },
            [](auto state, auto) -> State {
                return state;
            });
    }
}

} // namespace Elevator
