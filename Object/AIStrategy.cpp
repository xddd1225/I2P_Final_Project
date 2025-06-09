#include "Object/AIStrategy.hpp"
#include "Object/Action.hpp"
#include <Scene/PlayScene.hpp>
#include <vector>
#include <queue>
#include <iostream>
#include "Object/Bullet.hpp"

// // const Engine::Point DirectionVectors[];
// // const Engine::Point actionSet[];
// using namespace std;

State::State(PlayScene* scene) {
    mapState = scene->mapState;
    ai = new AITank(*scene->aiTank);
    player = new Tank(*scene->playerTank);
    BlockSize = scene->BlockSize;
    MapHeight = scene->MapHeight;
    MapWidth = scene->MapWidth;
    bullets.clear();
    for (auto& obj : scene->BulletGroup->GetObjects()) {
        Bullet* b = dynamic_cast<Bullet*>(obj);
        if (b) {
            bullets.push_back(new Bullet(*b));  // 假設 Bullet 有正確的複製建構子
        }
    }
}

State::State(const State& state) {
    mapState = state.mapState;
    ai = new AITank(*state.ai);
    player = new Tank(*state.player);
    BlockSize = state.BlockSize;
    MapHeight = state.MapHeight;
    MapWidth = state.MapWidth;
    bullets.clear();
    PlayScene* scene = ai->getPlayScene();
    for (auto& obj : scene->BulletGroup->GetObjects()) {
        Bullet* b = dynamic_cast<Bullet*>(obj);
        if (b) {
            bullets.push_back(new Bullet(*b));  // 假設 Bullet 有正確的複製建構子
        }
    }
}

// State::~State() {
//     delete ai;
//     delete player;
// }

// State& operator=(const State& other) {
//     if (this != &other) {
//         delete scene;
//         scene = other.scene->Clone();
//     }
//     return *this;
// }


void State::ApplyAction(const Action& act){
    ai->ApplyAction(act);
}
void State::Simulate(float deltaTime){
    const float fixedStep = 0.01f;
    float elapsed = 0.0f;
    while (elapsed + fixedStep <= deltaTime) {
        ai->PropertyChange(fixedStep);
        elapsed += fixedStep;
    }
    // 最後補上不足的時間（如果有）
    if (elapsed < deltaTime) {
        ai->PropertyChange(deltaTime - elapsed);
    }
}

float BulletThreatToPoint(Bullet* b, Engine::Point targetPos) {
    if ((targetPos - b->Position).Dot(b->Velocity) < 0) return 0;
    Point ap = targetPos - b->Position;
    Point v = b->Velocity;
    float cross = std::abs(ap.x * v.y - ap.y * v.x);
    float dist = cross / v.Magnitude();
    if (dist==0) dist = 1;
    float absDist = (b->Position - targetPos).Magnitude();

    // b-targetPos
    return (1000-dist)*(1000-absDist)/1e4;
}

float State::EvaluateScore() const {
    // BFS distance
    constexpr int SplitFactor = 4;
    const int SubBlockSize = BlockSize / SplitFactor;
    const int W = MapWidth * SplitFactor;
    const int H = MapHeight * SplitFactor;

    std::vector<std::vector<int>> fineMap(H, std::vector<int>(W, 0));
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            if (mapState[y / SplitFactor][x / SplitFactor] == PlayScene::TILE_WALL) {
                fineMap[y][x] = 1;
            }
        }
    }
    std::vector<std::vector<int>> dist(H, std::vector<int>(W, INT_MAX));
    std::queue<Point> q;

    Point start(static_cast<int>(ai->Position.x / SubBlockSize),
                static_cast<int>(ai->Position.y / SubBlockSize));
    Point goal(static_cast<int>(player->Position.x / SubBlockSize),
               static_cast<int>(player->Position.y / SubBlockSize));

    if (start.x < 0 || start.x >= W || start.y < 0 || start.y >= H ||
        goal.x < 0 || goal.x >= W || goal.y < 0 || goal.y >= H)
        return 1e9;

    dist[start.y][start.x] = 0;
    q.push(start);

    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};

    while (!q.empty()) {
        Point p = q.front(); q.pop();
        for (int i = 0; i < 4; ++i) {
            int nx = p.x + dx[i];
            int ny = p.y + dy[i];
            if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
            if (fineMap[ny][nx] == 1) continue;
            if (dist[ny][nx] != INT_MAX) continue;
            dist[ny][nx] = dist[p.y][p.x] + 1;
            q.push(Point(nx, ny));
        }
    }

    float bfs_dist = dist[goal.y][goal.x] != INT_MAX ? dist[goal.y][goal.x] : 1e9;


    float score = 0.0f;
    for (Bullet* b : bullets) {
        if (b->target == 0) {
            score += BulletThreatToPoint(b, ai->Position);
        }
    }
    std::cout << score << std::endl;
    return score/10;
}


Action MonteCarloAI::DecideBestAction(const State& current) {
    vector<Action> actions = GetAllActions();
    float minScore = std::numeric_limits<float>::max();
    Point best = Point(0, 0);
    for (const Action& act : actions) {
        float score = SimulateRollout(current, act);
        if (score < minScore) {
            minScore = score;
            best = act.direction;
        }
    }
    return best;
}

float MonteCarloAI::SimulateRollout(const State& initial, const Action& act) {
    int rolloutCount = 1;
    int rolloutSteps = 0;
    float totalScore = 0.0f;
    for (int i = 0; i < rolloutCount; ++i) {
        State current(initial);
        current.ApplyAction(act);
        current.Simulate(deltaTime);
        for (int j = 0; j < rolloutSteps; ++j) {
            std::vector<Action> actions = GetAllActions();
            Action randomAct = actions[rand() % actions.size()];
            current.ApplyAction(randomAct);
            current.Simulate(deltaTime);
        }
        totalScore += current.EvaluateScore();
    }
    return totalScore / rolloutCount;
}
std::vector<Action> MonteCarloAI::GetAllActions() {
    return AllActions;
}

