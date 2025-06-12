#include "Explosion.hpp"
#include "Engine/Resources.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"

Explosion::Explosion(float x, float y)
    : Sprite("play/explosion-1.png", x, y, 64, 64, 0.5, 0.5) {
    // Add all explosion frames
    for (int i = 1; i <= 5; ++i)
        frames.push_back("play/explosion-" + std::to_string(i) + ".png");
    // Set initial image
    bmp = Engine::Resources::GetInstance().GetBitmap(frames[0]);
}

void Explosion::Update(float deltaTime) {
    timer += deltaTime;
    if (timer >= frameTime) {
        timer = 0;
        ++currentFrame;
        if (currentFrame < frames.size()) {
            bmp = Engine::Resources::GetInstance().GetBitmap(frames[currentFrame]);
        } else {
            // Remove this explosion from the scene
            auto* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
            if (scene) scene->RemoveObject(objectIterator);
        }
    }
}