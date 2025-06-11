#include "HistoryScene.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"
#include <fstream>
#include <sstream>
#include <vector>

void HistoryScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int startY = 125;
    int lineHeight = 40;

    AddNewObject(new Engine::Label("History", "PixelatedElegance.ttf", 64, w / 2, 60, 255, 255, 255, 255, 0.5, 0.5));

    AddNewObject(new Engine::Label("Time", "pirulen.ttf", 32, w/4, startY, 255, 255, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Stage", "pirulen.ttf", 32, w/2, startY, 255, 255, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Winner", "pirulen.ttf", 32, (w/4)*3 , startY, 255, 255, 0, 255, 0.5, 0.5));

    LoadHistory();

    int y = startY + lineHeight;

    for (const auto& entry : historyEntries) {
        std::stringstream ss(entry);
        std::string time, stage, winner;

        if (std::getline(ss, time, ',') && std::getline(ss, stage, ',') && std::getline(ss, winner, ',')) {
            AddNewObject(new Engine::Label(time, "pirulen.ttf", 28, w/4, y, 255, 255, 255, 255, 0.5, 0.5));
            AddNewObject(new Engine::Label(stage, "pirulen.ttf", 28, w/2, y, 255, 255, 255, 255, 0.5, 0.5));
            AddNewObject(new Engine::Label(winner, "pirulen.ttf", 28, (w/4)*3, y, 255, 255, 255, 255, 0.5, 0.5));
        }
        y += lineHeight;
    }

    auto* btn = new Engine::ImageButton("stage-select/button.png", "stage-select/buttonOn.png", w / 2 - 200, h - 120, 400, 100);
    btn->SetOnClickCallback(std::bind(&HistoryScene::BackOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "PixelatedElegance.ttf", 48, w / 2, h - 65, 0, 0, 0, 255, 0.5, 0.5));
}

void HistoryScene::LoadHistory() {
    std::ifstream file("Resource/history.txt");
    std::string line;
    historyEntries.clear();
    while (std::getline(file, line)) {
        if (!line.empty())
            historyEntries.push_back(line);
    }
}

void HistoryScene::Terminate() {
    historyEntries.clear();
    IScene::Terminate();
}

void HistoryScene::BackOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("home");
}
