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

class PlayScene final : public Engine::IScene {
public:
    static const int MapWidth = 25;
    static const int MapHeight = 13;
    static const int BlockSize = 64;
    enum TileType {
        TILE_FLOOR,
        TILE_WALL,
    };

    Group* GroundGroup;
    Group* WallGroup;
    Group* TankGroup;
    Group* BulletGroup;

    std::vector<std::vector<int>> mapState;
    Tank* playerTank;
    AITank* aiTank;

    bool isGameOver = false;
    Engine::Label* gameOverText = nullptr;
    Engine::ImageButton* backButton = nullptr;
    Engine::Label* backButtonLabel = nullptr;
    int stageID = 0;

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

};

#endif   // PLAYSCENE_HPP

