#pragma once
#include "Engine/Sprite.hpp"
#include <vector>
#include <string>

class Explosion : public Engine::Sprite {
public:
    Explosion(float x, float y);
    void Update(float deltaTime) override;
private:
    std::vector<std::string> frames;
    float frameTime = 0.08f; // seconds per frame
    float timer = 0;
    int currentFrame = 0;
};