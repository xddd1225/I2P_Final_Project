#include <allegro5/allegro.h>
#include <cmath>
#include "Bullet.hpp"
#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include <iostream>
using namespace std;

PlayScene* Bullet::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Bullet::Bullet(float x, float y, float targetX, float targetY, std::vector<std::vector<int>>* mapState, int mapWidth, int mapHeight, int target)
    : Sprite("play/bullet-1.png", x, y), mapState(mapState), mapWidth(mapWidth), mapHeight(mapHeight), target(target) {
    Speed = 600;
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
    
    bool collided = false;
    Engine::Point oldPos = Position;

    float hitboxShrink = 0.1f;
    Engine::Point hitboxSize = Size * hitboxShrink;
    Engine::Point nextMin = nextPos - hitboxSize / 2;
    Engine::Point nextMax = nextPos + hitboxSize / 2;

    // 檢查牆壁反彈
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            if ((*mapState)[y][x] == PlayScene::TILE_WALL) {
                Engine::Point wallMin(x * 64, y * 64);
                Engine::Point wallMax = wallMin + Engine::Point(64, 64);
                if (Engine::Collider::IsRectOverlap(nextMin, nextMax, wallMin, wallMax)) {
                    collided = true;

                    if (!hasBounced) {
    // 檢查水平方向反彈
    Engine::Point testX = oldPos + Engine::Point(Velocity.x * Speed * deltaTime, 0);
    Engine::Point testY = oldPos + Engine::Point(0, Velocity.y * Speed * deltaTime);
    if (Engine::Collider::IsRectOverlap(testX - hitboxSize / 2, testX + hitboxSize / 2, wallMin, wallMax)) {
        Velocity.x = -Velocity.x;
    }
    if (Engine::Collider::IsRectOverlap(testY - hitboxSize / 2, testY + hitboxSize / 2, wallMin, wallMax)) {
        Velocity.y = -Velocity.y;
    }
    Rotation = atan2(Velocity.y, Velocity.x) - ALLEGRO_PI / 2;

    hasBounced = true;
    return;  // 本幀不移動，讓反彈生效
} else {
                        scene->RemoveObject(objectIterator); // 第二次撞牆才刪除
                        return;
                    }
                }
            }
        }
    }

    // 檢查擊中目標
    Engine::Point aitarget = scene->aiTank->Position;
    Engine::Point playertarget = scene->playerTank->Position;
    if (target==0 && Engine::Collider::IsCircleOverlap(nextPos, 0, aitarget, 24)) {
        scene->aiTank->hurt(1);
        scene->RemoveObject(objectIterator);
    } else if (target==1 && Engine::Collider::IsCircleOverlap(nextPos, 0, playertarget, 24)) {
        scene->playerTank->hurt(1);
        scene->RemoveObject(objectIterator);
    } else {
        Position = nextPos;
    }
}

vector<int> Bullet::SimulateUpdate(float deltaTime) {
    Engine::Point nextPos = Position + Velocity * Speed * deltaTime;
    vector<int> out = {false,false,false};
    float hitboxShrink = 0.1f;
    Engine::Point hitboxSize = Size * hitboxShrink;
    Engine::Point nextMin = nextPos - hitboxSize / 2;
    Engine::Point nextMax = nextPos + hitboxSize / 2;
    // Check map boundaries
    if (nextMin.x < 0 || nextMin.y < 0 || nextMax.x > mapWidth * 64 || nextMax.y > mapHeight * 64) {
        return out;
    }
    // Check wall collisions
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            if ((*mapState)[y][x] == PlayScene::TILE_WALL) {
                Engine::Point wallMin(x * 64, y * 64);
                Engine::Point wallMax = wallMin + Engine::Point(64, 64);
                if (Engine::Collider::IsRectOverlap(nextMin, nextMax, wallMin, wallMax)) {
                    out[0] = true;
                }
            }
        }
    }
    PlayScene* scene = getPlayScene();
    Engine::Point aitarget = scene->aiTank->Position;
    Engine::Point playertarget = scene->playerTank->Position;
    if (target==0 && Engine::Collider::IsCircleOverlap(nextPos, 0, aitarget, 24)) {
        out[1]=true;
    }
    if (target==1 && Engine::Collider::IsCircleOverlap(nextPos, 0, playertarget, 24)) {
        out[2]=true;
    }
    simDestroy = out[0]||out[1]||out[2];
    return out;
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
    Engine::Point aitarget = scene->aiTank->Position;
    Engine::Point playertarget = scene->playerTank->Position;
    if (target==0 && Engine::Collider::IsCircleOverlap(nextPos, 0, aitarget, 24)) {
        scene->aiTank->hurt(1);
        return true;
    }
    if (target==1 && Engine::Collider::IsCircleOverlap(nextPos, 0, playertarget, 24)) {
        scene->playerTank->hurt(1);
        return true;
    }
    
    return false;
}
