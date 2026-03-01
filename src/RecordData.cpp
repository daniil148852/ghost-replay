#include "RecordData.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>

// === FrameData ===

void FrameData::write(std::ofstream& stream) const {
    stream.write(reinterpret_cast<const char*>(&frame), sizeof(frame));
    stream.write(reinterpret_cast<const char*>(&posX), sizeof(posX));
    stream.write(reinterpret_cast<const char*>(&posY), sizeof(posY));
    stream.write(reinterpret_cast<const char*>(&rotation), sizeof(rotation));
    stream.write(reinterpret_cast<const char*>(&yVelocity), sizeof(yVelocity));
    stream.write(reinterpret_cast<const char*>(&playerScale), sizeof(playerScale));
    
    // Упаковка булевых значений в один байт
    uint8_t flags = 0;
    if (isHolding)    flags |= (1 << 0);
    if (isOnGround)   flags |= (1 << 1);
    if (isDead)       flags |= (1 << 2);
    if (isUpsideDown) flags |= (1 << 3);
    if (isDashing)    flags |= (1 << 4);
    if (isVisible)    flags |= (1 << 5);
    if (isMini)       flags |= (1 << 6);
    stream.write(reinterpret_cast<const char*>(&flags), sizeof(flags));
    
    stream.write(reinterpret_cast<const char*>(&gameMode), sizeof(gameMode));
}

void FrameData::read(std::ifstream& stream) {
    stream.read(reinterpret_cast<char*>(&frame), sizeof(frame));
    stream.read(reinterpret_cast<char*>(&posX), sizeof(posX));
    stream.read(reinterpret_cast<char*>(&posY), sizeof(posY));
    stream.read(reinterpret_cast<char*>(&rotation), sizeof(rotation));
    stream.read(reinterpret_cast<char*>(&yVelocity), sizeof(yVelocity));
    stream.read(reinterpret_cast<char*>(&playerScale), sizeof(playerScale));
    
    uint8_t flags = 0;
    stream.read(reinterpret_cast<char*>(&flags), sizeof(flags));
    isHolding    = (flags >> 0) & 1;
    isOnGround   = (flags >> 1) & 1;
    isDead       = (flags >> 2) & 1;
    isUpsideDown = (flags >> 3) & 1;
    isDashing    = (flags >> 4) & 1;
    isVisible    = (flags >> 5) & 1;
    isMini       = (flags >> 6) & 1;
    
    stream.read(reinterpret_cast<char*>(&gameMode), sizeof(gameMode));
}

// === InputAction ===

void InputAction::write(std::ofstream& stream) const {
    stream.write(reinterpret_cast<const char*>(&frame), sizeof(frame));
    uint8_t flags = 0;
    if (isPress) flags |= 1;
    if (isPlayer2) flags |= 2;
    stream.write(reinterpret_cast<const char*>(&flags), sizeof(flags));
    stream.write(reinterpret_cast<const char*>(&button), sizeof(button));
}

void InputAction::read(std::ifstream& stream) {
    stream.read(reinterpret_cast<char*>(&frame), sizeof(frame));
    uint8_t flags = 0;
    stream.read(reinterpret_cast<char*>(&flags), sizeof(flags));
    isPress = (flags & 1) != 0;
    isPlayer2 = (flags & 2) != 0;
    stream.read(reinterpret_cast<char*>(&button), sizeof(button));
}

// === RecordMeta ===

static void writeString(std::ofstream& stream, const std::string& str) {
    uint32_t len = static_cast<uint32_t>(str.length());
    stream.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (len > 0) {
        stream.write(str.data(), len);
    }
}

static std::string readString(std::ifstream& stream) {
    uint32_t len = 0;
    stream.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (len == 0 || len > 10000) return "";
    std::string str(len, '\0');
    stream.read(str.data(), len);
    return str;
}

