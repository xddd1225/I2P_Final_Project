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
    maxlife = life = 3;
    Velocity = Engine::Point(0, 0);
}

AITank::AITank(const AITank& other) 
    : Engine::Sprite(other) // 如果 Sprite 有複製建構子，建議初始化列表帶入
{
    mapState = other.mapState;
    mapWidth = other.mapWidth;
    mapHeight = other.mapHeight;
    Speed = other.Speed;
    aggressive = other.aggressive;
    Size = other.Size;
    life = other.life;
    Velocity = other.Velocity;
    shootCooldown = other.shootCooldown;
}

void AITank::ApplyAction(const Action& act){
    Velocity = act.direction.Normalize();
    shoot = act.shoot;
}

void AITank::Strategy() {
    // Position.x
    Engine::Point playerPos = getPlayScene()->playerTank->Position;
    float dist = (playerPos - Position).Magnitude();

    // 切換模式（你可以根據更複雜的策略切換）
    State snapshot = State(getPlayScene());
    MonteCarloAI ai(15, 0.3f, 30);
    Action best = ai.DecideBestAction(snapshot);
    ApplyAction(best);
}

void AITank::Update(float deltaTime) {
    
    // aggresive mode
    moodTimer += deltaTime;
    if (moodTimer > 3.0f) {
        moodTimer = 0;
        if (rand() % 2 == 0) {
            aggressive = true;
        } else {
            aggressive = false;
        }
    }

    PropertyChange(deltaTime);
    PlayScene* scene = getPlayScene();
    if (scene->isGameOver) return;

    int targetX = scene->playerTank->Position.x;
    int targetY = scene->playerTank->Position.y;
    if (shoot) {
        Shoot(targetX, targetY);
        shoot = false;
    }
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

void AITank::Draw() const {
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

void AITank::hurt(int damage) {
    life -= damage;
    if (life <= 0){
        PlayScene* scene = getPlayScene();
        // scene->RemoveObject(objectIterator);
        scene->showGameOverDialog("You Win!");
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
