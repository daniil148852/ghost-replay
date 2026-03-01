#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
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
        
        // Инициализируем менеджер призраков
        GhostManager::get()->onEnterLevel(this);
        
        // Запускаем запись если включена авто-запись
        if (Mod::get()->getSettingValue<bool>("auto-record")) {
            Recorder::get()->startRecording(this);
        }
        
        // Добавляем кнопку призрака в UI
        addGhostButton();
        
        return true;
    }
    
    void addGhostButton() {
        // Создаём кнопку в меню
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
        // Позиция в правом верхнем углу
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
        
        // Записываем кадр (позиция, вращение и т.д.)
        Recorder::get()->recordFrame(this);
        
        // Обновляем анимацию призраков
        GhostManager::get()->update(m_fields->frameCounter);
        
        m_fields->frameCounter++;
    }
    
    void resetLevel() {
        PlayLayer::resetLevel();
        
        m_fields->frameCounter = 0;
        
        // Перезапускаем запись
        Recorder::get()->onResetLevel(this);
        
        // Сбрасываем призраков в начало
        GhostManager::get()->reset();
    }
    
    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        // Записываем смерть до вызова оригинала
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
        // Сохраняем запись при выходе
        if (Recorder::get()->isRecording()) {
            Recorder::get()->stopRecording(this);
        }
        
        GhostManager::get()->onExitLevel();
        
        PlayLayer::onQuit();
    }
};
