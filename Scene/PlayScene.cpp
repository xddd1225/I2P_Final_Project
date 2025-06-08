#include <algorithm>
#include <allegro5/allegro.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>
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
    GenerateMaze();

    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
            mapState[y][x] = TILE_FLOOR;

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
                WallGroup->AddNewObject(new Engine::Image("play/dirt.png", x * BlockSize, y * BlockSize));
            }else{
                GroundGroup->AddNewObject(new Engine::Image("play/floor.png", x * BlockSize, y * BlockSize));
            }
        }
    }

    playerTank = new Tank(100, 100, &mapState, MapWidth, MapHeight);
    AITank = new Tank(800, 300, &mapState, MapWidth, MapHeight);
    TankGroup->AddNewObject(playerTank);
    TankGroup->AddNewObject(AITank);
}

void PlayScene::GenerateMaze() {
    mapState = std::vector<std::vector<int>>(MapHeight, std::vector<int>(MapWidth, TILE_WALL));

    for (int y = 0; y < MapHeight; ++y)
        for (int x = 0; x < MapWidth; ++x)
            mapState[y][x] = TILE_WALL;

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
}

void PlayScene::Terminate() {
    IScene::Terminate();
}

void PlayScene::Update(float deltaTime) {
    IScene::Update(deltaTime);
}

void PlayScene::Draw() const {
    IScene::Draw();
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