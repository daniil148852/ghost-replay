#include "GhostManager.hpp"
#include "FileManager.hpp"
#include "utils/Utils.hpp"

GhostManager* GhostManager::get() {
    static GhostManager instance;
    return &instance;
}

void GhostManager::onEnterLevel(PlayLayer* pl) {
    m_playLayer = pl;
    
    if (pl && pl->m_level) {
        m_currentLevelID = ghost_utils::getLevelID(pl->m_level);
    }
    
    createGhostLayer();
    loadEnabledGhosts(m_currentLevelID);
    
    log::info("GhostManager: Entered level {}, {} ghosts loaded", 
        m_currentLevelID, m_ghosts.size());
}

void GhostManager::onExitLevel() {
    removeAllGhosts();
    m_ghostLayer = nullptr;
    m_playLayer = nullptr;
    m_currentLevelID = 0;
}

void GhostManager::createGhostLayer() {
    if (!m_playLayer) return;
    
    m_ghostLayer = CCNode::create();
    m_ghostLayer->setID("ghost-layer"_spr);
    m_ghostLayer->setZOrder(10); // Над основным контентом, но ниже UI
    
    // Добавляем в objectLayer PlayLayer
    if (auto objectLayer = m_playLayer->m_objectLayer) {
        objectLayer->addChild(m_ghostLayer);
    } else {
        m_playLayer->addChild(m_ghostLayer);
    }
}

void GhostManager::update(uint32_t currentFrame) {
    for (auto& entry : m_ghosts) {
        if (entry.ghost) {
            entry.ghost->updateGhost(currentFrame);
        }
    }
}

void GhostManager::reset() {
    for (auto& entry : m_ghosts) {
        if (entry.ghost) {
            entry.ghost->reset();
        }
    }
}

void GhostManager::addGhost(const std::string& recordPath) {
    if (!m_playLayer || !m_ghostLayer) return;
    
    // Проверяем лимит
    int maxGhosts = static_cast<int>(Mod::get()->getSettingValue<int64_t>("max-ghosts"));
    if (static_cast<int>(m_ghosts.size()) >= maxGhosts) {
        log::warn("Max ghost limit reached ({})", maxGhosts);
        return;
    }
    
    // Проверяем что этот призрак ещё не добавлен
    for (auto& entry : m_ghosts) {
        if (entry.filePath == recordPath) return;
    }
    
    // Загружаем запись
    RecordData record;
    if (!FileManager::get()->loadRecord(recordPath, record)) {
        log::error("Failed to load ghost record: {}", recordPath);
        return;
    }
    
    // Создаём призрака
    auto ghost = GhostPlayer::create(record, m_playLayer);
    if (!ghost) {
        log::error("Failed to create ghost player");
        return;
    }
    
    ghost->setFilePath(recordPath);
    m_ghostLayer->addChild(ghost);
    
    GhostEntry entry;
    entry.ghost = ghost;
    entry.filePath = recordPath;
    m_ghosts.push_back(entry);
    
    log::info("Added ghost: {} ({:.1f}%)", record.getDisplayName(), record.meta.finalPercent);
}

void GhostManager::removeGhost(const std::string& recordPath) {
    auto it = std::remove_if(m_ghosts.begin(), m_ghosts.end(), 
        [&](const GhostEntry& entry) {
            if (entry.filePath == recordPath) {
                if (entry.ghost) {
                    entry.ghost->removeFromParent();
                }
                return true;
            }
            return false;
        });
    m_ghosts.erase(it, m_ghosts.end());
}

void GhostManager::removeAllGhosts() {
    for (auto& entry : m_ghosts) {
        if (entry.ghost) {
            entry.ghost->removeFromParent();
        }
    }
    m_ghosts.clear();
}

void GhostManager::loadEnabledGhosts(int levelID) {
    removeAllGhosts();
    
    auto enabledPaths = FileManager::get()->getEnabledRecords(levelID);
    
    for (auto& path : enabledPaths) {
        addGhost(path);
    }
}
