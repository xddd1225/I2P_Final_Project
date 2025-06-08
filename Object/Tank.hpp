#ifndef TANK_HPP
#define TANK_HPP

#include <allegro5/allegro.h>
#include <string>
#include <vector>
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"

class Tank : public Engine::Sprite {
public:
    explicit Tank(float x, float y, std::vector<std::vector<int>>* mapState, int mapWidth, int mapHeight);

    void Update(float deltaTime) override;
    void OnKeyDown(int keyCode);
    void OnKeyUp(int keyCode);

private:
    std::vector<std::vector<int>>* mapState;
    int mapWidth, mapHeight;
    Engine::Point Velocity;
    float Speed;

    bool CheckCollision(Engine::Point nextPos);
};

#endif // TANK_HPP
