#ifndef LANDMINE_HPP
#define LANDMINE_HPP

#include "Engine/Sprite.hpp"

class PlayScene;
class LandMine : public Engine::Sprite {
public:
    float timer = 3.0f;   // 倒數秒數，預設 3 秒
    bool exploded = false;

    LandMine(float x, float y, float delayTime = 3.0f);

    void Update(float deltaTime) override;
    void Explode();  // 觸發爆炸
    PlayScene *getPlayScene();
};

#endif // LANDMINE_HPP