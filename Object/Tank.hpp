#ifndef TANK_HPP
#define TANK_HPP

#include <allegro5/allegro.h>
#include <string>
#include <vector>
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
// #include "Scene/PlayScene.hpp"
class PlayScene; 
class Tank : public Engine::Sprite {
public:
    explicit Tank(float x, float y, std::vector<std::vector<int>>* mapState, int mapWidth, int mapHeight);

    void Update(float deltaTime) override;
    void OnKeyDown(int keyCode);
    void OnKeyUp(int keyCode);
    void Shoot(float targetX, float targetY);
    PlayScene* getPlayScene();

private:
    std::vector<std::vector<int>>* mapState;
    int mapWidth, mapHeight;
    Engine::Point Velocity;
    float Speed;
    float shootCooldown = 0;
    const float SHOOT_COOLDOWN_TIME = 0.01f; // Time between shots in seconds

    bool CheckCollision(Engine::Point nextPos);
};

#endif // TANK_HPP

