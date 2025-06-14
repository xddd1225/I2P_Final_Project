#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Object/Tank.hpp"
#include "Object/AITank.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"
#include "Object/Coin.hpp"

class PlayScene final : public Engine::IScene {
public:
    static const int MapWidth = 25;
    static const int MapHeight = 13;
    static const int BlockSize = 64;
    enum TileType {
        TILE_FLOOR,
        TILE_WALL,
        TILE_BREAK_WALL
    };

    ALLEGRO_SAMPLE_ID bgmId;

    Group* GroundGroup;
    Group* WallGroup;
    Group* TankGroup;
    Group* BulletGroup;
    Group* GroundEffectGroup;

    Group* CoinGroup = nullptr;
    int playerCoinCount = 0;

    std::vector<std::vector<int>> mapState;
    Tank* playerTank;
    AITank* aiTank;

    bool isGameOver = false;
    Engine::Label* gameOverText = nullptr;
    Engine::ImageButton* backButton = nullptr;
    Engine::Label* backButtonLabel = nullptr;
    int stageID = 0;
    bool isPaused = false;
    Engine::Label* pauseText = nullptr;

    // Item Buttons (New member variables)
    Engine::ImageButton* itemButton1 = nullptr;
    Engine::ImageButton* itemButton2 = nullptr;
    Engine::ImageButton* itemButton3 = nullptr;
    bool showItemButtons = false; // To control visibility

    // Coin spawning variables
    float coinSpawnTimer = 0.0f;
    const float coinSpawnCooldown = 5.0f; // Spawn a coin every 5 seconds on average

    explicit PlayScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnKeyDown(int keyCode) override;
    void OnKeyUp(int keyCode) override;
    void GenerateMaze();
    void LoadFixedMap();
    void OnMouseDown(int button, int mx, int my) override;
    void showGameOverDialog(const std::string& message);
    void SaveGameResult(const std::string& winner);
    void UpdateTileImage(int x, int y);
};

#endif   // PLAYSCENE_HPP

