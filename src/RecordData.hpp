#pragma once

#include <Geode/Geode.hpp>
#include <vector>
#include <string>

using namespace geode::prelude;

// Один кадр записи
struct FrameData {
    uint32_t frame;          // номер кадра
    float posX;              // позиция X
    float posY;              // позиция Y
    float rotation;          // вращение
    float yVelocity;         // вертикальная скорость
    float playerScale;       // масштаб игрока
    bool isHolding;          // зажата ли кнопка
    bool isOnGround;         // на земле ли
    bool isDead;             // мёртв ли
    bool isUpsideDown;       // перевёрнут ли (гравитация)
    bool isDashing;          // в дэше ли
    uint8_t gameMode;        // режим игры (куб, корабль и т.д.)
    bool isVisible;          // видим ли
    bool isMini;             // мини-режим
    
    // Сериализация
    void write(std::ofstream& stream) const;
    void read(std::ifstream& stream);
};

// Действие игрока (нажатие/отпускание)
struct InputAction {
    uint32_t frame;
    bool isPress;            // true = нажатие, false = отпускание
    bool isPlayer2;          // для dual mode
    int button;              // какая кнопка
    
    void write(std::ofstream& stream) const;
    void read(std::ifstream& stream);
};

// Метаданные записи
struct RecordMeta {
    static constexpr uint32_t MAGIC = 0x47485354; // "GHST"
    static constexpr uint32_t VERSION = 2;
    
    uint32_t magic = MAGIC;
    uint32_t version = VERSION;
    int levelID = 0;
    std::string levelName;
    std::string timestamp;
    float finalPercent = 0.f;
    uint32_t totalFrames = 0;
    float fps = 60.f;
    
    // Внешний вид игрока при записи
    int iconID = 1;
    int color1 = 0;
    int color2 = 3;
    int glowColor = 0;
    bool hasGlow = false;
    int shipID = 1;
    int ballID = 1;
    int ufoID = 1;
    int waveID = 1;
    int robotID = 1;
    int spiderID = 1;
    int swingID = 1;
    int deathEffect = 1;
    
    void write(std::ofstream& stream) const;
    void read(std::ifstream& stream);
};

// Полная запись
class RecordData {
public:
    RecordMeta meta;
    std::vector<FrameData> frames;
    std::vector<InputAction> inputs;
    
    // Операции с файлами
    bool saveToFile(const std::string& path) const;
    bool loadFromFile(const std::string& path);
    
    // Утилиты
    void clear();
    bool isEmpty() const { return frames.empty(); }
    
    // Получить кадр по номеру (с интерполяцией)
    FrameData getFrameAt(uint32_t frameNum) const;
    FrameData getInterpolatedFrame(float exactFrame) const;
    
    // Информация
    std::string getDisplayName() const;
    float getDuration() const;
    
private:
    // Бинарный поиск ближайшего кадра
    size_t findNearestFrameIndex(uint32_t frameNum) const;
};
