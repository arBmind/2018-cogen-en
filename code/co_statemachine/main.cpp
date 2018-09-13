#include "Elevator.h"

#include <iostream>

auto operator<<(std::ostream &o, Elevator::Move m)
    -> std::ostream & {
    using Move = Elevator::Move;
    return o << (m == Move::Up ? "Up" : "Down");
}

int main() {
    auto elevator = Elevator::create();
    auto &cout = std::cout;
    using namespace Elevator;

    for (auto event : std::initializer_list<Event>{
             Called{2}, FloorSensor{1}, FloorSensor{2},
             Called{1}, FloorSensor{1}, AlarmPressed{}}) {

        elevator.handle(event);
        match(
            pattern(elevator.state()),
            [&](Idle) { cout << "Idle\n"; },
            [&](Broken) { cout << "Broken\n"; },
            [&](Move m) { cout << "Move: " << m << '\n'; });
    }
}
