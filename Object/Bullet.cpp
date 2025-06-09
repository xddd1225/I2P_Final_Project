#include <allegro5/allegro.h>
#include <cmath>
#include "Bullet.hpp"
#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include <iostream>

PlayScene* Bullet::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Bullet::Bullet(float x, float y, float targetX, float targetY, std::vector<std::vector<int>>* mapState, int mapWidth, int mapHeight)
    : Sprite("play/bullet-1.png", x, y), mapState(mapState), mapWidth(mapWidth), mapHeight(mapHeight) {
    Speed = 300;
    Size.x = 64;
    Size.y = 64;
    
    // Calculate direction vector
    float dx = targetX - x;
    float dy = targetY - y;
    float length = sqrt(dx * dx + dy * dy);
    Velocity = Engine::Point(dx / length, dy / length);
    
    // Set rotation to face the target
    Rotation = atan2(dy, dx) - ALLEGRO_PI / 2;
}

void Bullet::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    Engine::Point nextPos = Position + Velocity * Speed * deltaTime;
    PlayScene* scene = getPlayScene();
    if (CheckCollision(nextPos)) {
        // Remove bullet on collision
        scene->RemoveObject(objectIterator);
    } else {
        Position = nextPos;
    }
}

bool Bullet::CheckCollision(Engine::Point nextPos) {
    float hitboxShrink = 0.1f;
    Engine::Point hitboxSize = Size * hitboxShrink;
    Engine::Point nextMin = nextPos - hitboxSize / 2;
    Engine::Point nextMax = nextPos + hitboxSize / 2;

    // Check map boundaries
    if (nextMin.x < 0 || nextMin.y < 0 || nextMax.x > mapWidth * 64 || nextMax.y > mapHeight * 64) {
        return true;
    }

    // Check wall collisions
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
    // Check tank collision
    PlayScene* scene = getPlayScene();
    Engine::Point target = scene->aiTank->Position;
    if (Engine::Collider::IsCircleOverlap(nextPos, 0, target, 24)) {
        scene->aiTank->hurt(1);
        return true;
    }
    
    return false;
}
