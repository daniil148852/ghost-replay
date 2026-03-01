#include "GhostPopup.hpp"
#include "GhostManager.hpp"
#include "utils/Utils.hpp"

// === GhostPopup ===

GhostPopup* GhostPopup::create(int levelID) {
    auto ret = new GhostPopup();
    if (ret && ret->initAnchored(380.f, 280.f, levelID)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool GhostPopup::setup(int levelID) {
    m_levelID = levelID;

    this->setTitle("Ghost Recordings");

    auto refreshSpr = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    refreshSpr->setScale(0.8f);
    auto refreshBtn = CCMenuItemSpriteExtra::create(
        refreshSpr, this, menu_selector(GhostPopup::onRefresh)
    );
    refreshBtn->setPosition(ccp(170.f, 125.f));
    m_buttonMenu->addChild(refreshBtn);

    auto deleteAllSpr = ButtonSprite::create("Delete All", "goldFont.fnt", "GJ_button_06.png", 0.6f);
    deleteAllSpr->setScale(0.7f);
    auto deleteAllBtn = CCMenuItemSpriteExtra::create(
        deleteAllSpr, this, menu_selector(GhostPopup::onDeleteAll)
    );
    deleteAllBtn->setPosition(ccp(0.f, -125.f));
    m_buttonMenu->addChild(deleteAllBtn);

    std::string infoText = fmt::format("Level ID: {}", m_levelID);
    auto infoLabel = CCLabelBMFont::create(infoText.c_str(), "bigFont.fnt");
    infoLabel->setScale(0.35f);
    infoLabel->setPosition(ccp(190.f, 245.f));
    infoLabel->setAnchorPoint(ccp(0.5f, 0.5f));
    m_mainLayer->addChild(infoLabel);

    m_scrollLayer = ScrollLayer::create(ccp(340.f, 200.f));
    m_scrollLayer->setPosition(ccp(20.f, 30.f));
    m_mainLayer->addChild(m_scrollLayer);

    auto scrollBg = CCLayerColor::create(ccc4(0, 0, 0, 80), 340.f, 200.f);
    scrollBg->setPosition(ccp(20.f, 30.f));
    m_mainLayer->addChild(scrollBg, -1);

    refreshList();

    return true;
}

void GhostPopup::refreshList() {
    m_records = FileManager::get()->getRecordsForLevel(m_levelID);

    auto content = m_scrollLayer->m_contentLayer;
    content->removeAllChildren();

    if (m_records.empty()) {
        auto emptyLabel = CCLabelBMFont::create("No recordings found", "bigFont.fnt");
        emptyLabel->setScale(0.4f);
        emptyLabel->setPosition(ccp(170.f, 90.f));
        content->addChild(emptyLabel);
        content->setContentSize(ccp(340.f, 200.f));
        return;
    }

    float cellHeight = 45.f;
    // FIX: явный каст к float чтобы избежать конфликта типов в std::max
    float totalHeight = cellHeight * static_cast<float>(m_records.size());
    if (totalHeight < 200.f) totalHeight = 200.f;

    content->setContentSize(ccp(340.f, totalHeight));

    for (int i = 0; i < static_cast<int>(m_records.size()); i++) {
        auto cell = createRecordCell(m_records[i], i);
        float y = totalHeight - (i + 1) * cellHeight;
        cell->setPosition(ccp(0.f, y));
        content->addChild(cell);
    }

    m_scrollLayer->moveToTop();
}

CCNode* GhostPopup::createRecordCell(const RecordFileInfo& info, int index) {
    auto cell = CCNode::create();
    cell->setContentSize(ccp(340.f, 45.f));

    auto bg = CCLayerColor::create(
        index % 2 == 0 ? ccc4(50, 50, 60, 150) : ccc4(40, 40, 50, 150),
        340.f, 44.f
    );
    bg->setPosition(ccp(0.f, 0.5f));
    cell->addChild(bg, -1);

    char percentBuf[32];
    snprintf(percentBuf, sizeof(percentBuf), "%.1f%%", info.meta.finalPercent);
    auto percentLabel = CCLabelBMFont::create(percentBuf, "bigFont.fnt");
    percentLabel->setScale(0.4f);
    percentLabel->setPosition(ccp(40.f, 28.f));
    percentLabel->setAnchorPoint(ccp(0.f, 0.5f));

    if (info.meta.finalPercent >= 100.f) {
        percentLabel->setColor(ccc3(100, 255, 100));
    } else if (info.meta.finalPercent >= 50.f) {
        percentLabel->setColor(ccc3(255, 255, 100));
    } else {
        percentLabel->setColor(ccc3(255, 150, 150));
    }
    cell->addChild(percentLabel);

    std::string timeStr = ghost_utils::formatTimestamp(info.meta.timestamp);
    auto timeLabel = CCLabelBMFont::create(timeStr.c_str(), "chatFont.fnt");
    timeLabel->setScale(0.55f);
    timeLabel->setPosition(ccp(40.f, 12.f));
    timeLabel->setAnchorPoint(ccp(0.f, 0.5f));
    timeLabel->setColor(ccc3(180, 180, 180));
    cell->addChild(timeLabel);

    char framesBuf[32];
    snprintf(framesBuf, sizeof(framesBuf), "%u frames", info.meta.totalFrames);
    auto framesLabel = CCLabelBMFont::create(framesBuf, "chatFont.fnt");
    framesLabel->setScale(0.45f);
    framesLabel->setPosition(ccp(200.f, 12.f));
    framesLabel->setAnchorPoint(ccp(0.f, 0.5f));
    framesLabel->setColor(ccc3(150, 150, 150));
    cell->addChild(framesLabel);

    auto playerIcon = SimplePlayer::create(info.meta.iconID);
    playerIcon->setScale(0.5f);
    auto gm = GameManager::sharedState();
    playerIcon->setColor(gm->colorForIdx(info.meta.color1));
    playerIcon->setSecondColor(gm->colorForIdx(info.meta.color2));
    playerIcon->setPosition(ccp(18.f, 22.f));
    cell->addChild(playerIcon);

    auto menu = CCMenu::create();
    menu->setPosition(CCPointZero);
    cell->addChild(menu);

    auto toggleOnSpr = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    auto toggleOffSpr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    toggleOnSpr->setScale(0.7f);
    toggleOffSpr->setScale(0.7f);

    auto toggleBtn = CCMenuItemToggler::create(
        toggleOffSpr, toggleOnSpr,
        this, menu_selector(GhostPopup::onToggleRecord)
    );
    toggleBtn->setPosition(ccp(280.f, 22.f));
    toggleBtn->setTag(index);
    toggleBtn->toggle(info.enabled);
    menu->addChild(toggleBtn);

    auto deleteSpr = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
    deleteSpr->setScale(0.6f);
    auto deleteBtn = CCMenuItemSpriteExtra::create(
        deleteSpr, this, menu_selector(GhostPopup::onDeleteRecord)
    );
    deleteBtn->setPosition(ccp(320.f, 22.f));
    deleteBtn->setTag(index);
    menu->addChild(deleteBtn);

    return cell;
}

void GhostPopup::onToggleRecord(CCObject* sender) {
    auto toggle = static_cast<CCMenuItemToggler*>(sender);
    int idx = toggle->getTag();

    if (idx < 0 || idx >= static_cast<int>(m_records.size())) return;

    bool enabled = !toggle->isToggled();

    auto& info = m_records[idx];
    FileManager::get()->setRecordEnabled(info.fullPath, enabled);
    info.enabled = enabled;

    auto ghostMgr = GhostManager::get();
    if (ghostMgr->getPlayLayer()) {
        if (enabled) {
            ghostMgr->addGhost(info.fullPath);
        } else {
            ghostMgr->removeGhost(info.fullPath);
        }
    }
}

void GhostPopup::onDeleteRecord(CCObject* sender) {
    int idx = sender->getTag();
    if (idx < 0 || idx >= static_cast<int>(m_records.size())) return;

    auto& info = m_records[idx];

    geode::createQuickPopup(
        "Delete Recording",
        fmt::format("Delete recording with {:.1f}%?", info.meta.finalPercent),
        "Cancel", "Delete",
        [this, path = info.fullPath](auto*, bool btn2) {
            if (btn2) {
                GhostManager::get()->removeGhost(path);
                FileManager::get()->deleteRecord(path);
                this->refreshList();
            }
        }
    );
}

void GhostPopup::onDeleteAll(CCObject* sender) {
    geode::createQuickPopup(
        "Delete All",
        "Delete ALL recordings for this level?",
        "Cancel", "Delete All",
        [this](auto*, bool btn2) {
            if (btn2) {
                GhostManager::get()->removeAllGhosts();
                for (auto& info : m_records) {
                    FileManager::get()->deleteRecord(info.fullPath);
                }
                refreshList();
            }
        }
    );
}

void GhostPopup::onRefresh(CCObject* sender) {
    refreshList();
}

void GhostPopup::onClose(CCObject* sender) {
    Popup::onClose(sender);
}

// === GhostMainPopup ===

GhostMainPopup* GhostMainPopup::create() {
    auto ret = new GhostMainPopup();
    if (ret && ret->initAnchored(350.f, 260.f)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool GhostMainPopup::setup() {
    this->setTitle("Ghost Recordings");

    m_scrollLayer = ScrollLayer::create(ccp(310.f, 190.f));
    m_scrollLayer->setPosition(ccp(20.f, 25.f));
    m_mainLayer->addChild(m_scrollLayer);

    auto scrollBg = CCLayerColor::create(ccc4(0, 0, 0, 80), 310.f, 190.f);
    scrollBg->setPosition(ccp(20.f, 25.f));
    m_mainLayer->addChild(scrollBg, -1);

    refreshList();

    return true;
}

void GhostMainPopup::refreshList() {
    auto allRecords = FileManager::get()->getAllRecords();

    auto content = m_scrollLayer->m_contentLayer;
    content->removeAllChildren();

    if (allRecords.empty()) {
        auto emptyLabel = CCLabelBMFont::create("No recordings yet!\nPlay some levels first.", "bigFont.fnt");
        emptyLabel->setScale(0.35f);
        emptyLabel->setPosition(ccp(155.f, 95.f));
        emptyLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
        content->addChild(emptyLabel);
        content->setContentSize(ccp(310.f, 190.f));
        return;
    }

    std::map<int, std::pair<std::string, int>> levelMap;
    for (auto& rec : allRecords) {
        auto& entry = levelMap[rec.meta.levelID];
        if (entry.first.empty()) entry.first = rec.meta.levelName;
        entry.second++;
    }

    float cellHeight = 40.f;
    // FIX: избегаем конфликта типов
    float totalHeight = cellHeight * static_cast<float>(levelMap.size());
    if (totalHeight < 190.f) totalHeight = 190.f;

    content->setContentSize(ccp(310.f, totalHeight));

    int i = 0;
    for (auto& [levelID, info] : levelMap) {
        auto cell = createLevelCell(levelID, info.first, info.second);
        float y = totalHeight - (i + 1) * cellHeight;
        cell->setPosition(ccp(0.f, y));
        content->addChild(cell);
        i++;
    }

    m_scrollLayer->moveToTop();
}

CCNode* GhostMainPopup::createLevelCell(int levelID, const std::string& levelName, int recordCount) {
    auto cell = CCNode::create();
    cell->setContentSize(ccp(310.f, 40.f));

    auto bg = CCLayerColor::create(ccc4(50, 50, 70, 150), 310.f, 39.f);
    bg->setPosition(ccp(0.f, 0.5f));
    cell->addChild(bg, -1);

    std::string displayName = levelName;
    if (displayName.length() > 25) {
        displayName = displayName.substr(0, 22) + "...";
    }
    auto nameLabel = CCLabelBMFont::create(displayName.c_str(), "bigFont.fnt");
    nameLabel->setScale(0.35f);
    nameLabel->setPosition(ccp(10.f, 25.f));
    nameLabel->setAnchorPoint(ccp(0.f, 0.5f));
    cell->addChild(nameLabel);

    char countBuf[32];
    snprintf(countBuf, sizeof(countBuf), "%d recordings", recordCount);
    auto countLabel = CCLabelBMFont::create(countBuf, "chatFont.fnt");
    countLabel->setScale(0.5f);
    countLabel->setPosition(ccp(10.f, 10.f));
    countLabel->setAnchorPoint(ccp(0.f, 0.5f));
    countLabel->setColor(ccc3(180, 180, 180));
    cell->addChild(countLabel);

    auto menu = CCMenu::create();
    menu->setPosition(CCPointZero);
    cell->addChild(menu);

    auto viewSpr = ButtonSprite::create("View", "goldFont.fnt", "GJ_button_01.png", 0.6f);
    viewSpr->setScale(0.6f);
    auto viewBtn = CCMenuItemSpriteExtra::create(
        viewSpr, this, menu_selector(GhostMainPopup::onSelectLevel)
    );
    viewBtn->setPosition(ccp(270.f, 20.f));
    viewBtn->setTag(levelID);
    menu->addChild(viewBtn);

    return cell;
}

void GhostMainPopup::onSelectLevel(CCObject* sender) {
    int levelID = sender->getTag();
    auto popup = GhostPopup::create(levelID);
    if (popup) {
        popup->show();
    }
}
