#include "FileManager.hpp"
#include "utils/Utils.hpp"
#include <algorithm>

FileManager* FileManager::get() {
    static FileManager instance;
    return &instance;
}

void FileManager::init() {
    auto dir = getRecordsDir();
    if (!fs::exists(dir)) {
        std::error_code ec;
        fs::create_directories(dir, ec);
        if (ec) {
            log::error("Failed to create records directory: {}", ec.message());
        }
    }
    log::info("Records directory: {}", dir.string());
}

fs::path FileManager::getRecordsDir() const {
    return Mod::get()->getSaveDir() / "recordings";
}

std::string FileManager::generateFilename(int levelID, const std::string& timestamp) const {
    return fmt::format("ghost_{}_{}.ghr", levelID, timestamp);
}

bool FileManager::saveRecord(const RecordData& record) {
    auto dir = getRecordsDir();
    if (!fs::exists(dir)) {
        std::error_code ec;
        fs::create_directories(dir, ec);
        if (ec) return false;
    }
    
    std::string filename = generateFilename(record.meta.levelID, record.meta.timestamp);
    std::string fullPath = (dir / filename).string();
    
    bool result = record.saveToFile(fullPath);
    
    if (result) {
        log::info("Saved recording: {}", filename);
    }
    
    return result;
}

bool FileManager::loadRecord(const std::string& path, RecordData& out) {
    return out.loadFromFile(path);
}

std::vector<RecordFileInfo> FileManager::getRecordsForLevel(int levelID) {
    std::vector<RecordFileInfo> result;
    auto dir = getRecordsDir();
    
    if (!fs::exists(dir)) return result;
    
    std::string prefix = fmt::format("ghost_{}_", levelID);
    
    for (auto& entry : fs::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;
        
        std::string filename = entry.path().filename().string();
        if (filename.find(prefix) != 0) continue;
        if (entry.path().extension() != ".ghr") continue;
        
        RecordFileInfo info;
        info.filename = filename;
        info.fullPath = entry.path().string();
        info.enabled = isRecordEnabled(info.fullPath);
        
        // Читаем только мету (быстро)
        RecordData temp;
        std::ifstream file(info.fullPath, std::ios::binary);
        if (file.is_open()) {
            info.meta.read(file);
            file.close();
            
            if (info.meta.magic == RecordMeta::MAGIC) {
                result.push_back(info);
            }
        }
    }
    
    // Сортируем по проценту (лучшие сверху)
    std::sort(result.begin(), result.end(), [](const RecordFileInfo& a, const RecordFileInfo& b) {
        return a.meta.finalPercent > b.meta.finalPercent;
    });
    
    return result;
}

std::vector<RecordFileInfo> FileManager::getAllRecords() {
    std::vector<RecordFileInfo> result;
    auto dir = getRecordsDir();
    
    if (!fs::exists(dir)) return result;
    
    for (auto& entry : fs::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".ghr") continue;
        
        RecordFileInfo info;
        info.filename = entry.path().filename().string();
        info.fullPath = entry.path().string();
        info.enabled = isRecordEnabled(info.fullPath);
        
        std::ifstream file(info.fullPath, std::ios::binary);
        if (file.is_open()) {
            info.meta.read(file);
            file.close();
            
            if (info.meta.magic == RecordMeta::MAGIC) {
                result.push_back(info);
            }
        }
    }
    
    return result;
}

bool FileManager::deleteRecord(const std::string& path) {
    std::error_code ec;
    bool removed = fs::remove(path, ec);
    if (removed) {
        m_enabledRecords.erase(path);
        log::info("Deleted recording: {}", path);
    }
    return removed;
}

void FileManager::setRecordEnabled(const std::string& path, bool enabled) {
    m_enabledRecords[path] = enabled;
}

bool FileManager::isRecordEnabled(const std::string& path) const {
    auto it = m_enabledRecords.find(path);
    if (it != m_enabledRecords.end()) return it->second;
    return false;
}

std::vector<std::string> FileManager::getEnabledRecords(int levelID) const {
    std::vector<std::string> result;
    
    for (auto& [path, enabled] : m_enabledRecords) {
        if (!enabled) continue;
        
        // Проверяем что файл для нужного уровня
        std::string prefix = fmt::format("ghost_{}_", levelID);
        std::string filename = fs::path(path).filename().string();
        
        if (filename.find(prefix) == 0) {
            result.push_back(path);
        }
    }
    
    return result;
}

void FileManager::clearEnabledRecords() {
    m_enabledRecords.clear();
}

void FileManager::cleanupRecords(int levelID, int keepCount) {
    auto records = getRecordsForLevel(levelID);
    
    // Уже отсортированы по проценту (лучшие первые)
    if (static_cast<int>(records.size()) <= keepCount) return;
    
    // Удаляем всё после keepCount, кроме включённых
    for (int i = keepCount; i < static_cast<int>(records.size()); i++) {
        if (!records[i].enabled) {
            deleteRecord(records[i].fullPath);
        }
    }
}
