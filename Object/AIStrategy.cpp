#include "Object/AIStrategy.hpp"
#include "Object/Action.hpp"
#include <Scene/PlayScene.hpp>
#include <vector>
#include <queue>
#include <iostream>
#include <chrono>
#include <ctime>
#include "Object/Bullet.hpp"

// // const Engine::Point DirectionVectors[];
// // const Engine::Point actionSet[];
// using namespace std;

State::State(PlayScene* scene) {
    mapState = scene->mapState;
    if(scene->aiTank) ai = new AITank(*scene->aiTank);
    if(scene->playerTank) player = new Tank(*scene->playerTank);
    BlockSize = scene->BlockSize;
    MapHeight = scene->MapHeight;
    MapWidth = scene->MapWidth;
    hitCount = 0;
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
    hitCount = state.hitCount;
    bullets.clear();
    for (Bullet* b : state.bullets) {
        if (b) {
            bullets.push_back(new Bullet(*b));  // 深拷貝 bullet
        }
    }
}


void State::ApplyAction(const Action& act){
    ai->ApplyAction(act);
}
void State::Simulate(float deltaTime){
    const float fixedStep = 0.01f;
    float elapsed = 0.0f;
    while (elapsed + fixedStep <= deltaTime) {
        Step(fixedStep);
        elapsed += fixedStep;
    }
    // 最後補上不足的時間（如果有）
    if (elapsed < deltaTime) {
        Step(deltaTime - elapsed);
    }
}

bool State::CheckShoot() const {
    Point from = ai->Position;
    Point to = player->Position;
    float dx = to.x - from.x;
    float dy = to.y - from.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist == 0) return true;

    float step = BlockSize / 4.0f; // 每步距離，例如每 4 分之 1 格檢查一次
    int steps = static_cast<int>(dist / step);
    float vx = dx / dist * step;
    float vy = dy / dist * step;

    for (int i = 0; i <= steps; ++i) {
        float cx = from.x + vx * i;
        float cy = from.y + vy * i;
        int gridX = static_cast<int>(cx / BlockSize);
        int gridY = static_cast<int>(cy / BlockSize);
        if (gridX < 0 || gridY < 0 || gridX >= MapWidth || gridY >= MapHeight)
            return false;
        if (mapState[gridY][gridX] == PlayScene::TILE_WALL)
            return false;
    }

    return true;
}

void State::Step(float dt){
    ai->PropertyChange(dt);
    vector<Bullet*> toDestroy;
    for (Bullet* blt: bullets){
        vector<int> result = blt->SimulateUpdate(dt);
        // result: wall, hit ai, hit player
        if (!blt->simDestroy) continue;
        // hit target or wall
        toDestroy.push_back(blt);
        if (result[1]) { // hit ai
            ++hitCount;
        }
    }
    for (Bullet* b : toDestroy) {
        bullets.erase(std::remove(bullets.begin(), bullets.end(), b), bullets.end());
        delete b;  // 若 bullet 是 new 出來的
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

// float State::DFSdodge() const {
//     Engine::Point diff = ai->Position - player->Position;
//     // if (diff.Magnitude() > 30) return 0;  // 無威脅，不需 dodge
//     vector<Point> cands;
//     int dx[4] = {0,0,1,-1};
//     int dy[4] = {1,-1,0,0};
//     Point cur(ai->Position.x / BlockSize, ai->Position.y / BlockSize);
//     for (int i = 0; i < 4; i++) {
//         Point offset(dx[i], dy[i]);
//         float dot = diff.Dot(Engine::Point(offset.x, offset.y));
//         if (dot < 0) {
//             Point next = cur + offset;
//             if (next.x < 0 || next.x >= MapWidth || next.y < 0 || next.y >= MapHeight)
//                 continue;
//             if (mapState[next.y][next.x] != PlayScene::TILE_WALL) {
//                 cands.push_back(offset);  // 可走方向
//             }
//         }
//     }
//     std::cout << cands.size() << std::endl;
//     return 0;
// }

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
    if (ai->aggressive){
        return score + abs(bfs_dist-1) + hitCount*1e5;
    }else{
        return score + abs(bfs_dist-15) + hitCount*1e5;
    }
}


// Action MonteCarloAI::DecideBestAction(const State& current) {
//     vector<Action> actions = GetAllActions();
//     float minScore = std::numeric_limits<float>::max();
//     Point best = Point(0, 0);
//     for (const Action& act : actions) {
//         float score = SimulateRollout(current, act);
//         if (score < minScore) {
//             minScore = score;
//             best = act.direction;
//         }
//     }
//     return best;
// }

Action MonteCarloAI::DecideBestAction(const State& current) {
    vector<Action> actions = GetAllActions();
    float bestScore = std::numeric_limits<float>::max();
    Action bestAction = actions[0];
    vector<Action> path;
    int depth = 1;  // 可自由更改模擬深度
    DFSPathSearch(current, path, depth, bestScore, bestAction);
    if (current.CheckShoot()){
        bestAction.shoot = true;
    }
    return bestAction;
}
void MonteCarloAI::DFSPathSearch(const State& current, vector<Action>& path,
                                 int depth, float& bestScore, Action& bestAction) {
    if (path.size() == depth) {
        // 做一整段 rollout（使用你原本的函數）
        State simState(current);
        for (const Action& act : path) {
            simState.ApplyAction(act);
            simState.Simulate(deltaTime);
        }
        float score = simState.EvaluateScore();
        if (score < bestScore) {
            bestScore = score;
            bestAction = path[0];  // 記住第一步
        }
        return;
    }

    for (const Action& act : GetAllActions()) {
        path.push_back(act);
        DFSPathSearch(current, path, depth, bestScore, bestAction);
        path.pop_back();
    }
}

// float MonteCarloAI::SimulateRollout(const State& initial, const Action& act) {
//     int rolloutCount = 1;
//     int rolloutSteps = 0;
//     float totalScore = 0.0f;
//     for (int i = 0; i < rolloutCount; ++i) {
//         State current(initial);
//         current.ApplyAction(act);
//         current.Simulate(deltaTime);
//         for (int j = 0; j < rolloutSteps; ++j) {
//             std::vector<Action> actions = GetAllActions();
//             Action randomAct = actions[rand() % actions.size()];
//             current.ApplyAction(randomAct);
//             current.Simulate(deltaTime);
//         }
//         totalScore += current.EvaluateScore();
//     }
//     return totalScore / rolloutCount;
// }
std::vector<Action> MonteCarloAI::GetAllActions() {
    return AllActions;
}

