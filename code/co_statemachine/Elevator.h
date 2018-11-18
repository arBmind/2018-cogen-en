#pragma once
#include "StateMachine.h"
#include <tuple>
#include <utility>
#include <variant>

template<class... F>
struct overloaded : F... {
  using F::operator()...;
};
template<class... F>
overloaded(F...)->overloaded<F...>;

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
enum class Move { Up, Down };
struct Broken {};

using State = std::variant<Idle, Move, Broken>;

inline auto create() -> StateMachine<Event, State> {
  int currentFloor = 0;
  int targetFloor{};
  State state = Idle{};
  while (true) {
    Event event = co_yield state;
    auto matchers = overloaded{
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
          if (currentFloor == targetFloor) return Idle{};
          return move;
        },
        [&](auto state, FloorSensor f) -> State {
          if (currentFloor != f.floor) return Broken{};
          return state;
        },
        [](auto state, auto) -> State { return state; }};
    state = std::visit(matchers, state, event);
  }
}

} // namespace Elevator
