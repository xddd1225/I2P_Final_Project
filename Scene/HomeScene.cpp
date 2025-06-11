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
        halfW, halfH / 3 + 60, 
        173, 136, 55, 255,      // r g b a
        0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/button.png", "stage-select/buttonOn.png", halfW - 200, halfH / 2 + 250, 400, 100);
    btn->SetOnClickCallback(std::bind(&HomeScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Play", "PixelatedElegance.ttf", 48, halfW, halfH / 2 + 305, 0, 0, 0, 255, 0.5, 0.5));
    
    btn = new Engine::ImageButton("stage-select/button.png", "stage-select/buttonOn.png", halfW - 200, halfH / 2 + 370, 400, 100);
    btn->SetOnClickCallback([]() {Engine::GameEngine::GetInstance().ChangeScene("history");});
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("History", "PixelatedElegance.ttf", 48, halfW, halfH / 2 + 425, 0, 0, 0, 255, 0.5, 0.5));
}

void HomeScene::Terminate() {
    IScene::Terminate();
}

void HomeScene::PlayOnClick(int stage){
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}