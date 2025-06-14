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
#include "Object/Explosion.hpp"
#include "Object/Coin.hpp"

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
    GroundEffectGroup = new Group();
    CoinGroup = new Group();

    AddNewObject(GroundGroup);
    AddNewObject(WallGroup);
    AddNewObject(TankGroup);
    AddNewObject(BulletGroup);
    AddNewObject(GroundEffectGroup);
    AddNewObject(CoinGroup);

    playerCoinCount = 0;

    for (int y = 0; y < MapHeight; ++y) {
        for (int x = 0; x < MapWidth; ++x) {

            if (mapState[y][x] == TILE_WALL) {
                WallGroup->AddNewObject(new Engine::Image("play/box.png", x * BlockSize, y * BlockSize));
            }else if (mapState[y][x] == TILE_BREAK_WALL){
                WallGroup->AddNewObject(new Engine::Image("play/straw.png", x * BlockSize, y * BlockSize));
            }else{
                GroundGroup->AddNewObject(new Engine::Image("play/oak.png", x * BlockSize, y * BlockSize));
            }
        }
    }

    playerTank = new Tank(100, 100, &mapState, MapWidth, MapHeight);
    aiTank = new AITank(1450, 760, &mapState, MapWidth, MapHeight);
    TankGroup->AddNewObject(playerTank);
    TankGroup->AddNewObject(aiTank);
    bgmId = AudioHelper::PlayBGM("ingamebgm.ogg");

    // Initialize coin spawn timer with a random offset
    coinSpawnTimer = static_cast<float>(rand()) / RAND_MAX * coinSpawnCooldown;

    // Item buttons
    int halfW = Engine::GameEngine::GetInstance().GetScreenSize().x / 2;
    int screenH = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfH = Engine::GameEngine::GetInstance().GetScreenSize().y / 2;
    int buttonWidth = 80;
    int buttonHeight = 80;
    int buttonSpacing = 20;
    int totalButtonsWidth = (buttonWidth * 3) + (buttonSpacing * 2);
    int startX = halfW - (totalButtonsWidth / 2);
    int buttonY = screenH - buttonHeight - 20; // 20 pixels from bottom

    // Item Button 1
    itemButton1 = new Engine::ImageButton(
        "play/speed.png",
        "play/speedOn.png", // Hover image (can be changed later)
        startX, buttonY,
        buttonWidth, buttonHeight
    );
    itemButton1->SetOnClickCallback([this]() {
        // Item 1: Increase Tank Speed
        if (playerTank) {
            if (!showItemButtons || playerCoinCount<3) return;
            playerCoinCount-=3;
            AudioHelper::PlayAudio("cashReg.wav");
            playerTank->IncreaseSpeed();
        }
    });
    AddNewControlObject(itemButton1);

    // Item Button 2
    itemButton2 = new Engine::ImageButton(
        "play/attack.png",
        "play/attackOn.png", // Hover image
        startX + buttonWidth + buttonSpacing, buttonY,
        buttonWidth, buttonHeight
    );
    itemButton2->SetOnClickCallback([this]() {
        // Item 2: Increase Fire Rate
        if (!showItemButtons || playerCoinCount<3) return;
        playerCoinCount-=3;
        AudioHelper::PlayAudio("cashReg.wav");
        if (playerTank) {
            playerTank->IncreaseFireRate();
        }
    });
    AddNewControlObject(itemButton2);

    // Item Button 3
    itemButton3 = new Engine::ImageButton(
        "play/heart.png",
        "play/heartOn.png",
        startX + (buttonWidth + buttonSpacing) * 2, buttonY,
        buttonWidth, buttonHeight
    );
    itemButton3->SetOnClickCallback([this]() {
        // Item 3: Heal Tank
        if (playerTank) {
            if (!showItemButtons || playerCoinCount<3) return;
            playerCoinCount-=3;
            AudioHelper::PlayAudio("cashReg.wav");
            playerTank->Heal(2); // Heal by 2 life points
        }
    });
    AddNewControlObject(itemButton3);

    // Pause Text
    pauseText = new Engine::Label(
        "PAUSED",
        "PixelatedElegance.ttf",
        60,
        halfW, halfH,
        255, 255, 255, 255, 0.5, 0.5
    );
    // Do not AddNewObject(pauseText) here, it will be drawn manually in Draw() when paused.

    // Initially hide item buttons
    itemButton1->Visible = false;
    itemButton2->Visible = false;
    itemButton3->Visible = false;
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

    // Delete item buttons (simplified due to API limitations)
    if (itemButton1) {
        delete itemButton1;
        itemButton1 = nullptr;
    }
    if (itemButton2) {
        delete itemButton2;
        itemButton2 = nullptr;
    }
    if (itemButton3) {
        delete itemButton3;
        itemButton3 = nullptr;
    }

    if (pauseText) {
        delete pauseText;
        pauseText = nullptr;
    }
    IScene::Terminate();
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
            if (mapState[y][x] == TILE_WALL){
                int randint = rand()%10;
                if (randint <= 1){
                    mapState[y][x] = TILE_FLOOR;
                }else if (randint <= 3) {
                    mapState[y][x] = TILE_BREAK_WALL;
                }
            }
                
    // mapState = std::vector<std::vector<int>>(MapHeight, std::vector<int>(MapWidth, TILE_FLOOR));
}


