#pragma once

#include <Geode/Geode.hpp>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>

using namespace geode::prelude;

namespace ghost_utils {

    inline std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        ss << "_" << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    inline std::string formatTimestamp(const std::string& raw) {
        if (raw.length() < 15) return raw;
        // 20240101_120000_000 -> 2024-01-01 12:00:00
        std::string result;
        result += raw.substr(0, 4) + "-" + raw.substr(4, 2) + "-" + raw.substr(6, 2);
        result += " " + raw.substr(9, 2) + ":" + raw.substr(11, 2) + ":" + raw.substr(13, 2);
        return result;
    }

    inline int getLevelID(GJGameLevel* level) {
        if (!level) return 0;
        return level->m_levelID.value();
    }

    inline std::string getLevelName(GJGameLevel* level) {
        if (!level) return "Unknown";
        return std::string(level->m_levelName.c_str());
    }

    inline float getPlayerPercent(PlayLayer* pl) {
        if (!pl) return 0.f;
        auto player = pl->m_player1;
        if (!player) return 0.f;
        float percent = (player->getPositionX() / pl->m_levelLength) * 100.f;
        return std::min(percent, 100.f);
    }
}
