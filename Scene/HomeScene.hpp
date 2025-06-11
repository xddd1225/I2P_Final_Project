#ifndef HOME_SCENE_HPP
#define HOME_SCENE_HPP
#include <allegro5/allegro_audio.h>
#include "Engine/IScene.hpp"

class HomeScene final : public Engine::IScene {
public:
    explicit HomeScene() = default;

    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
};

#endif