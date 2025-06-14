#include "Coin.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Collider.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/LOG.hpp"

PlayScene* Coin::getPlayScene() {
    return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Coin::Coin(float x, float y, float w, float h)
    : Engine::Sprite("play/coin.png", x, y, w, h) {
    // Set collision radius if needed, or adjust size for better collision detection
    CollisionRadius = 32.0f;
}

void Coin::Update(float deltaTime) {
    Sprite::Update(deltaTime);

    PlayScene* scene = getPlayScene();
    if (scene->playerTank && Engine::Collider::IsCircleOverlap(Position, CollisionRadius, scene->playerTank->Position, scene->playerTank->CollisionRadius)) {
        // Collision with player tank
        scene->playerCoinCount++;
        Engine::LOG(Engine::INFO) << "Coin collected! Total coins: " << scene->playerCoinCount;
        AudioHelper::PlayAudio("coin.wav");
        scene->CoinGroup->RemoveObject(objectIterator);
        // Mark for deletion if object is managed by group
        // Visible = false; // Hide immediately
    }
} 