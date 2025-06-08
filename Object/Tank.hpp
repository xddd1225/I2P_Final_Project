#ifndef TANK_HPP
#define TANK_HPP

#include <allegro5/allegro.h>
#include <string>
#include <vector>
#include <map>
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include <allegro5/allegro_primitives.h>
// #include "Scene/PlayScene.hpp"
class PlayScene; 
class Tank : public Engine::Sprite {
public:
    explicit Tank(float x, float y, std::vector<std::vector<int>>* mapState, int mapWidth, int mapHeight);

    void Update(float deltaTime) override;
    void OnKeyDown(int keyCode);
    void OnKeyUp(int keyCode);
    void Shoot(float targetX, float targetY);
    void hurt(int damage);
    PlayScene* getPlayScene();

private:
    std::vector<std::vector<int>>* mapState;
    std::map<int, bool> pressedKey;
    int mapWidth, mapHeight;
    Engine::Point Velocity;
    float Speed;
    float shootCooldown = 0;
    int life;
    int maxlife;
    const float SHOOT_COOLDOWN_TIME = 0.01f; // Time between shots in seconds
    
    bool CheckCollision(Engine::Point nextPos);
    void Draw() const override;
};

#endif // TANK_HPP

