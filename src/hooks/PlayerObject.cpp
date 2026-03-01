#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include "../Recorder.hpp"
#include "../GhostManager.hpp"
#include "../GhostPlayer.hpp"

using namespace geode::prelude;

class $modify(GhostPlayerObject, PlayerObject) {

    bool isGhostInstance() {
        return GhostPlayer::isGhost(this);
    }

    bool isMainPlayer() {
        auto pl = PlayLayer::get();
        return pl && this == pl->m_player1;
    }

    void update(float dt) {
        if (isGhostInstance()) return;
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
            this->setVisible(false);
            return;
        }
        PlayerObject::playerDestroyed(explode);
    }
};
