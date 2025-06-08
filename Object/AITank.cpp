#include <allegro5/allegro.h>
#include <cmath>
#include "AITank.hpp"
#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "Bullet.hpp"
#include <iostream>

PlayScene *AITank::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

AITank::AITank(float x, float y, std::vector<std::vector<int>>* mapState, int mapWidth, int mapHeight)
    : Sprite("play/tank.png", x, y), mapState(mapState), mapWidth(mapWidth), mapHeight(mapHeight) {
    Speed = 100;
    Size.x = 64;
    Size.y = 64;
    life = 3;
    Velocity = Engine::Point(0, 0);
}

void AITank::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    Engine::Point nextPos = Position + Velocity * Speed * deltaTime;

    if (!CheckCollision(nextPos)) {
        Position = nextPos;
    }

    // Update shoot cooldown
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
        Bullet* bullet = new Bullet(Position.x, Position.y, targetX, targetY, mapState, mapWidth, mapHeight);
        getPlayScene()->BulletGroup->AddNewObject(bullet);
        shootCooldown = SHOOT_COOLDOWN_TIME;
    }
}
