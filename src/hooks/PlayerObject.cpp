#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include "../Recorder.hpp"
#include "../GhostManager.hpp"

using namespace geode::prelude;

class $modify(GhostPlayerObject, PlayerObject) {
    
    struct Fields {
        bool isGhostPlayer = false;
    };

    // Хук на обновление игрока — захватываем дополнительные данные
    void update(float dt) {
        PlayerObject::update(dt);
        
        // Не обрабатываем призраков
        if (m_fields->isGhostPlayer) return;
        
        // Проверяем что это основной игрок в PlayLayer
        auto playLayer = PlayLayer::get();
        if (!playLayer) return;
        if (this != playLayer->m_player1) return;
        
        // Дополнительная синхронизация данных записи
        // (основная запись идёт через PlayLayer::update, 
        //  здесь ловим промежуточные состояния)
    }
    
    // Хук на смену формы — важно для корректного отображения призрака
    void toggleFlyMode(bool flying, bool playEffects) {
        PlayerObject::toggleFlyMode(flying, playEffects);
        
        if (m_fields->isGhostPlayer) return;
        
        auto playLayer = PlayLayer::get();
        if (!playLayer || this != playLayer->m_player1) return;
        
        // Записываем смену режима как специальный инпут
        Recorder::get()->recordInput(flying, false, 100); // 100 = flyMode marker
    }
    
    void toggleBirdMode(bool bird, bool playEffects) {
        PlayerObject::toggleBirdMode(bird, playEffects);
        
        if (m_fields->isGhostPlayer) return;
        
        auto playLayer = PlayLayer::get();
        if (!playLayer || this != playLayer->m_player1) return;
        
        Recorder::get()->recordInput(bird, false, 101); // 101 = birdMode (UFO)
    }
    
    void toggleDartMode(bool dart, bool playEffects) {
        PlayerObject::toggleDartMode(dart, playEffects);
        
        if (m_fields->isGhostPlayer) return;
        
        auto playLayer = PlayLayer::get();
        if (!playLayer || this != playLayer->m_player1) return;
        
        Recorder::get()->recordInput(dart, false, 102); // 102 = dartMode (Wave)
    }
    
    void toggleRollMode(bool ball, bool playEffects) {
        PlayerObject::toggleRollMode(ball, playEffects);
        
        if (m_fields->isGhostPlayer) return;
        
        auto playLayer = PlayLayer::get();
        if (!playLayer || this != playLayer->m_player1) return;
        
        Recorder::get()->recordInput(ball, false, 103); // 103 = rollMode (Ball)
    }
    
    void toggleRobotMode(bool robot, bool playEffects) {
        PlayerObject::toggleRobotMode(robot, playEffects);
        
        if (m_fields->isGhostPlayer) return;
        
        auto playLayer = PlayLayer::get();
        if (!playLayer || this != playLayer->m_player1) return;
        
        Recorder::get()->recordInput(robot, false, 104); // 104 = robotMode
    }
    
    void toggleSpiderMode(bool spider, bool playEffects) {
        PlayerObject::toggleSpiderMode(spider, playEffects);
        
        if (m_fields->isGhostPlayer) return;
        
        auto playLayer = PlayLayer::get();
        if (!playLayer || this != playLayer->m_player1) return;
        
        Recorder::get()->recordInput(spider, false, 105); // 105 = spiderMode
    }
    
    void toggleSwingMode(bool swing, bool playEffects) {
        PlayerObject::toggleSwingMode(swing, playEffects);
        
        if (m_fields->isGhostPlayer) return;
        
        auto playLayer = PlayLayer::get();
        if (!playLayer || this != playLayer->m_player1) return;
        
        Recorder::get()->recordInput(swing, false, 106); // 106 = swingMode
    }
    
    // Хук на изменение гравитации
    void flipGravity(bool flipGravity, bool playEffects) {
        PlayerObject::flipGravity(flipGravity, playEffects);
        
        if (m_fields->isGhostPlayer) return;
        
        auto playLayer = PlayLayer::get();
        if (!playLayer || this != playLayer->m_player1) return;
        
        Recorder::get()->recordInput(flipGravity, false, 200); // 200 = gravity flip
    }
    
    // Хук на изменение размера (мини портал)
    void togglePlayerScale(bool mini, bool playEffects) {
        PlayerObject::togglePlayerScale(mini, playEffects);
        
        if (m_fields->isGhostPlayer) return;
        
        auto playLayer = PlayLayer::get();
        if (!playLayer || this != playLayer->m_player1) return;
        
        Recorder::get()->recordInput(mini, false, 201); // 201 = scale change
    }
    
    // Хук на смерть игрока
    void playerDestroyed(bool explode) {
        if (m_fields->isGhostPlayer) {
            // Призрак не должен запускать эффекты смерти
            // Просто скрываем
            this->setVisible(false);
            return;
        }
        
        PlayerObject::playerDestroyed(explode);
    }
    
    // Хук на респаун (checkpoint)
    void resetObject() {
        PlayerObject::resetObject();
        
        if (m_fields->isGhostPlayer) return;
        
        // Не нужно специально обрабатывать — 
        // PlayLayer::resetLevel уже вызывает Recorder::onResetLevel
    }
    
    // Предотвращаем взаимодействие призрака с триггерами и объектами
    void collidedWithObject(float dt, GameObject* obj, CCRect rect, bool idk) {
        // Призраки не коллайдятся
        if (m_fields->isGhostPlayer) return;
        
        PlayerObject::collidedWithObject(dt, obj, rect, idk);
    }
    
    // Предотвращаем столкновения призрака с землёй/потолком
    void checkSnapJumpToObject(GameObject* obj) {
        if (m_fields->isGhostPlayer) return;
        
        PlayerObject::checkSnapJumpToObject(obj);
    }
    
    // Хук на эффект дэша
    void startDashing(DashRingObject* ring) {
        if (m_fields->isGhostPlayer) return;
        
        PlayerObject::startDashing(ring);
        
        auto playLayer = PlayLayer::get();
        if (!playLayer || this != playLayer->m_player1) return;
        
        Recorder::get()->recordInput(true, false, 300); // 300 = dash start
    }
    
    void stopDashing() {
        if (m_fields->isGhostPlayer) return;
        
        PlayerObject::stopDashing();
        
        auto playLayer = PlayLayer::get();
        if (!playLayer || this != playLayer->m_player1) return;
        
        Recorder::get()->recordInput(false, false, 300); // 300 = dash stop
    }
    
    // Важно: предотвращаем активацию триггеров призраком
    void activateObject(GameObject* obj) {
        if (m_fields->isGhostPlayer) return;
        
        PlayerObject::activateObject(obj);
    }
    
    // Хук на создание трейла (частицы)
    void setupStreak() {
        // Призраки не создают трейлы (опционально)
        if (m_fields->isGhostPlayer) return;
        
        PlayerObject::setupStreak();
    }
    
    // Ring (орб) активация
    void ringJump(RingObject* ring, bool push) {
        if (m_fields->isGhostPlayer) return;
        
        PlayerObject::ringJump(ring, push);
        
        auto playLayer = PlayLayer::get();
        if (!playLayer || this != playLayer->m_player1) return;
        
        Recorder::get()->recordInput(push, false, 400); // 400 = ring
    }
    
    // Pad активация
    void activateJumpPad(PadObject* pad, bool push) {
        if (m_fields->isGhostPlayer) return;
        
        PlayerObject::activateJumpPad(pad, push);
    }
};
