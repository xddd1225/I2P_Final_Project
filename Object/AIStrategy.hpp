#ifndef AISTRATEGY_HPP
#define AISTRATEGY_HPP

#include <allegro5/allegro.h>
// #include "AITank.hpp"
// #include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
// #include "Engine/Point.hpp"
// #include "Engine/Sprite.hpp"
// #include "Scene/PlayScene.hpp"
// #include "Bullet.hpp"
// #include <string>
// #include <vector>
// #include <map>
// #include <cmath>
#include "Object/AITank.hpp"
#include "Object/Tank.hpp"
#include "Object/Bullet.hpp"

using namespace std;
using Point = Engine::Point;




class PlayScene;
class State {
public:
    State(PlayScene* original);
    State(const State& state);
    void ApplyAction(const Action& act);
    void Simulate(float deltaTime);
    float EvaluateScore() const;
    bool IsTerminal() const;
    void Step(float dt);
    enum TileType {
        TILE_FLOOR,
        TILE_WALL,
    };


    vector<vector<int>> mapState;
    vector<Bullet*> bullets;
    AITank* ai;
    Tank* player;
    int score;
    int BlockSize;
    int MapHeight;
    int MapWidth;
    int hitCount;
};


class MonteCarloAI {
public:
    MonteCarloAI(int rollouts, float timePerStep, int stepsPerRollout){
        this->numRollouts=rollouts;
        this->deltaTime = timePerStep;
        this->rolloutSteps = stepsPerRollout;
    }

    Action DecideBestAction(const State& current);

private:
    float SimulateRollout(const State& initial, const Action& act); // 跑一次模擬
    vector<Action> GetAllActions(); // 產生 80 組合法行為

private:
    int numRollouts;
    float deltaTime;
    int rolloutSteps;
};
#endif // AISTRATEGY_HPP