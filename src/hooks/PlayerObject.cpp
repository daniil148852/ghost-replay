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
    
    // Проверка: является ли этот объект основным игроком, которого контролирует человек
    bool isMainPlayer() {
        auto pl = PlayLayer::get();
        if (!pl) return false;
        // Это либо игрок 1, либо игрок 2 (в дуале)
        return (this == pl->m_player1 || this == pl->m_player2);
    }

    // === ПЕРЕХВАТ НАЖАТИЙ (НОВАЯ ЛОГИКА) ===
    
    void pushButton(int button) {
        if (isGhostInstance()) return; // Призраки не нажимают кнопки физически
        
        PlayerObject::pushButton(button);
        
        // Записываем нажатие, если это реальный игрок
        if (isMainPlayer()) {
            bool isP2 = false;
            if (auto pl = PlayLayer::get()) {
                if (this == pl->m_player2) isP2 = true;
            }
            Recorder::get()->recordInput(true, isP2, button);
        }
    }

    void releaseButton(int button) {
        if (isGhostInstance()) return;
        
        PlayerObject::releaseButton(button);
        
        if (isMainPlayer()) {
            bool isP2 = false;
            if (auto pl = PlayLayer::get()) {
                if (this == pl->m_player2) isP2 = true;
            }
            Recorder::get()->recordInput(false, isP2, button);
        }
    }

    // === ОСТАЛЬНЫЕ ХУКИ ДЛЯ СОСТОЯНИЙ И ИЗОЛЯЦИИ ПРИЗРАКОВ ===

    void update(float dt) {
        if (isGhostInstance()) return; // Призраки обновляются вручную через GhostManager
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
    
    // Призрак просто исчезает, не вызывая краш игры или эффекты смерти
    void playerDestroyed(bool explode) {
        if (isGhostInstance()) {
            this->setVisible(false);
            return;
        }
        PlayerObject::playerDestroyed(explode);
    }
    
    // Изоляция физики призрака
    void resetObject() {
        if (isGhostInstance()) return;
        PlayerObject::resetObject();
    }
    
    void collidedWithObject(float dt, GameObject* obj, CCRect rect, bool idk) {
        if (isGhostInstance()) return;
        PlayerObject::collidedWithObject(dt, obj, rect, idk);
    }
    
    void checkSnapJumpToObject(GameObject* obj) {
        if (isGhostInstance()) return;
        PlayerObject::checkSnapJumpToObject(obj);
    }
    
    void activateObject(GameObject* obj) {
        if (isGhostInstance()) return;
        PlayerObject::activateObject(obj);
    }
    
    void setupStreak() {
        if (isGhostInstance()) return;
        PlayerObject::setupStreak();
    }
    
    void ringJump(RingObject* ring, bool push) {
        if (isGhostInstance()) return;
        PlayerObject::ringJump(ring, push);
        
        if (isMainPlayer()) {
            Recorder::get()->recordInput(push, false, 400);
        }
    }
    
    void activateJumpPad(PadObject* pad, bool push) {
        if (isGhostInstance()) return;
        PlayerObject::activateJumpPad(pad, push);
    }
    
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
    
    void playDeathEffect() {
        if (isGhostInstance()) return;
        PlayerObject::playDeathEffect();
    }
    
    void playSpawnEffect() {
        if (isGhostInstance()) return;
        PlayerObject::playSpawnEffect();
    }
    
    void portalTeleport(TeleportPortalObject* portal) {
        if (isGhostInstance()) return;
        PlayerObject::portalTeleport(portal);
    }
};
