#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include "../Recorder.hpp"
#include "../GhostManager.hpp"
#include "../GhostPlayer.hpp"

using namespace geode::prelude;

class $modify(GhostPlayerObject, PlayerObject) {

    // Проверка: является ли этот PlayerObject призраком
    bool isGhostInstance() {
        return GhostPlayer::isGhost(this);
    }
    
    // Проверка: является ли этот объект основным player1
    bool isMainPlayer() {
        auto pl = PlayLayer::get();
        return pl && this == pl->m_player1;
    }

    void update(float dt) {
        if (isGhostInstance()) return; // Призраки не обновляются через движок
        PlayerObject::update(dt);
    }
    
    void toggleFlyMode(bool flying, bool playEffects) {
        if (isGhostInstance()) return;
        PlayerObject::toggleFlyMode(flying, playEffects);
        
        if (isMainPlayer()) {
            Recorder::get()->recordInput(flying, false, 100);
        }
    }
    
    void toggleBirdMode(bool bird, bool playEffects) {
        if (isGhostInstance()) return;
        PlayerObject::toggleBirdMode(bird, playEffects);
        
        if (isMainPlayer()) {
            Recorder::get()->recordInput(bird, false, 101);
        }
    }
    
    void toggleDartMode(bool dart, bool playEffects) {
        if (isGhostInstance()) return;
        PlayerObject::toggleDartMode(dart, playEffects);
        
        if (isMainPlayer()) {
            Recorder::get()->recordInput(dart, false, 102);
        }
    }
    
    void toggleRollMode(bool ball, bool playEffects) {
        if (isGhostInstance()) return;
        PlayerObject::toggleRollMode(ball, playEffects);
        
        if (isMainPlayer()) {
            Recorder::get()->recordInput(ball, false, 103);
        }
    }
    
    void toggleRobotMode(bool robot, bool playEffects) {
        if (isGhostInstance()) return;
        PlayerObject::toggleRobotMode(robot, playEffects);
        
        if (isMainPlayer()) {
            Recorder::get()->recordInput(robot, false, 104);
        }
    }
    
    void toggleSpiderMode(bool spider, bool playEffects) {
        if (isGhostInstance()) return;
        PlayerObject::toggleSpiderMode(spider, playEffects);
        
        if (isMainPlayer()) {
            Recorder::get()->recordInput(spider, false, 105);
        }
    }
    
    void toggleSwingMode(bool swing, bool playEffects) {
        if (isGhostInstance()) return;
        PlayerObject::toggleSwingMode(swing, playEffects);
        
        if (isMainPlayer()) {
            Recorder::get()->recordInput(swing, false, 106);
        }
    }
    
    void flipGravity(bool flip, bool playEffects) {
        if (isGhostInstance()) return;
        PlayerObject::flipGravity(flip, playEffects);
        
        if (isMainPlayer()) {
            Recorder::get()->recordInput(flip, false, 200);
        }
    }
    
    void togglePlayerScale(bool mini, bool playEffects) {
        if (isGhostInstance()) return;
        PlayerObject::togglePlayerScale(mini, playEffects);
        
        if (isMainPlayer()) {
            Recorder::get()->recordInput(mini, false, 201);
        }
    }
    
    void playerDestroyed(bool explode) {
        if (isGhostInstance()) {
            // Призрак — только скрываем, без эффектов смерти
            this->setVisible(false);
            return;
        }
        PlayerObject::playerDestroyed(explode);
    }
    
    void resetObject() {
        if (isGhostInstance()) return;
        PlayerObject::resetObject();
    }
    
    // Коллизии — призраки полностью игнорируют
    void collidedWithObject(float dt, GameObject* obj, CCRect rect, bool idk) {
        if (isGhostInstance()) return;
        PlayerObject::collidedWithObject(dt, obj, rect, idk);
    }
    
    void checkSnapJumpToObject(GameObject* obj) {
        if (isGhostInstance()) return;
        PlayerObject::checkSnapJumpToObject(obj);
    }
    
    // Триггеры — призраки не активируют
    void activateObject(GameObject* obj) {
        if (isGhostInstance()) return;
        PlayerObject::activateObject(obj);
    }
    
    // Трейлы
    void setupStreak() {
        if (isGhostInstance()) return;
        PlayerObject::setupStreak();
    }
    
    // Орбы
    void ringJump(RingObject* ring, bool push) {
        if (isGhostInstance()) return;
        PlayerObject::ringJump(ring, push);
        
        if (isMainPlayer()) {
            Recorder::get()->recordInput(push, false, 400);
        }
    }
    
    // Пады
    void activateJumpPad(PadObject* pad, bool push) {
        if (isGhostInstance()) return;
        PlayerObject::activateJumpPad(pad, push);
    }
    
    // Дэш
    void startDashing(DashRingObject* ring) {
        if (isGhostInstance()) return;
        PlayerObject::startDashing(ring);
        
        if (isMainPlayer()) {
            Recorder::get()->recordInput(true, false, 300);
        }
    }
    
    void stopDashing() {
        if (isGhostInstance()) return;
        PlayerObject::stopDashing();
        
        if (isMainPlayer()) {
            Recorder::get()->recordInput(false, false, 300);
        }
    }
    
    // Предотвращаем звуки и эффекты от призраков
    void playDeathEffect() {
        if (isGhostInstance()) return;
        PlayerObject::playDeathEffect();
    }
    
    void playSpawnEffect() {
        if (isGhostInstance()) return;
        PlayerObject::playSpawnEffect();
    }
    
    // Телепорты — призраки не телепортируются через порталы
    void portalTeleport(TeleportPortalObject* portal) {
        if (isGhostInstance()) return;
        PlayerObject::portalTeleport(portal);
    }
};
