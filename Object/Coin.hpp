#ifndef COIN_HPP
#define COIN_HPP

#include "Engine/Sprite.hpp"

class PlayScene;

class Coin : public Engine::Sprite {
public:
    explicit Coin(float x, float y, float w, float h);
    void Update(float deltaTime) override;
    PlayScene* getPlayScene();
};

#endif // COIN_HPP 