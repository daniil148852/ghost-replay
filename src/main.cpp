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

    // Отслеживание изменений настроек (новый API Geode)
    listenForSettingChanges("auto-record", [](bool value) {
        log::info("Auto Record setting changed to: {}", value);
    });
}

$on_mod(Unloaded) {
    log::info("Ghost Replay mod unloaded!");
    
    // Останавливаем запись если идёт
    if (Recorder::get()->isRecording()) {
        Recorder::get()->stopRecording(nullptr);
    }
    
    // Очищаем призраков
    GhostManager::get()->onExitLevel();
}
