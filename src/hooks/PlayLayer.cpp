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
        CCMenuItemSpriteExtra* ghostBtn = nullptr;
    };
    
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        
        auto fields = m_fields.self();
        fields->frameCounter = 0;
        fields->initialized = true;
        
        // Инициализируем менеджер призраков
        GhostManager::get()->onEnterLevel(this);
        
        // Запускаем запись если включена авто-запись
        if (Mod::get()->getSettingValue<bool>("auto-record")) {
            Recorder::get()->startRecording(this);
        }
        
        // Добавляем кнопку призрака в pause menu UI
        addGhostButton();
        
        return true;
    }
    
    void addGhostButton() {
        // Создаём кнопку в правом верхнем углу
        auto menu = CCMenu::create();
        menu->setPosition(CCPointZero);
        menu->setID("ghost-menu"_spr);
        
        auto spr = CCSprite::createWithSpriteFrameName("GJ_profileButton_001.png");
        spr->setScale(0.6f);
        // Делаем спрайт полупрозрачным для неинвазивности
        spr->setOpacity(180);
        
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(GhostPlayLayer::onGhostButton)
        );
        
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        btn->setPosition(ccp(winSize.width - 30.f, winSize.height - 30.f));
        
        menu->addChild(btn);
        menu->setZOrder(1000);
        this->addChild(menu);
        
        m_fields->ghostBtn = btn;
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
        
        // Записываем кадр
        Recorder::get()->recordFrame(this);
        
        // Обновляем призраков
        GhostManager::get()->update(m_fields->frameCounter);
        
        m_fields->frameCounter++;
    }
    
    void resetLevel() {
        PlayLayer::resetLevel();
        
        m_fields->frameCounter = 0;
        
        // Перезапускаем запись
        Recorder::get()->onResetLevel(this);
        
        // Сбрасываем призраков
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
    
    // Хук нажатий для записи инпутов
    void pushButton(int button, bool isPlayer2) {
        Recorder::get()->recordInput(true, isPlayer2, button);
        PlayLayer::pushButton(button, isPlayer2);
    }
    
    void releaseButton(int button, bool isPlayer2) {
        Recorder::get()->recordInput(false, isPlayer2, button);
        PlayLayer::releaseButton(button, isPlayer2);
    }
};
