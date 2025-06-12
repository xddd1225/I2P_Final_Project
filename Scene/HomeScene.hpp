#ifndef HOME_SCENE_HPP
#define HOME_SCENE_HPP
#include <allegro5/allegro_audio.h>
#include "Engine/IScene.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"
#include "UI/Component/Image.hpp"

class HomeScene final : public Engine::IScene {
public:
    explicit HomeScene() = default;

    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
    void SettingOnClick();
    bool isSettingOpen = false;
    Engine::Image* settingBox = nullptr;
    Slider* bgmSlider = nullptr;
    Slider* sfxSlider = nullptr;
    Engine::Label* bgmLabel = nullptr;
    Engine::Label* sfxLabel = nullptr;
    Engine::Label* settingTitle = nullptr;

    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    bool isPlayingBGM = false;
    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);
};

#endif