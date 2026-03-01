#pragma once

#include "FileManager.hpp"
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>

using namespace geode::prelude;

class GhostPopup : public geode::Popup<int> {
public:
    static GhostPopup* create(int levelID);
    
protected:
    bool setup(int levelID) override;
    
    void onClose(CCObject*) override;
    
private:
    int m_levelID;
    ScrollLayer* m_scrollLayer = nullptr;
    CCMenu* m_listMenu = nullptr;
    std::vector<RecordFileInfo> m_records;
    
    void refreshList();
    CCNode* createRecordCell(const RecordFileInfo& info, int index);
    
    void onToggleRecord(CCObject* sender);
    void onDeleteRecord(CCObject* sender);
    void onDeleteAll(CCObject* sender);
    void onRefresh(CCObject* sender);
};

// Попап для выбора уровня (из главного меню)
class GhostMainPopup : public geode::Popup<> {
public:
    static GhostMainPopup* create();
    
protected:
    bool setup() override;
    
private:
    ScrollLayer* m_scrollLayer = nullptr;
    
    void refreshList();
    CCNode* createLevelCell(int levelID, const std::string& levelName, int recordCount);
    
    void onSelectLevel(CCObject* sender);
};
