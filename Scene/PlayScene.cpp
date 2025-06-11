#include <allegro5/allegro.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <random>

#include <allegro5/allegro_primitives.h>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "UI/Component/Label.hpp"

#include "Object/Tank.hpp"

void PlayScene::Initialize() {
    srand(static_cast<unsigned int>(time(nullptr)));

    if(stageID == 1){
        LoadFixedMap();
    }else if(stageID == 2){
        GenerateMaze();
    }
    

    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
            mapState[y][x] = TILE_FLOOR;
    
    
    for (int y = 1; y <= 3; ++y)
        for (int x = 1; x <= 3; ++x)
            mapState[MapHeight - y][MapWidth - x] = TILE_FLOOR;

    GroundGroup = new Group();
    WallGroup = new Group();
    TankGroup = new Group();
    BulletGroup = new Group();

    AddNewObject(GroundGroup);
    AddNewObject(WallGroup);
    AddNewObject(TankGroup);
    AddNewObject(BulletGroup);

    for (int y = 0; y < MapHeight; ++y) {
        for (int x = 0; x < MapWidth; ++x) {

            if (mapState[y][x] == TILE_WALL) {
                WallGroup->AddNewObject(new Engine::Image("play/box.png", x * BlockSize, y * BlockSize));
            }else{
                GroundGroup->AddNewObject(new Engine::Image("play/floor.png", x * BlockSize, y * BlockSize));
            }
        }
    }

    playerTank = new Tank(100, 100, &mapState, MapWidth, MapHeight);
    aiTank = new AITank(1450, 760, &mapState, MapWidth, MapHeight);
    TankGroup->AddNewObject(playerTank);
    TankGroup->AddNewObject(aiTank);
}

void PlayScene::LoadFixedMap() {
    mapState = std::vector<std::vector<int>>(MapHeight, std::vector<int>(MapWidth, TILE_FLOOR));

    for (int x = 3; x < MapWidth-3; ++x) {
        mapState[3][x] = TILE_WALL;
        mapState[MapHeight - 4][x] = TILE_WALL;
    }
    for (int y = 3; y < MapHeight-3; ++y) {
        mapState[y][3] = TILE_WALL;
        mapState[y][MapWidth - 4] = TILE_WALL;
    }
    for (int x = 11; x < 14; ++x) {
        mapState[3][x] = TILE_FLOOR;
        mapState[MapHeight - 4][x] = TILE_FLOOR;
    }
    for (int y = 5; y < 8; ++y) {
        mapState[y][3] = TILE_FLOOR;
        mapState[y][MapWidth - 4] = TILE_FLOOR;
    }
    for (int x = 8; x < 17; ++x) {
        mapState[6][x] = TILE_WALL;
    }
}


void PlayScene::GenerateMaze() {
    mapState = std::vector<std::vector<int>>(MapHeight, std::vector<int>(MapWidth, TILE_WALL));

    auto InBounds = [&](int x, int y) {
        return x >= 0 && x < MapWidth && y >= 0 && y < MapHeight;
    };

    struct Cell { int x, y; };
    std::vector<Cell> frontier;
    auto AddFrontier = [&](int x, int y) {
        if (!InBounds(x, y)) return;
        if (mapState[y][x] != TILE_WALL) return;
        mapState[y][x] = -1;
        frontier.push_back({x, y});
    };

    int startX = (rand() % (MapWidth / 2)) * 2;
    int startY = (rand() % (MapHeight / 2)) * 2;
    mapState[startY][startX] = TILE_FLOOR;
    AddFrontier(startX + 2, startY);
    AddFrontier(startX - 2, startY);
    AddFrontier(startX, startY + 2);
    AddFrontier(startX, startY - 2);

    while (!frontier.empty()) {
        int idx = rand() % frontier.size();
        auto [fx, fy] = frontier[idx];
        frontier.erase(frontier.begin() + idx);

        std::vector<Cell> neighbors;
        if (InBounds(fx + 2, fy) && mapState[fy][fx + 2] == TILE_FLOOR)
            neighbors.push_back({fx + 2, fy});
        if (InBounds(fx - 2, fy) && mapState[fy][fx - 2] == TILE_FLOOR)
            neighbors.push_back({fx - 2, fy});
        if (InBounds(fx, fy + 2) && mapState[fy + 2][fx] == TILE_FLOOR)
            neighbors.push_back({fx, fy + 2});
        if (InBounds(fx, fy - 2) && mapState[fy - 2][fx] == TILE_FLOOR)
            neighbors.push_back({fx, fy - 2});

        if (!neighbors.empty()) {
            auto [nx, ny] = neighbors[rand() % neighbors.size()];
            mapState[fy][fx] = TILE_FLOOR;
            mapState[(fy + ny) / 2][(fx + nx) / 2] = TILE_FLOOR;

            if (rand() % 4 == 0) {
                std::vector<Cell> dirs = {{2, 0}, {-2, 0}, {0, 2}, {0, -2}};
                std::random_device rd;
                std::mt19937 g(rd());
                std::shuffle(dirs.begin(), dirs.end(), g);
                std::shuffle(dirs.begin(), dirs.end(), g);
                for (auto d : dirs) {
                    int ex = fx + d.x, ey = fy + d.y;
                    if (InBounds(ex, ey) && mapState[ey][ex] == TILE_WALL) {
                        mapState[ey][ex] = TILE_FLOOR;
                        mapState[(ey + fy) / 2][(ex + fx) / 2] = TILE_FLOOR;
                        break;
                    }
                }
            }

            AddFrontier(fx + 2, fy);
            AddFrontier(fx - 2, fy);
            AddFrontier(fx, fy + 2);
            AddFrontier(fx, fy - 2);
        } else {
            mapState[fy][fx] = TILE_WALL;
        }
    }

    for (int y = 0; y < MapHeight; ++y)
        for (int x = 0; x < MapWidth; ++x)
            if (mapState[y][x] == -1)
                mapState[y][x] = TILE_WALL;

    for (int y = 1; y < MapHeight - 1; ++y)
        for (int x = 1; x < MapWidth - 1; ++x)
            if (mapState[y][x] == TILE_WALL && rand() % 10 == 0)
                mapState[y][x] = TILE_FLOOR;
    // mapState = std::vector<std::vector<int>>(MapHeight, std::vector<int>(MapWidth, TILE_FLOOR));
}

