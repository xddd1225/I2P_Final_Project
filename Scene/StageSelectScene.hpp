#ifndef STAGE_SELECT_SCENE_HPP
#define STAGE_SELECT_SCENE_HPP

#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"

class StageSelectScene final : public Engine::IScene {
public:
    explicit StageSelectScene() = default;
    void Initialize() override;
    void Terminate() override;
    void StageOnClick(int stage);
    void BackOnClick();
};

#endif // STAGE_SELECT_SCENE_HPP