void RecordMeta::write(std::ofstream& stream) const {
    stream.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
    stream.write(reinterpret_cast<const char*>(&version), sizeof(version));
    stream.write(reinterpret_cast<const char*>(&levelID), sizeof(levelID));
    writeString(stream, levelName);
    writeString(stream, timestamp);
    stream.write(reinterpret_cast<const char*>(&finalPercent), sizeof(finalPercent));
    stream.write(reinterpret_cast<const char*>(&totalFrames), sizeof(totalFrames));
    stream.write(reinterpret_cast<const char*>(&fps), sizeof(fps));
    
    // Внешний вид
    stream.write(reinterpret_cast<const char*>(&iconID), sizeof(iconID));
    stream.write(reinterpret_cast<const char*>(&color1), sizeof(color1));
    stream.write(reinterpret_cast<const char*>(&color2), sizeof(color2));
    stream.write(reinterpret_cast<const char*>(&glowColor), sizeof(glowColor));
    uint8_t glowFlag = hasGlow ? 1 : 0;
    stream.write(reinterpret_cast<const char*>(&glowFlag), sizeof(glowFlag));
    stream.write(reinterpret_cast<const char*>(&shipID), sizeof(shipID));
    stream.write(reinterpret_cast<const char*>(&ballID), sizeof(ballID));
    stream.write(reinterpret_cast<const char*>(&ufoID), sizeof(ufoID));
    stream.write(reinterpret_cast<const char*>(&waveID), sizeof(waveID));
    stream.write(reinterpret_cast<const char*>(&robotID), sizeof(robotID));
    stream.write(reinterpret_cast<const char*>(&spiderID), sizeof(spiderID));
    stream.write(reinterpret_cast<const char*>(&swingID), sizeof(swingID));
    stream.write(reinterpret_cast<const char*>(&deathEffect), sizeof(deathEffect));
}

void RecordMeta::read(std::ifstream& stream) {
    stream.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    if (magic != MAGIC) return;
    
    stream.read(reinterpret_cast<char*>(&version), sizeof(version));
    stream.read(reinterpret_cast<char*>(&levelID), sizeof(levelID));
    levelName = readString(stream);
    timestamp = readString(stream);
    stream.read(reinterpret_cast<char*>(&finalPercent), sizeof(finalPercent));
    stream.read(reinterpret_cast<char*>(&totalFrames), sizeof(totalFrames));
    stream.read(reinterpret_cast<char*>(&fps), sizeof(fps));
    
    if (version >= 2) {
        stream.read(reinterpret_cast<char*>(&iconID), sizeof(iconID));
        stream.read(reinterpret_cast<char*>(&color1), sizeof(color1));
        stream.read(reinterpret_cast<char*>(&color2), sizeof(color2));
        stream.read(reinterpret_cast<char*>(&glowColor), sizeof(glowColor));
        uint8_t glowFlag = 0;
        stream.read(reinterpret_cast<char*>(&glowFlag), sizeof(glowFlag));
        hasGlow = glowFlag != 0;
        stream.read(reinterpret_cast<char*>(&shipID), sizeof(shipID));
        stream.read(reinterpret_cast<char*>(&ballID), sizeof(ballID));
        stream.read(reinterpret_cast<char*>(&ufoID), sizeof(ufoID));
        stream.read(reinterpret_cast<char*>(&waveID), sizeof(waveID));
        stream.read(reinterpret_cast<char*>(&robotID), sizeof(robotID));
        stream.read(reinterpret_cast<char*>(&spiderID), sizeof(spiderID));
        stream.read(reinterpret_cast<char*>(&swingID), sizeof(swingID));
        stream.read(reinterpret_cast<char*>(&deathEffect), sizeof(deathEffect));
    }
}

// === RecordData ===

bool RecordData::saveToFile(const std::string& path) const {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        log::error("Failed to open file for writing: {}", path);
        return false;
    }
    
    // Записываем мету
    meta.write(file);
    
    // Количество кадров
    uint32_t frameCount = static_cast<uint32_t>(frames.size());
    file.write(reinterpret_cast<const char*>(&frameCount), sizeof(frameCount));
    
    // Кадры
    for (auto& f : frames) {
        f.write(file);
    }
    
    // Количество инпутов
    uint32_t inputCount = static_cast<uint32_t>(inputs.size());
    file.write(reinterpret_cast<const char*>(&inputCount), sizeof(inputCount));
    
    // Инпуты
    for (auto& inp : inputs) {
        inp.write(file);
    }
    
    file.close();
    log::info("Saved recording: {} frames, {} inputs to {}", frameCount, inputCount, path);
    return true;
}

