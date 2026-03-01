#pragma once

#include "GhostPlayer.hpp"
#include "RecordData.hpp"
#include <Geode/Geode.hpp>
#include <vector>
#include <memory>

using namespace geode::prelude;

class GhostManager {
public:
    static GhostManager* get();
    
    // Инициализация при входе в уровень
    void onEnterLevel(PlayLayer* pl);
    
    // Очистка при выходе из уровня
    void onExitLevel();
    
    // Обновление каждый кадр
    void update(uint32_t currentFrame);
    
    // Сброс при рестарте
    void reset();
    
    // Добавить/удалить призрака
    void addGhost(const std::string& recordPath);
    void removeGhost(const std::string& recordPath);
    void removeAllGhosts();
    
    // Загрузить включённых призраков для уровня
    void loadEnabledGhosts(int levelID);
    
    // Информация
    int getGhostCount() const { return static_cast<int>(m_ghosts.size()); }
    bool hasGhosts() const { return !m_ghosts.empty(); }
    
    // Текущий уровень
    int getCurrentLevelID() const { return m_currentLevelID; }
    PlayLayer* getPlayLayer() const { return m_playLayer; }
    
private:
    GhostManager() = default;
    
    PlayLayer* m_playLayer = nullptr;
    int m_currentLevelID = 0;
    
    // Слой для призраков (добавляется в PlayLayer)
    CCNode* m_ghostLayer = nullptr;
    
    struct GhostEntry {
        GhostPlayer* ghost = nullptr;
        std::string filePath;
    };
    
    std::vector<GhostEntry> m_ghosts;
    
    void createGhostLayer();
};
