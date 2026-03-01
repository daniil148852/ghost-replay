#pragma once

#include "RecordData.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class GhostPlayer : public CCNode {
public:
    static GhostPlayer* create(const RecordData& record, PlayLayer* playLayer);
    
    bool init(const RecordData& record, PlayLayer* playLayer);
    
    // Обновление позиции каждый кадр
    void updateGhost(uint32_t currentFrame);
    
    // Управление
    void show();
    void hide();
    void reset();
    bool isFinished() const { return m_finished; }
    bool isDead() const { return m_dead; }
    
    // Данные
    const RecordData& getRecord() const { return m_record; }
    std::string getFilePath() const { return m_filePath; }
    void setFilePath(const std::string& path) { m_filePath = path; }
    
private:
    RecordData m_record;
    std::string m_filePath;
    
    // Визуальное представление
    CCSprite* m_mainSprite = nullptr;
    CCSprite* m_secondarySprite = nullptr;
    CCSprite* m_glowSprite = nullptr;
    SimplePlayer* m_simplePlayer = nullptr;
    
    // Состояние
    uint32_t m_currentFrame = 0;
    bool m_finished = false;
    bool m_dead = false;
    bool m_visible = true;
    uint8_t m_lastGameMode = 255;
    float m_opacity = 120.f;
    
    PlayLayer* m_playLayer = nullptr;
    
    // Обновление визуала при смене режима
    void updateGameMode(uint8_t gameMode);
    void applyFrameData(const FrameData& frame);
    void setupVisuals();
    void updateColors();
};