bool RecordData::loadFromFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        log::error("Failed to open file for reading: {}", path);
        return false;
    }
    
    clear();
    
    // Читаем мету
    meta.read(file);
    if (meta.magic != RecordMeta::MAGIC) {
        log::error("Invalid file format: {}", path);
        return false;
    }
    
    // Кадры
    uint32_t frameCount = 0;
    file.read(reinterpret_cast<char*>(&frameCount), sizeof(frameCount));
    
    if (frameCount > 1000000) { // защита от поврежденных файлов
        log::error("Too many frames: {}", frameCount);
        return false;
    }
    
    frames.resize(frameCount);
    for (uint32_t i = 0; i < frameCount; i++) {
        frames[i].read(file);
    }
    
    // Инпуты
    uint32_t inputCount = 0;
    file.read(reinterpret_cast<char*>(&inputCount), sizeof(inputCount));
    
    if (inputCount > 1000000) {
        log::error("Too many inputs: {}", inputCount);
        return false;
    }
    
    inputs.resize(inputCount);
    for (uint32_t i = 0; i < inputCount; i++) {
        inputs[i].read(file);
    }
    
    file.close();
    log::info("Loaded recording: {} frames, {} inputs from {}", frameCount, inputCount, path);
    return true;
}

void RecordData::clear() {
    frames.clear();
    inputs.clear();
    meta = RecordMeta();
}

size_t RecordData::findNearestFrameIndex(uint32_t frameNum) const {
    if (frames.empty()) return 0;
    
    // Бинарный поиск
    size_t lo = 0, hi = frames.size() - 1;
    while (lo < hi) {
        size_t mid = (lo + hi) / 2;
        if (frames[mid].frame < frameNum) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return lo;
}

FrameData RecordData::getFrameAt(uint32_t frameNum) const {
    if (frames.empty()) return FrameData{};
    
    size_t idx = findNearestFrameIndex(frameNum);
    
    // Если точно нашли
    if (frames[idx].frame == frameNum) return frames[idx];
    
    // Если за пределами — вернуть последний
    if (idx == 0) return frames[0];
    if (idx >= frames.size()) return frames.back();
    
    // Вернуть предыдущий (ближайший более ранний)
    if (frames[idx].frame > frameNum && idx > 0) {
        return frames[idx - 1];
    }
    
    return frames[idx];
}

FrameData RecordData::getInterpolatedFrame(float exactFrame) const {
    if (frames.empty()) return FrameData{};
    
    uint32_t frameFloor = static_cast<uint32_t>(exactFrame);
    float t = exactFrame - frameFloor;
    
    size_t idx = findNearestFrameIndex(frameFloor);
    
    if (idx >= frames.size() - 1 || t < 0.001f) {
        return getFrameAt(frameFloor);
    }
    
    // Интерполяция между двумя кадрами
    const auto& a = frames[idx];
    const auto& b = frames[std::min(idx + 1, frames.size() - 1)];
    
    FrameData result = a;
    result.posX = a.posX + (b.posX - a.posX) * t;
    result.posY = a.posY + (b.posY - a.posY) * t;
    result.rotation = a.rotation + (b.rotation - a.rotation) * t;
    
    return result;
}

std::string RecordData::getDisplayName() const {
    std::string name = meta.levelName;
    if (name.empty()) name = "Level " + std::to_string(meta.levelID);
    
    char buf[32];
    snprintf(buf, sizeof(buf), " (%.1f%%)", meta.finalPercent);
    name += buf;
    
    return name;
}

float RecordData::getDuration() const {
    if (frames.empty() || meta.fps <= 0) return 0.f;
    return static_cast<float>(meta.totalFrames) / meta.fps;
}
