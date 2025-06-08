#include <allegro5/allegro.h>
#include <cmath>
#include "Tank.hpp"
#include "Engine/Collider.hpp"
#include "Scene/PlayScene.hpp"

Tank::Tank(float x, float y, std::vector<std::vector<int>>* mapState, int mapWidth, int mapHeight)
    : Sprite("play/tank.png", x, y), mapState(mapState), mapWidth(mapWidth), mapHeight(mapHeight) {
    Speed = 100;
    Size.x = 64;
    Size.y = 64;
    Velocity = Engine::Point(0, 0);
}

void Tank::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    Engine::Point nextPos = Position + Velocity * Speed * deltaTime;

    if (!CheckCollision(nextPos)) {
        Position = nextPos;
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

    if (Velocity.x != 0 || Velocity.y != 0) {
        Rotation = atan2(Velocity.y, Velocity.x) - ALLEGRO_PI / 2;
    }
}

void Tank::OnKeyUp(int keyCode) {
    switch (keyCode) {
    case ALLEGRO_KEY_W:
    case ALLEGRO_KEY_S:
        Velocity.y = 0;
        break;
    case ALLEGRO_KEY_A:
    case ALLEGRO_KEY_D:
        Velocity.x = 0;
        break;
    }
    Rotation = atan2(Velocity.y, Velocity.x) - ALLEGRO_PI / 2;
}
