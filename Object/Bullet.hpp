#pragma once
#include <vector>
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"

class Bullet : public Engine::Sprite {
private:
    std::vector<std::vector<int>>* mapState;
    int mapWidth;
    int mapHeight;
    int Speed=300;
    bool CheckCollision(Engine::Point nextPos);
    PlayScene *getPlayScene();

public:
    Bullet(float x, float y, float targetX, float targetY, std::vector<std::vector<int>>* mapState, int mapWidth, int mapHeight);
    void Update(float deltaTime) override;
};
