#pragma once
#include <vector>
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"

class Bullet : public Engine::Sprite {
private:
    std::vector<std::vector<int>>* mapState;
    int mapWidth;
    int mapHeight;
    int Speed;
    bool CheckCollision(Engine::Point nextPos);
    PlayScene *getPlayScene();

public:
    int target;
    bool hasBounced = false;
    int simDestroy; // if collision then true (simulation update)
    Bullet(float x, float y, float targetX, float targetY, std::vector<std::vector<int>>* mapState, int mapWidth, int mapHeight, int target);
    void Update(float deltaTime) override;
    std::vector<int> SimulateUpdate(float deltaTime);
};
