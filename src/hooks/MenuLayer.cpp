#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "../GhostPopup.hpp"
#include "../FileManager.hpp"

using namespace geode::prelude;

class $modify(GhostMenuLayer, MenuLayer) {
    
    bool init() {
        if (!MenuLayer::init()) return false;
        
        // Инициализируем файловый менеджер
        FileManager::get()->init();
        
        // Добавляем кнопку Ghost в главное меню
        auto menu = this->getChildByID("bottom-menu");
        if (!menu) {
            // Создаём своё меню если bottom-menu не найдено
            menu = CCMenu::create();
            menu->setID("ghost-bottom-menu"_spr);
            this->addChild(menu);
        }
        
        // Создаём кнопку
        auto ghostSpr = CircleButtonSprite::createWithSpriteFrameName(
            "GJ_profileButton_001.png", 1.0f,
            CircleBaseColor::Green, CircleBaseSize::Medium
        );
        
        if (!ghostSpr) {
            // Fallback
            ghostSpr = CircleButtonSprite::createWithSpriteFrameName(
                "gj_profileButton_001.png", 1.0f,
                CircleBaseColor::Green, CircleBaseSize::Medium
            );
        }
        
        CCNode* btnSprite = ghostSpr;
        if (!btnSprite) {
            // Ещё один fallback — просто текст
            btnSprite = ButtonSprite::create("Ghost", "goldFont.fnt", "GJ_button_01.png");
            static_cast<ButtonSprite*>(btnSprite)->setScale(0.7f);
        }
        
        auto ghostBtn = CCMenuItemSpriteExtra::create(
            btnSprite, this, menu_selector(GhostMenuLayer::onGhostBtn)
        );
        ghostBtn->setID("ghost-button"_spr);
        
        // Позиционирование
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        if (menu->getID() == "bottom-menu") {
            // Добавляем в существующее меню
            menu->addChild(ghostBtn);
            menu->updateLayout();
        } else {
            // Позиционируем вручную
            ghostBtn->setPosition(ccp(winSize.width - 50.f, 50.f));
            auto customMenu = CCMenu::create();
            customMenu->setPosition(CCPointZero);
            customMenu->addChild(ghostBtn);
            customMenu->setZOrder(100);
            this->addChild(customMenu);
        }
        
        // Добавляем маленький лейбл "Ghost"
        auto label = CCLabelBMFont::create("Ghost", "bigFont.fnt");
        label->setScale(0.25f);
        label->setPosition(ccp(
            ghostBtn->getPositionX(),
            ghostBtn->getPositionY() - 22.f
        ));
        label->setOpacity(180);
        
        if (auto parentMenu = ghostBtn->getParent()) {
            parentMenu->addChild(label);
        }
        
        return true;
    }
    
    void onGhostBtn(CCObject* sender) {
        auto popup = GhostMainPopup::create();
        if (popup) {
            popup->show();
        }
    }
};
