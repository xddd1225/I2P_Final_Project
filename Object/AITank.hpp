#ifndef AITANK_HPP
#define AITANK_HPP

#include <allegro5/allegro.h>
#include <string>
#include <vector>
#include <map>
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Object/Action.hpp"
#include "Object/AITank.hpp"

// Forward declarations
class PlayScene;

class AITank : public Engine::Sprite {
public:
    explicit AITank(float x, float y, std::vector<std::vector<int>>* mapState, int mapWidth, int mapHeight);
    AITank(const AITank& tank);
    void Update(float deltaTime) override;
    void Shoot(float targetX, float targetY);
    void PropertyChange(float deltaTime);
    void hurt(int damage);
    void setVelocity(Engine::Point point);
    void ApplyAction(const Action& act);
    void Strategy();
    PlayScene* getPlayScene();

// private:
    friend class AIstrategy;
    std::vector<std::vector<int>>* mapState;
    std::map<int, bool> pressedKey;
    int mapWidth, mapHeight;
    Engine::Point Velocity;
    float Speed;
    float shootCooldown = 0;
    int life;
    const float SHOOT_COOLDOWN_TIME = 0.5f; // Time between shots in seconds
    
    bool CheckCollision(Engine::Point nextPos);
    void extractState(PlayScene* scene);
    // void ApplyAction(Action& act);
};

#endif // AITANK_HPP