void PlayScene::Terminate() {
    IScene::Terminate();
}

void PlayScene::Update(float deltaTime) {
    if(isGameOver) return;
    aiTank->Strategy();
    IScene::Update(deltaTime);
}

void PlayScene::Draw() const {
    IScene::Draw();
    if(isGameOver){
        auto screenSize = Engine::GameEngine::GetInstance().GetScreenSize();
        // mask in game over dialog
        al_draw_filled_rectangle(0, 0, screenSize.x, screenSize.y, al_map_rgba(255, 255, 255, 120));
        gameOverText->Draw();
        backButton->Draw();
        backButtonLabel->Draw();
    }
}

void PlayScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (playerTank)
        playerTank->OnKeyDown(keyCode);
}

void PlayScene::OnKeyUp(int keyCode) {
    IScene::OnKeyUp(keyCode);
    if (playerTank)
        playerTank->OnKeyUp(keyCode);
}

void PlayScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseDown(button, mx, my);
    if (button == 1 && playerTank) { // Left mouse button
        playerTank->Shoot(mx, my);
    }
}

void PlayScene::showGameOverDialog(const std::string& message){
    std::string winner;
    if(message.find("Win") != std::string::npos){
        winner = "You";
    } else if(message.find("Lose") != std::string::npos){
        winner = "AI";
    }
    SaveGameResult(winner);
    
    isGameOver = true;
    int halfW = Engine::GameEngine::GetInstance().GetScreenSize().x/2;
    int halfH = Engine::GameEngine::GetInstance().GetScreenSize().y/2;
    gameOverText = new Engine::Label(
        message, 
        "pirulen.ttf",
        48,
        halfW, halfH-50,
        10, 10, 10, 255, 0.5, 0.5
    );
    AddNewObject(gameOverText);
    backButton = new Engine::ImageButton(
        "stage-select/button.png",
        "stage-select/buttonOn.png",   // button img when hovered
        halfW-200, halfH+50,
        400, 100           // button size
    );
    backButton->SetOnClickCallback([this]() {   // `this` needs to be passed
        isGameOver = false;
        Engine::GameEngine::GetInstance().ChangeScene("home");
    });
    AddNewControlObject(backButton);
    backButtonLabel = new Engine::Label(
        "Back to Home",
        "pirulen.ttf",
        30,
        halfW, halfH+100,
        0, 0, 0, 255, 0.5, 0.5
    );
}

void PlayScene::SaveGameResult(const std::string& winner) {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    char timebuf[64];
    std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", now);

    std::ofstream ofs("Resource/history.txt", std::ios::app);
    if (ofs.is_open()) {
        ofs << timebuf << "," << stageID << "," << winner << std::endl;
        ofs.close();
    }
}