void PlayScene::UpdateTileImage(int x, int y) {
    // Clear all tiles and rebuild them
    WallGroup->Clear();
    GroundGroup->Clear();
    
    // Rebuild all tiles
    for (int y = 0; y < MapHeight; ++y) {
        for (int x = 0; x < MapWidth; ++x) {
            if (mapState[y][x] == TILE_WALL) {
                WallGroup->AddNewObject(new Engine::Image("play/box.png", x * BlockSize, y * BlockSize));
            } else if (mapState[y][x] == TILE_BREAK_WALL) {
                WallGroup->AddNewObject(new Engine::Image("play/straw.png", x * BlockSize, y * BlockSize));
            } else if (mapState[y][x] == TILE_FLOOR){
                GroundGroup->AddNewObject(new Engine::Image("play/oak.png", x * BlockSize, y * BlockSize));
                // GroundGroup->AddNewObject(new Engine::Image("play/light-2.png", x * BlockSize, y * BlockSize));
            }
        }
    }
}

void PlayScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
    // Delete item buttons (managed by Group, but manually setting to nullptr to be safe)
    if (itemButton1) { itemButton1 = nullptr; }
    if (itemButton2) { itemButton2 = nullptr; }
    if (itemButton3) { itemButton3 = nullptr; }

    if (pauseText) {
        delete pauseText;
        pauseText = nullptr;
    }
    IScene::Terminate(); // This will clear all groups and delete objects added with AddNewObject
}

void PlayScene::Update(float deltaTime) {
    if (isPaused) {
        // Optional: Update pause menu items if any, then return.
        return;
    }
    if(isGameOver){
        for(auto &obj : TankGroup->GetObjects()){
            if(dynamic_cast<Explosion*>(obj)){
                obj->Update(deltaTime);
            }
        }
        return;
    }
    if(aiTank) aiTank->Strategy();
    IScene::Update(deltaTime);

    // Coin spawning logic
    coinSpawnTimer -= deltaTime;
    if (coinSpawnTimer <= 0) {
        // Generate random position for the coin
        int coinX, coinY;
        bool validPositionFound = false;
        for (int i = 0; i < 100; ++i) { // Try up to 100 times to find a valid spot
            coinX = rand() % MapWidth;
            coinY = rand() % MapHeight;
            if (mapState[coinY][coinX] == TILE_FLOOR) {
                validPositionFound = true;
                break;
            }
        }

        if (validPositionFound) {
            // Add coin to the CoinGroup
            CoinGroup->AddNewObject(new Coin(coinX * BlockSize + BlockSize / 4, coinY * BlockSize + BlockSize / 4, BlockSize / 2, BlockSize / 2));
            Engine::LOG(Engine::INFO) << "Coin spawned at: (" << coinX << ", " << coinY << ")";
        }
        // Reset timer, adding some randomness
        coinSpawnTimer = coinSpawnCooldown + (static_cast<float>(rand()) / RAND_MAX * coinSpawnCooldown * 0.5f - coinSpawnCooldown * 0.25f);
    }
}

void PlayScene::Draw() const {
    IScene::Draw();
    // Draw Coin Count
    std::string coinText = "Coins: $" + std::to_string(playerCoinCount);
    Engine::Label coinLabel(coinText, "PixelatedElegance.ttf", 30, 10, 10, 0, 0, 0, 255, 0, 0);
    coinLabel.Draw();

    if(isGameOver){
        auto screenSize = Engine::GameEngine::GetInstance().GetScreenSize();
        // mask in game over dialog
        al_draw_filled_rectangle(0, 0, screenSize.x, screenSize.y, al_map_rgba(255, 255, 255, 120));
        gameOverText->Draw();
        backButton->Draw();
        backButtonLabel->Draw();
    }
    else{
        if (isPaused) {
            // Draw semi-transparent overlay
            auto screenSize = Engine::GameEngine::GetInstance().GetScreenSize();
            al_draw_filled_rectangle(0, 0, screenSize.x, screenSize.y, al_map_rgba(0, 0, 0, 150)); // Semi-transparent black
            // Draw PAUSED text
            if (pauseText) {
                pauseText->Draw();
            }
        }
        else if (playerTank) {
            Engine::Point tankPos = playerTank->Position;
            Engine::Point mouse = Engine::GameEngine::GetInstance().GetMousePosition();
            const float dotSpacing = 20.0f;
            const float dotRadius = 4.0f;
            ALLEGRO_COLOR color = al_map_rgba(100, 100, 100, 150);    // color of the line

            float dx = mouse.x - tankPos.x;
            float dy = mouse.y - tankPos.y;
            float dist = sqrt(dx * dx + dy * dy);
            float steps = dist / dotSpacing;
            for(int i = 0; i < steps; ++i){
                float t = i / steps;
                float x = tankPos.x + dx * t;
                float y = tankPos.y + dy * t;
                al_draw_filled_circle(x, y, dotRadius, color);
            }
        }
    }
}

void PlayScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_P) {
        isPaused = !isPaused;
        if (isPaused) {
            AudioHelper::StopBGM(bgmId);
        } else {
            bgmId = AudioHelper::PlayBGM("ingamebgm.ogg"); // Replay BGM from beginning
        }
    }
    if (keyCode == ALLEGRO_KEY_O) {
        showItemButtons = !showItemButtons;
        itemButton1->Visible = showItemButtons;
        itemButton2->Visible = showItemButtons;
        itemButton3->Visible = showItemButtons;
    }
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
    if (button == 1 && !isGameOver && playerTank) { // Left mouse button
        playerTank->Shoot(mx, my);
    }
}

void PlayScene::showGameOverDialog(const std::string& message){
    AudioHelper::StopBGM(bgmId);
    bgmId = AudioHelper::PlayAudio((message == "You Win!") ? "respect.wav" : "die.ogg");
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
        "PixelatedElegance.ttf",
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
        "PixelatedElegance.ttf",
        30,
        halfW, halfH+105,
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