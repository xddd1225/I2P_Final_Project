#include "StageSelectScene.hpp"
#include "PlayScene.hpp"
#include "HomeScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

void StageSelectScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    AddNewObject(new Engine::Label("Select Stage", "PixelatedElegance.ttf", 72, halfW, halfH / 4, 255, 255, 255, 255, 0.5, 0.5));

    // Stage 1 button
    auto* btn1 = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH - 100, 400, 100);
    btn1->SetOnClickCallback(std::bind(&StageSelectScene::StageOnClick, this, 1));
    AddNewControlObject(btn1);
    AddNewObject(new Engine::Label("Stage 1", "PixelatedElegance.ttf", 48, halfW, halfH - 50, 0, 0, 0, 255, 0.5, 0.5));

    // Stage 2 button
    auto* btn2 = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH + 50, 400, 100);
    btn2->SetOnClickCallback(std::bind(&StageSelectScene::StageOnClick, this, 2));
    AddNewControlObject(btn2);
    AddNewObject(new Engine::Label("Stage 2", "PixelatedElegance.ttf", 48, halfW, halfH + 100, 0, 0, 0, 255, 0.5, 0.5));

    // Back button
    auto* backBtn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH + 200, 400, 100);
    backBtn->SetOnClickCallback(std::bind(&StageSelectScene::BackOnClick, this));
    AddNewControlObject(backBtn);
    AddNewObject(new Engine::Label("Back", "PixelatedElegance.ttf", 48, halfW, halfH + 250, 0, 0, 0, 255, 0.5, 0.5));
}

void StageSelectScene::Terminate() {
    IScene::Terminate();
}

void StageSelectScene::StageOnClick(int stage) {
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->stageID = stage;
    Engine::GameEngine::GetInstance().ChangeScene("play");
}

void StageSelectScene::BackOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("home");
}
