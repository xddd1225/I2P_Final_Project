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
    : Sprite("play/tank.png", x, y), mapState(mapState), mapWidth(mapWidth), mapHeight(mapHeight){
    Speed = 300;
    Size.x = 64;
    Size.y = 64;
    maxlife = life = 5;
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
    if (getPlayScene()->isGameOver) return;

    // 更新衝刺狀態
    if (isDashing) {
        dashTimer -= deltaTime;
        if (dashTimer <= 0) {
            isDashing = false;
            dashCooldownTimer = dashCooldown;
        }
    } else if (dashCooldownTimer > 0) {
        dashCooldownTimer -= deltaTime;
    }
    float accel = AccelerationSpeed * deltaTime;
    float friction = 5.0f * deltaTime;
    float speedMultiplier = isDashing ? 2.5f : 1.0f;  // 衝刺加速

    // X 軸處理
    if (std::abs(TargetVelocity.x) > 0.01f) {
        Velocity.x += (TargetVelocity.x - Velocity.x) * std::min(1.0f, accel);
    } else {
        if (std::abs(Velocity.x) < friction) Velocity.x = 0;
        else Velocity.x -= std::copysign(friction, Velocity.x);
    }

    // Y 軸處理
    if (std::abs(TargetVelocity.y) > 0.01f) {
        Velocity.y += (TargetVelocity.y - Velocity.y) * std::min(1.0f, accel);
    } else {
        if (std::abs(Velocity.y) < friction) Velocity.y = 0;
        else Velocity.y -= std::copysign(friction, Velocity.y);
    }

    Engine::Point normVel = Velocity.Normalize();
    Engine::Point fullMove = normVel * Speed * deltaTime * speedMultiplier;
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

void Tank::Draw() const {
    Sprite::Draw();

    const float barWidth = 40;
    const float barHeight = 6;
    const float offsetY = -Size.y / 2 - 10;

    float x = Position.x;
    float y = Position.y + offsetY;

    float ratio = std::max(0.0f, std::min(1.0f, static_cast<float>(life) / maxlife));

    al_draw_filled_rectangle(
        x - barWidth / 2, y,
        x + barWidth / 2, y + barHeight,
        al_map_rgb(100, 100, 100)
    );

    al_draw_filled_rectangle(
        x - barWidth / 2, y,
        x - barWidth / 2 + barWidth * ratio, y + barHeight,
        al_map_rgb(255, 0, 0)
    );

    al_draw_rectangle(
        x - barWidth / 2, y,
        x + barWidth / 2, y + barHeight,
        al_map_rgb(0, 0, 0), 1.0
    );
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
        TargetVelocity.y = -1;
        break;
    case ALLEGRO_KEY_S:
        TargetVelocity.y = 1;
        break;
    case ALLEGRO_KEY_A:
        TargetVelocity.x = -1;
        break;
    case ALLEGRO_KEY_D:
        TargetVelocity.x = 1;
        break;
    case ALLEGRO_KEY_SPACE: {
        if (!isDashing && dashCooldownTimer <= 0 && (Velocity.x != 0 || Velocity.y != 0)) {
            isDashing = true;
            dashTimer = dashDuration;
        }
        break;
    }
    }

    if (TargetVelocity.x != 0 && TargetVelocity.y != 0) {
        TargetVelocity.x = (TargetVelocity.x > 0 ? 1 : -1) * std::sqrt(2) / 2;
        TargetVelocity.y = (TargetVelocity.y > 0 ? 1 : -1) * std::sqrt(2) / 2;
    }

    if (TargetVelocity.x != 0 || TargetVelocity.y != 0) {
        Engine::Point norm = TargetVelocity.Normalize();
        Rotation = atan2(norm.y, norm.x) - ALLEGRO_PI / 2;
    }
}

void Tank::OnKeyUp(int keyCode) {
    pressedKey[keyCode] = false;
    switch (keyCode) {
    case ALLEGRO_KEY_W:
        if (pressedKey[ALLEGRO_KEY_S])
            TargetVelocity.y = 1;
        else
            TargetVelocity.y = 0;
        break;
    case ALLEGRO_KEY_S:
        if (pressedKey[ALLEGRO_KEY_W])
            TargetVelocity.y = -1;
        else
            TargetVelocity.y = 0;
        break;
    case ALLEGRO_KEY_A:
        if (pressedKey[ALLEGRO_KEY_D])
            TargetVelocity.x = 1;
        else
            TargetVelocity.x = 0;
        break;
    case ALLEGRO_KEY_D:
        if (pressedKey[ALLEGRO_KEY_A])
            TargetVelocity.x = -1;
        else
            TargetVelocity.x = 0;
        break;
    }
    // 更新旋轉角度
    if (TargetVelocity.x != 0 || TargetVelocity.y != 0) {
        Engine::Point norm = TargetVelocity.Normalize();
        Rotation = atan2(norm.y, norm.x) - ALLEGRO_PI / 2;
    }
}

void Tank::hurt(int damage) {
    life -= damage;
    if (life <= 0){
        PlayScene* scene = getPlayScene();
        scene->RemoveObject(objectIterator);
        scene->playerTank = nullptr;    // prevent crash (e.g., move tank after game over)
        scene->showGameOverDialog("You Lose!");
    }
}

void Tank::Shoot(float targetX, float targetY) {
    if (shootCooldown <= 0) {
        // Create a new bullet at the tank's position
        Bullet* bullet = new Bullet(Position.x, Position.y, targetX, targetY, mapState, mapWidth, mapHeight, 0);
        getPlayScene()->BulletGroup->AddNewObject(bullet);
        shootCooldown = SHOOT_COOLDOWN_TIME;
    }
}
