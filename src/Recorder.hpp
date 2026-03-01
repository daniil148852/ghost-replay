#pragma once

#include "RecordData.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class Recorder {
public:
    static Recorder* get();
    
    // Управление записью
    void startRecording(PlayLayer* pl);
    void stopRecording(PlayLayer* pl);
    void pauseRecording();
    void resumeRecording();
    
    // Вызывается каждый кадр из PlayLayer::update
    void recordFrame(PlayLayer* pl);
    
    // Вызывается при нажатиях
    void recordInput(bool isPress, bool isPlayer2, int button);
    
    // Вызывается при смерти
    void onPlayerDeath(PlayLayer* pl);
    
    // Вызывается при завершении уровня
    void onLevelComplete(PlayLayer* pl);
    
    // Вызывается при рестарте
    void onResetLevel(PlayLayer* pl);
    
    // Состояние
    bool isRecording() const { return m_recording; }
    bool isPaused() const { return m_paused; }
    uint32_t getCurrentFrame() const { return m_currentFrame; }
    
    // Захват внешнего вида игрока
    void capturePlayerAppearance(PlayerObject* player);
    
    // Текущая запись (для сохранения)
    RecordData& getCurrentRecord() { return m_currentRecord; }
    
private:
    Recorder() = default;
    
    bool m_recording = false;
    bool m_paused = false;
    uint32_t m_currentFrame = 0;
    RecordData m_currentRecord;
    RecordData m_bestRecordThisSession; // лучшая запись за сессию
    float m_bestPercent = 0.f;
    
    // Кэш FPS
    float m_targetFPS = 60.f;
    
    FrameData captureFrame(PlayLayer* pl);
};
