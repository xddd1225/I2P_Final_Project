#ifndef ACTION_HPP
#define ACTION_HPP

#include "Engine/Point.hpp"
#include <vector>

class Action {
public:
    Engine::Point direction;
    bool shoot;

    Action(Engine::Point dir = Engine::Point(0, 0), bool shoot = false)
        : direction(dir), shoot(shoot) {}
};

// Define direction vectors
const std::vector<Engine::Point> DirectionVectors = {
    Engine::Point(0, -1),   // Up
    Engine::Point(0, 1),    // Down
    Engine::Point(-1, 0),   // Left
    Engine::Point(1, 0),    // Right
    Engine::Point(-1, -1),  // UpLeft
    Engine::Point(1, -1),   // UpRight
    Engine::Point(-1, 1),   // DownLeft
    Engine::Point(1, 1)     // DownRight
};

// Define all possible actions
const std::vector<Action> AllActions = []() {
    std::vector<Action> actions;
    for (const Engine::Point& dir : DirectionVectors) {
        Engine::Point normalized = dir.Normalize();
        actions.emplace_back(normalized, false);   // Move with shooting
    }
    return actions;
}();

#endif // ACTION_HPP