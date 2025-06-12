#include "HomeScene.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void HomeScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;
    AddNewObject(new Engine::Image("stage-select/background.png", 0, 0, w, h));
    AddNewObject(new Engine::Label("FINAL TANK", "PixelatedElegance.ttf", 120, 
        halfW, halfH / 3 + 80, 
        110, 105, 59, 255,      // r g b a
        0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/button.png", "stage-select/buttonOn.png", halfW - 200, halfH / 2 + 250, 400, 100);
    btn->SetOnClickCallback(std::bind(&HomeScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Play", "PixelatedElegance.ttf", 48, halfW, halfH / 2 + 305, 0, 0, 0, 255, 0.5, 0.5));
    
    btn = new Engine::ImageButton("stage-select/button.png", "stage-select/buttonOn.png", halfW - 200, halfH / 2 + 370, 400, 100);
    btn->SetOnClickCallback([]() {Engine::GameEngine::GetInstance().ChangeScene("history");});
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("History", "PixelatedElegance.ttf", 48, halfW, halfH / 2 + 425, 0, 0, 0, 255, 0.5, 0.5));

    // setting button
    btn = new Engine::ImageButton("stage-select/button.png", "stage-select/buttonOn.png", w-190, halfH / 2 + 480, 100, 100);
    btn->SetOnClickCallback(std::bind(&HomeScene::SettingOnClick, this));
    AddNewControlObject(btn);
    // gear icon
    AddNewObject(new Engine::Image("stage-select/setting.png", w-175, halfH/2 + 490, 70, 70));

    if(!isPlayingBGM){
        // bgmId = AudioHelper::PlayBGM("BF2Theme.wav");
        bgmInstance = AudioHelper::PlaySample("BF2Theme.wav", true, AudioHelper::BGMVolume);
        isPlayingBGM = true;
    }
}

void HomeScene::Terminate() {
    IScene::Terminate();
}

void HomeScene::PlayOnClick(int stage){
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

void HomeScene::SettingOnClick() {
    if(isSettingOpen){
        isSettingOpen = false;
        Engine::GameEngine::GetInstance().ChangeScene("home");
        return;
    }

    isSettingOpen = true;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int boxW = 500, boxH = 300;
    int boxX = w / 2 - boxW / 2, boxY = h / 2 - boxH / 2;

    // rectangle background
    settingBox = new Engine::Image("stage-select/settingBG.png", boxX, boxY, boxW, boxH);
    AddNewObject(settingBox);

    // title
    settingTitle = new Engine::Label("Volume Setting", "PixelatedElegance.ttf", 32, w/2, boxY + 50, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(settingTitle);

    // BGM slider
    bgmLabel = new Engine::Label("BGM", "PixelatedElegance.ttf", 28, boxX + 100, boxY + 110, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(bgmLabel);
    bgmSlider = new Slider(boxX + 180, boxY + 95, 250, 30);
    bgmSlider->SetValue(AudioHelper::BGMVolume);
    bgmSlider->SetOnValueChangedCallback(std::bind(&HomeScene::BGMSlideOnValueChanged, this, std::placeholders::_1));
    AddNewControlObject(bgmSlider);

    // SFX slider
    sfxLabel = new Engine::Label("SFX", "PixelatedElegance.ttf", 28, boxX + 100, boxY + 190, 255, 255, 255, 255, 0.5, 0.5);
    AddNewObject(sfxLabel);
    sfxSlider = new Slider(boxX + 180, boxY + 175, 250, 30);
    sfxSlider->SetValue(AudioHelper::SFXVolume);
    sfxSlider->SetOnValueChangedCallback(std::bind(&HomeScene::SFXSlideOnValueChanged, this, std::placeholders::_1));
    AddNewControlObject(sfxSlider);
}

void HomeScene::BGMSlideOnValueChanged(float value) {
    AudioHelper::ChangeSampleVolume(bgmInstance, value);
    AudioHelper::BGMVolume = value;
}
void HomeScene::SFXSlideOnValueChanged(float value) {
    AudioHelper::SFXVolume = value;
}