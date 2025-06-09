#include <allegro5/allegro.h>
#include <cmath>
#include "Tank.hpp"
#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "Bullet.hpp"
#include <iostream>

PlayScene *Tank::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Tank::Tank(float x, float y, std::vector<std::vector<int>>* mapState, int mapWidth, int mapHeight)
    : Sprite("play/tank.png", x, y), mapState(mapState), mapWidth(mapWidth), mapHeight(mapHeight) {
    Speed = 100;
    Size.x = 64;
    Size.y = 64;
    life = 3;
    Velocity = Engine::Point(0, 0);
}

Tank::Tank(const Tank& other)
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

void Tank::Update(float deltaTime) {
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

bool Tank::CheckCollision(Engine::Point nextPos) {
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

void Tank::OnKeyDown(int keyCode) {
    pressedKey[keyCode] = true;
    switch (keyCode) {
    case ALLEGRO_KEY_W:
        Velocity.y = -1;
        break;
    case ALLEGRO_KEY_S:
        Velocity.y = 1;
        break;
    case ALLEGRO_KEY_A:
        Velocity.x = -1;
        break;
    case ALLEGRO_KEY_D:
        Velocity.x = 1;
        break;
    }
    if (Velocity.x!=0&&Velocity.y!=0){
        Velocity.x = (Velocity.x>0?1:-1)*sqrt(2)/2;
        Velocity.y = (Velocity.y>0?1:-1)*sqrt(2)/2;
    }
    if (Velocity.x != 0 || Velocity.y != 0) {
        Rotation = atan2(Velocity.y, Velocity.x) - ALLEGRO_PI / 2;
    }
}

void Tank::OnKeyUp(int keyCode) {
    pressedKey[keyCode] = false;
    switch (keyCode) {
    case ALLEGRO_KEY_W: {
        Velocity.y = 0;
        if (pressedKey[ALLEGRO_KEY_S])
            Velocity.y=1;
        break;
        }
    case ALLEGRO_KEY_S:{
        Velocity.y = 0;
        if (pressedKey[ALLEGRO_KEY_W])
            Velocity.y=-1;
        break;
    }
    case ALLEGRO_KEY_A:{
        Velocity.x = 0;
        std::cout << pressedKey[ALLEGRO_KEY_D]  << std::endl;
        if (pressedKey[ALLEGRO_KEY_D])
            Velocity.x=1;
        break;
    }
    case ALLEGRO_KEY_D:{
        Velocity.x = 0;
        if (pressedKey[ALLEGRO_KEY_A])
            Velocity.x=-1;
        break;
    }
    }
    Rotation = atan2(Velocity.y, Velocity.x) - ALLEGRO_PI / 2;
}

void Tank::hurt(int damage) {
    life -= damage;
    if (life <= 0){
        // getPlayScene()->RemoveObject(objectIterator);
    }
}

void Tank::Shoot(float targetX, float targetY) {
    if (shootCooldown <= 0) {
        // Create a new bullet at the tank's position
        Bullet* bullet = new Bullet(Position.x, Position.y, targetX, targetY, mapState, mapWidth, mapHeight);
        getPlayScene()->BulletGroup->AddNewObject(bullet);
        shootCooldown = SHOOT_COOLDOWN_TIME;
    }
}
