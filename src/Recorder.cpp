#include "Recorder.hpp"
#include "FileManager.hpp"
#include "utils/Utils.hpp"

Recorder* Recorder::get() {
    static Recorder instance;
    return &instance;
}

void Recorder::startRecording(PlayLayer* pl) {
    if (!pl) return;
    
    m_currentRecord.clear();
    m_currentFrame = 0;
    m_recording = true;
    m_paused = false;
    
    auto level = pl->m_level;
    m_currentRecord.meta.levelID = ghost_utils::getLevelID(level);
    m_currentRecord.meta.levelName = ghost_utils::getLevelName(level);
    m_currentRecord.meta.timestamp = ghost_utils::getCurrentTimestamp();
    m_currentRecord.meta.fps = 60.f; // TODO: detect actual FPS
    
    // Захватываем внешний вид
    if (pl->m_player1) {
        capturePlayerAppearance(pl->m_player1);
    }
    
    log::info("Started recording for level {} ({})", 
        m_currentRecord.meta.levelID, m_currentRecord.meta.levelName);
}

void Recorder::stopRecording(PlayLayer* pl) {
    if (!m_recording) return;
    
    m_recording = false;
    
    m_currentRecord.meta.totalFrames = m_currentFrame;
    
    if (pl) {
        m_currentRecord.meta.finalPercent = ghost_utils::getPlayerPercent(pl);
    }
    
    // Сохраняем если есть кадры
    if (!m_currentRecord.isEmpty()) {
        // Сохраняем лучшую запись за сессию
        if (m_currentRecord.meta.finalPercent > m_bestPercent) {
            m_bestPercent = m_currentRecord.meta.finalPercent;
            m_bestRecordThisSession = m_currentRecord;
        }
        
        // Всегда сохраняем текущую запись
        FileManager::get()->saveRecord(m_currentRecord);
    }
    
    log::info("Stopped recording: {} frames, {:.1f}%", 
        m_currentFrame, m_currentRecord.meta.finalPercent);
}

void Recorder::pauseRecording() {
    m_paused = true;
}

void Recorder::resumeRecording() {
    m_paused = false;
}

void Recorder::recordFrame(PlayLayer* pl) {
    if (!m_recording || m_paused || !pl) return;
    
    FrameData frame = captureFrame(pl);
    frame.frame = m_currentFrame;
    
    m_currentRecord.frames.push_back(frame);
    m_currentFrame++;
}

void Recorder::recordInput(bool isPress, bool isPlayer2, int button) {
    if (!m_recording || m_paused) return;
    
    InputAction action;
    action.frame = m_currentFrame;
    action.isPress = isPress;
    action.isPlayer2 = isPlayer2;
    action.button = button;
    
    m_currentRecord.inputs.push_back(action);
}

void Recorder::onPlayerDeath(PlayLayer* pl) {
    if (!m_recording || !pl) return;
    
    // Записываем кадр смерти
    FrameData deathFrame = captureFrame(pl);
    deathFrame.frame = m_currentFrame;
    deathFrame.isDead = true;
    m_currentRecord.frames.push_back(deathFrame);
    
    // Останавливаем и сохраняем
    m_currentRecord.meta.totalFrames = m_currentFrame;
    m_currentRecord.meta.finalPercent = ghost_utils::getPlayerPercent(pl);
    
    if (!m_currentRecord.isEmpty()) {
        if (m_currentRecord.meta.finalPercent > m_bestPercent) {
            m_bestPercent = m_currentRecord.meta.finalPercent;
            m_bestRecordThisSession = m_currentRecord;
        }
        FileManager::get()->saveRecord(m_currentRecord);
    }
    
    m_recording = false;
    log::info("Player died at {:.1f}%, saved recording", m_currentRecord.meta.finalPercent);
}

void Recorder::onLevelComplete(PlayLayer* pl) {
    if (!m_recording || !pl) return;
    
    m_currentRecord.meta.finalPercent = 100.f;
    m_currentRecord.meta.totalFrames = m_currentFrame;
    
    FileManager::get()->saveRecord(m_currentRecord);
    m_recording = false;
    
    log::info("Level completed! Saved recording with {} frames", m_currentFrame);
}

void Recorder::onResetLevel(PlayLayer* pl) {
    // Начинаем новую запись при рестарте
    bool shouldRecord = Mod::get()->getSettingValue<bool>("auto-record");
    if (shouldRecord) {
        startRecording(pl);
    }
}

FrameData Recorder::captureFrame(PlayLayer* pl) {
    FrameData frame{};
    
    auto player = pl->m_player1;
    if (!player) return frame;
    
    frame.posX = player->getPositionX();
    frame.posY = player->getPositionY();
    frame.rotation = player->getRotation();
    frame.yVelocity = player->m_yVelocity;
    frame.playerScale = player->getScale();
    frame.isOnGround = player->m_isOnGround;
    frame.isUpsideDown = player->m_isUpsideDown;
    frame.isDead = player->m_isDead;
    frame.isDashing = player->m_isDashing;
    frame.isVisible = player->isVisible();
    frame.isMini = player->m_vehicleSize != 1.0f; // приблизительно
    frame.isHolding = player->m_isHolding;
    
    // Определяем режим игры
    if (player->m_isShip) frame.gameMode = 1;
    else if (player->m_isBall) frame.gameMode = 2;
    else if (player->m_isBird) frame.gameMode = 3; // UFO
    else if (player->m_isDart) frame.gameMode = 4; // Wave
    else if (player->m_isRobot) frame.gameMode = 5;
    else if (player->m_isSpider) frame.gameMode = 6;
    else if (player->m_isSwing) frame.gameMode = 7;
    else frame.gameMode = 0; // Cube
    
    return frame;
}

void Recorder::capturePlayerAppearance(PlayerObject* player) {
    if (!player) return;
    
    auto gm = GameManager::sharedState();
    if (!gm) return;
    
    m_currentRecord.meta.iconID = gm->getPlayerFrame();
    m_currentRecord.meta.color1 = gm->getPlayerColor();
    m_currentRecord.meta.color2 = gm->getPlayerColor2();
    m_currentRecord.meta.glowColor = gm->getPlayerGlowColor();
    m_currentRecord.meta.hasGlow = gm->getPlayerGlow();
    m_currentRecord.meta.shipID = gm->getPlayerShip();
    m_currentRecord.meta.ballID = gm->getPlayerBall();
    m_currentRecord.meta.ufoID = gm->getPlayerBird();
    m_currentRecord.meta.waveID = gm->getPlayerDart();
    m_currentRecord.meta.robotID = gm->getPlayerRobot();
    m_currentRecord.meta.spiderID = gm->getPlayerSpider();
    m_currentRecord.meta.swingID = gm->getPlayerSwing();
    m_currentRecord.meta.deathEffect = gm->getPlayerDeathEffect();
}
