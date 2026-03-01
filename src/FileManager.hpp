#pragma once

#include "RecordData.hpp"
#include <Geode/Geode.hpp>
#include <vector>
#include <string>
#include <filesystem>

using namespace geode::prelude;
namespace fs = std::filesystem;

struct RecordFileInfo {
    std::string filename;
    std::string fullPath;
    RecordMeta meta;
    bool enabled = false; // включен ли для воспроизведения
};

class FileManager {
public:
    static FileManager* get();
    
    // Инициализация
    void init();
    
    // Сохранение
    bool saveRecord(const RecordData& record);
    
    // Загрузка
    bool loadRecord(const std::string& path, RecordData& out);
    
    // Получение списка записей для уровня
    std::vector<RecordFileInfo> getRecordsForLevel(int levelID);
    
    // Получение всех записей
    std::vector<RecordFileInfo> getAllRecords();
    
    // Удаление записи
    bool deleteRecord(const std::string& path);
    
    // Управление включёнными записями
    void setRecordEnabled(const std::string& path, bool enabled);
    bool isRecordEnabled(const std::string& path) const;
    std::vector<std::string> getEnabledRecords(int levelID) const;
    void clearEnabledRecords();
    
    // Путь к папке записей
    fs::path getRecordsDir() const;
    
    // Очистка старых записей (оставить N лучших)
    void cleanupRecords(int levelID, int keepCount = 20);
    
private:
    FileManager() = default;
    
    std::map<std::string, bool> m_enabledRecords;
    
    std::string generateFilename(int levelID, const std::string& timestamp) const;
};
