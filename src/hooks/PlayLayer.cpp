#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include "../Recorder.hpp"
#include "../GhostManager.hpp"
#include "../GhostPopup.hpp"
#include "../FileManager.hpp"
#include "../utils/Utils.hpp"

using namespace geode::prelude;

class $modify(GhostPlayLayer, PlayLayer) {
    struct Fields {
        uint32_t frameCounter = 0;
        bool initialized = false;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        m_fields->frameCounter = 0;
        m_fields->initialized = true;

        GhostManager::get()->onEnterLevel(this);

        if (Mod::get()->getSettingValue<bool>("auto-record")) {
            Recorder::get()->startRecording(this);
        }

        addGhostButton();

        return true;
    }

    void addGhostButton() {
        auto menu = CCMenu::create();
        menu->setPosition(CCPointZero);
        menu->setID("ghost-menu"_spr);

        auto spr = CCSprite::createWithSpriteFrameName("GJ_profileButton_001.png");
        spr->setScale(0.6f);
        spr->setOpacity(180);

        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(GhostPlayLayer::onGhostButton)
        );

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        btn->setPosition(ccp(winSize.width - 30.f, winSize.height - 30.f));

        menu->addChild(btn);
        menu->setZOrder(1000);
        this->addChild(menu);
    }

    void onGhostButton(CCObject* sender) {
        int levelID = 0;
        if (m_level) {
            levelID = ghost_utils::getLevelID(m_level);
        }
        auto popup = GhostPopup::create(levelID);
        if (popup) {
            popup->show();
        }
    }

    void update(float dt) {
        PlayLayer::update(dt);

        if (!m_fields->initialized) return;

        Recorder::get()->recordFrame(this);
        GhostManager::get()->update(m_fields->frameCounter);

        m_fields->frameCounter++;
    }

    void resetLevel() {
        PlayLayer::resetLevel();

        m_fields->frameCounter = 0;

        Recorder::get()->onResetLevel(this);
        GhostManager::get()->reset();
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        if (player == m_player1) {
            Recorder::get()->onPlayerDeath(this);
        }
        PlayLayer::destroyPlayer(player, obj);
    }

    void levelComplete() {
        Recorder::get()->onLevelComplete(this);
        PlayLayer::levelComplete();
    }

    void onQuit() {
        if (Recorder::get()->isRecording()) {
            Recorder::get()->stopRecording(this);
        }
        GhostManager::get()->onExitLevel();
        PlayLayer::onQuit();
    }
};

// FIX: pushButton и releaseButton принадлежат GJBaseGameLayer, не PlayLayer
class $modify(GhostBaseGameLayer, GJBaseGameLayer) {

    void pushButton(int button, bool isPlayer2) {
        Recorder::get()->recordInput(true, isPlayer2, button);
        GJBaseGameLayer::pushButton(button, isPlayer2);
    }

    void releaseButton(int button, bool isPlayer2) {
        Recorder::get()->recordInput(false, isPlayer2, button);
        GJBaseGameLayer::releaseButton(button, isPlayer2);
    }
};
