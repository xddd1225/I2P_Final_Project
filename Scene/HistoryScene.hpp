#ifndef HISTORY_SCENE_HPP
#define HISTORY_SCENE_HPP

#include "Engine/IScene.hpp"
#include <vector>
#include <string>

class HistoryScene final : public Engine::IScene {
public:
    void Initialize() override;
    void Terminate() override;
    void BackOnClick();
    void NextPageOnClick();
    void PrevPageOnClick();

private:
    std::vector<std::string> historyEntries;
    void LoadHistory();
    int curPage = 0;
    const int recordsPerPage = 12;
};

#endif // HISTORY_SCENE_HPP
