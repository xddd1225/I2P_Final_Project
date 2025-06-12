#include "StageSelectScene.hpp"
#include "PlayScene.hpp"
#include "HomeScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/AudioHelper.hpp"

void StageSelectScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    AddNewObject(new Engine::Image("stage-select/mapBackground.png", 0, 0, w, h));

    AddNewObject(new Engine::Label("Select Map", "PixelatedElegance.ttf", 72, halfW, halfH / 4, 255, 255, 255, 255, 0.5, 0.5));

    // Stage 1 button
    auto* btn1 = new Engine::ImageButton("stage-select/button.png", "stage-select/buttonOn.png", halfW - 200, halfH - 100, 400, 100);
    btn1->SetOnClickCallback(std::bind(&StageSelectScene::StageOnClick, this, 1));
    AddNewControlObject(btn1);
    AddNewObject(new Engine::Label("Map 1", "PixelatedElegance.ttf", 48, halfW, halfH - 45, 0, 0, 0, 255, 0.5, 0.5));

    // Stage 2 button
    auto* btn2 = new Engine::ImageButton("stage-select/button.png", "stage-select/buttonOn.png", halfW - 200, halfH + 55, 400, 100);
    btn2->SetOnClickCallback(std::bind(&StageSelectScene::StageOnClick, this, 2));
    AddNewControlObject(btn2);
    AddNewObject(new Engine::Label("Random", "PixelatedElegance.ttf", 48, halfW, halfH + 110, 0, 0, 0, 255, 0.5, 0.5));

    // Back button
    auto* backBtn = new Engine::ImageButton("stage-select/button.png", "stage-select/buttonOn.png", halfW - 200, halfH + 200, 400, 100);
    backBtn->SetOnClickCallback(std::bind(&StageSelectScene::BackOnClick, this));
    AddNewControlObject(backBtn);
    AddNewObject(new Engine::Label("Back", "PixelatedElegance.ttf", 48, halfW, halfH + 260, 0, 0, 0, 255, 0.5, 0.5));
}

void StageSelectScene::Terminate() {
    IScene::Terminate();
}

void StageSelectScene::StageOnClick(int stage) {
    HomeScene *homeScene = dynamic_cast<HomeScene *>(Engine::GameEngine::GetInstance().GetScene("home"));
    // stop menu BGM when entering playscene
    if (homeScene) {
        AudioHelper::StopBGM(homeScene->bgmId);
        homeScene->isPlayingBGM = false;
    }
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->stageID = stage;
    Engine::GameEngine::GetInstance().ChangeScene("play");
}

void StageSelectScene::BackOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("home");
}
