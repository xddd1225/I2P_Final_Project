#include <allegro5/allegro.h>
#include <cmath>
#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "AIStrategy.hpp"
#include "Scene/PlayScene.hpp"
#include "Object/AITank.hpp"
#include "Bullet.hpp"
#include <iostream>

PlayScene *AITank::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}


AITank::AITank(float x, float y, std::vector<std::vector<int>>* mapState, int mapWidth, int mapHeight)
    : Sprite("play/tank.png", x, y), mapState(mapState), mapWidth(mapWidth), mapHeight(mapHeight) {
    Speed = 300;
    Size.x = 64;
    Size.y = 64;
    life = 300;
    Velocity = Engine::Point(0, 0);
}

AITank::AITank(const AITank& other) 
    : Engine::Sprite(other) // 如果 Sprite 有複製建構子，建議初始化列表帶入
{
    mapState = other.mapState;
    mapWidth = other.mapWidth;
    mapHeight = other.mapHeight;
    Speed = other.Speed;
    Size = other.Size;
    life = other.life;
    Velocity = other.Velocity;
    shootCooldown = other.shootCooldown;
}

void AITank::ApplyAction(const Action& act){
    Velocity = act.direction.Normalize();
}

void AITank::Strategy() {
    // Position.x
    State snapshot = State(getPlayScene());
    MonteCarloAI ai(15, 0.5f, 30);
    Action best = ai.DecideBestAction(snapshot);
    ApplyAction(best);
}

void AITank::Update(float deltaTime) {
    PropertyChange(deltaTime);
    PlayScene* scene = getPlayScene();
    int targetX = scene->playerTank->Position.x;
    int targetY = scene->playerTank->Position.y;
    Shoot(targetX, targetY);
}

void AITank::PropertyChange(float deltaTime){
    Velocity = Velocity.Normalize();
    Engine::Point fullMove = Velocity * Speed * deltaTime;
    Engine::Point nextPos = Position + fullMove;

    if (!CheckCollision(nextPos)) {
        Position = nextPos; // 直接通過
    } else {
        Engine::Point xMove = Engine::Point(fullMove.x, 0);
        Engine::Point yMove = Engine::Point(0, fullMove.y);
        if (!CheckCollision(Position + xMove)) {
            Position = Position + xMove;
        } else if (!CheckCollision(Position + yMove)) {
            Position = Position + yMove;
        }
    }

    if (shootCooldown > 0) {
        shootCooldown -= deltaTime;
    }
}

bool AITank::CheckCollision(Engine::Point nextPos) {
    float hitboxShrink = 0.7f;
    Engine::Point hitboxSize = Size * hitboxShrink;
    Engine::Point nextMin = nextPos - hitboxSize / 2;
    Engine::Point nextMax = nextPos + hitboxSize / 2;

    if (nextMin.x < 0 || nextMin.y < 0 || nextMax.x > mapWidth * 64 || nextMax.y > mapHeight * 64) {
        return true;
    }

    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            if ((*mapState)[y][x] == PlayScene::TILE_WALL) {
                Engine::Point wallMin(x * 64, y * 64);
                Engine::Point wallMax = wallMin + Engine::Point(64, 64);
                if (Engine::Collider::IsRectOverlap(nextMin, nextMax, wallMin, wallMax)) {
                    return true;
                }
            }
        }
    }

    return false;
}

void AITank::hurt(int damage) {
    life -= damage;
    if (life <= 0){
        getPlayScene()->RemoveObject(objectIterator);
    }
}

void AITank::Shoot(float targetX, float targetY) {
    if (shootCooldown <= 0) {
        // Create a new bullet at the tank's position
        Bullet* bullet = new Bullet(Position.x, Position.y, targetX, targetY, mapState, mapWidth, mapHeight, 1);
        getPlayScene()->BulletGroup->AddNewObject(bullet);
        shootCooldown = SHOOT_COOLDOWN_TIME;
    }
}
