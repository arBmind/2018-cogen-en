#include "Elevator.h"

#include <iostream>

using namespace Elevator;

auto operator<<(std::ostream &o, Move m) -> std::ostream & {
  return o << (m == Move::Up ? "Up" : "Down");
}

auto operator<<(std::ostream &o, Event e)
    -> std::ostream & {
  auto handlers = overloaded{
      [&](Called c) -> std::ostream & {
        return o << "Called to: " << c.toFloor;
      },
      [&](FloorSensor fs) -> std::ostream & {
        return o << "FloorSensor at: " << fs.floor;
      },
      [&](AlarmPressed) -> std::ostream & {
        return o << "AlarmPressed";
      } //
  };
  return std::visit(handlers, e);
}

auto operator<<(std::ostream &o, State s)
    -> std::ostream & {

  auto handlers = overloaded{
      [&](Idle) -> std::ostream & { return o << "Idle"; },
      [&](Broken) -> std::ostream & {
        return o << "Broken";
      },
      [&](Move m) -> std::ostream & {
        return o << "Move: " << m;
      } //
  };
  return std::visit(handlers, s);
}

int main() {
  auto elevator = Elevator::create();
  auto &o = std::cout;

  o << "@ " << elevator.state() << '\n';
  for (auto event : std::initializer_list<Event>{
           Called{2}, FloorSensor{1}, FloorSensor{2},
           Called{1}, FloorSensor{1}, AlarmPressed{}}) {

    o << "+ " << event << '\n';
    elevator.handle(event);
    o << "@ " << elevator.state() << '\n';
  }
}
