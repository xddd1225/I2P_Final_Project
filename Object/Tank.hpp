#ifndef TANK_HPP
#define TANK_HPP

#include <allegro5/allegro.h>
#include <string>
#include <vector>
#include <map>
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include <allegro5/allegro_primitives.h>


class PlayScene;
class AIStrategy;
class Tank : public Engine::Sprite {
public:
    explicit Tank(float x, float y, std::vector<std::vector<int>>* mapState, int mapWidth, int mapHeight);
    Tank(const Tank& other);
    void Update(float deltaTime) override;
    void OnKeyDown(int keyCode);
    void OnKeyUp(int keyCode);
    void Shoot(float targetX, float targetY);
    void hurt(int damage);
    PlayScene* getPlayScene();

private:
    friend class AIstrategy;
    std::vector<std::vector<int>>* mapState;
    std::map<int, bool> pressedKey;
    int mapWidth, mapHeight;
    Engine::Point Velocity;
    float Speed;
    float shootCooldown = 0;
    int life, maxlife;
    const float SHOOT_COOLDOWN_TIME = 0.5f; // Time between shots in seconds
    Engine::Point TargetVelocity = Engine::Point(0, 0);
    float AccelerationSpeed = 5.0f;

    bool isDashing = false;
    float dashDuration = 0.2f;       // 衝刺持續時間（秒）
    float dashCooldown = 1.0f;       // 衝刺冷卻時間（秒）
    float dashTimer = 0.0f;          // 當前衝刺剩餘時間
    float dashCooldownTimer = 0.0f;  // 冷卻時間計時器
    
    bool CheckCollision(Engine::Point nextPos);
    void Draw() const override;
};

#endif // TANK_HPP

