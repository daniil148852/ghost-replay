#include <Geode/Geode.hpp>
#include "FileManager.hpp"
#include "Recorder.hpp"
#include "GhostManager.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    log::info("Ghost Replay mod loaded!");
    
    // Инициализация файлового менеджера
    FileManager::get()->init();
    
    log::info("Records directory: {}", FileManager::get()->getRecordsDir().string());

    // Отслеживание изменений настроек
    listenForSettingChanges("auto-record", [](bool value) {
        log::info("Auto Record setting changed to: {}", value);
    });
}

// Unloaded больше не используется в Geode 4.x таким образом
// Очистка произойдёт автоматически при закрытии игры
