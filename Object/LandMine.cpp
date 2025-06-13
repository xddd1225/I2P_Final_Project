#include "LandMine.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Object/LandMine.hpp"
#include <iostream>
#include "Scene/PlayScene.hpp"
#include "Object/Explosion.hpp"

LandMine::LandMine(float x, float y, float delayTime) :
    Engine::Sprite("play/explosion-1.png", x, y), timer(delayTime), exploded(false) {
    Anchor = Engine::Point(0.5f, 0.5f);
}

void LandMine::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    if (exploded) return;

    timer -= deltaTime;
    if (timer <= 0) {
        Explode();
        exploded = true;
    }
}
PlayScene* LandMine::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

void LandMine::Explode() {
    PlayScene* scene = getPlayScene();
    
    // Create explosion effect
    scene->TankGroup->AddNewObject(new Explosion(Position.x, Position.y));
    
    // Play explosion sound
    AudioHelper::PlayAudio("explosion.wav");
    
    // Get map state
    auto& map = scene->mapState;
    int w = scene->MapWidth;
    int h = scene->MapHeight;

    // Get landmine center position (tile coordinates)
    int cx = floor(Position.x / scene->BlockSize);
    int cy = floor(Position.y / scene->BlockSize);

    // Explosion radius
    float radius = 1.5;
    float explosionRadius = radius * scene->BlockSize; // Convert to pixel radius

    // Check for tank damage
    if (scene->playerTank) {
        float dist = (scene->playerTank->Position - Position).Magnitude();
        if (dist <= explosionRadius) {
            scene->playerTank->hurt(1);
        }
    }
    if (scene->aiTank) {
        float dist = (scene->aiTank->Position - Position).Magnitude();
        if (dist <= explosionRadius) {
            scene->aiTank->hurt(1);
        }
    }

    // Scan surrounding tiles for explosion effect
    bool mapChanged = false;
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            int nx = cx + dx;
            int ny = cy + dy;
            if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                if (map[ny][nx] == scene->TILE_BREAK_WALL) {
                    map[ny][nx] = scene->TILE_FLOOR;
                    // std::cout << "update tile at " << nx << "," << ny << std::endl;
                    mapChanged = true;
                }
            }
        }
    }

    // Update the map if any changes were made
    if (mapChanged) {
        scene->UpdateTileImage(cx, cy);
    }

    // Remove the landmine after all updates are done
    scene->RemoveObject(objectIterator);
}