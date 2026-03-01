#include "GhostPlayer.hpp"

GhostPlayer* GhostPlayer::create(const RecordData& record, PlayLayer* playLayer) {
    auto ret = new GhostPlayer();
    if (ret && ret->init(record, playLayer)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool GhostPlayer::init(const RecordData& record, PlayLayer* playLayer) {
    if (!CCNode::init()) return false;
    
    m_record = record;
    m_playLayer = playLayer;
    m_opacity = static_cast<float>(Mod::get()->getSettingValue<int64_t>("ghost-opacity"));
    
    setupVisuals();
    
    return true;
}

void GhostPlayer::setupVisuals() {
    // Создаём SimplePlayer для корректного отображения иконки
    m_simplePlayer = SimplePlayer::create(m_record.meta.iconID);
    if (!m_simplePlayer) {
        // Fallback — простой спрайт
        m_mainSprite = CCSprite::create("player_01_001.png");
        if (m_mainSprite) {
            m_mainSprite->setOpacity(static_cast<GLubyte>(m_opacity));
            this->addChild(m_mainSprite);
        }
        return;
    }
    
    // Устанавливаем цвета
    auto gm = GameManager::sharedState();
    
    ccColor3B col1 = gm->colorForIdx(m_record.meta.color1);
    ccColor3B col2 = gm->colorForIdx(m_record.meta.color2);
    
    m_simplePlayer->setColor(col1);
    m_simplePlayer->setSecondColor(col2);
    
    if (m_record.meta.hasGlow) {
        m_simplePlayer->setGlowOutline(gm->colorForIdx(m_record.meta.glowColor));
    } else {
        m_simplePlayer->disableGlowOutline();
    }
    
    m_simplePlayer->setOpacity(static_cast<GLubyte>(m_opacity));
    m_simplePlayer->setScale(0.9f); // немного меньше для различения
    
    this->addChild(m_simplePlayer);
    
    log::info("Ghost visuals setup: icon={}, c1={}, c2={}", 
        m_record.meta.iconID, m_record.meta.color1, m_record.meta.color2);
}

void GhostPlayer::updateGameMode(uint8_t gameMode) {
    if (gameMode == m_lastGameMode) return;
    m_lastGameMode = gameMode;
    
    if (!m_simplePlayer) return;
    
    int iconID = m_record.meta.iconID;
    
    switch (gameMode) {
        case 0: // Cube
            m_simplePlayer->updatePlayerFrame(m_record.meta.iconID, IconType::Cube);
            break;
        case 1: // Ship
            m_simplePlayer->updatePlayerFrame(m_record.meta.shipID, IconType::Ship);
            break;
        case 2: // Ball
            m_simplePlayer->updatePlayerFrame(m_record.meta.ballID, IconType::Ball);
            break;
        case 3: // UFO
            m_simplePlayer->updatePlayerFrame(m_record.meta.ufoID, IconType::Ufo);
            break;
        case 4: // Wave
            m_simplePlayer->updatePlayerFrame(m_record.meta.waveID, IconType::Wave);
            break;
        case 5: // Robot
            m_simplePlayer->updatePlayerFrame(m_record.meta.robotID, IconType::Robot);
            break;
        case 6: // Spider
            m_simplePlayer->updatePlayerFrame(m_record.meta.spiderID, IconType::Spider);
            break;
        case 7: // Swing
            m_simplePlayer->updatePlayerFrame(m_record.meta.swingID, IconType::Swing);
            break;
    }
}

void GhostPlayer::applyFrameData(const FrameData& frame) {
    this->setPosition(ccp(frame.posX, frame.posY));
    this->setRotation(frame.rotation);
    
    if (frame.playerScale != 0.f) {
        float baseScale = 0.9f;
        this->setScale(frame.playerScale * baseScale);
    }
    
    // Гравитация (переворот)
    if (frame.isUpsideDown) {
        this->setScaleY(-std::abs(this->getScaleY()));
    } else {
        this->setScaleY(std::abs(this->getScaleY()));
    }
    
    // Видимость
    this->setVisible(frame.isVisible && m_visible);
    
    // Обновление режима игры
    updateGameMode(frame.gameMode);
}

void GhostPlayer::updateGhost(uint32_t currentFrame) {
    if (m_finished || m_dead) return;
    if (m_record.isEmpty()) return;
    
    m_currentFrame = currentFrame;
    
    // Проверяем выход за пределы записи
    if (currentFrame >= m_record.meta.totalFrames) {
        bool loop = Mod::get()->getSettingValue<bool>("loop-ghosts");
        if (loop) {
            m_currentFrame = currentFrame % m_record.meta.totalFrames;
        } else {
            m_finished = true;
            this->setVisible(false);
            return;
        }
    }
    
    // Получаем интерполированный кадр
    FrameData frame = m_record.getInterpolatedFrame(static_cast<float>(m_currentFrame));
    
    // Проверяем смерть
    if (frame.isDead) {
        m_dead = true;
        bool loop = Mod::get()->getSettingValue<bool>("loop-ghosts");
        if (!loop) {
            this->setVisible(false);
            return;
        } else {
            // При зацикливании - сбрасываем
            m_dead = false;
            m_currentFrame = 0;
            frame = m_record.getFrameAt(0);
        }
    }
    
    applyFrameData(frame);
}

void GhostPlayer::show() {
    m_visible = true;
    this->setVisible(true);
}

void GhostPlayer::hide() {
    m_visible = false;
    this->setVisible(false);
}

void GhostPlayer::reset() {
    m_currentFrame = 0;
    m_finished = false;
    m_dead = false;
    m_lastGameMode = 255;
    show();
}

void GhostPlayer::updateColors() {
    if (!m_simplePlayer) return;
    
    auto gm = GameManager::sharedState();
    ccColor3B col1 = gm->colorForIdx(m_record.meta.color1);
    ccColor3B col2 = gm->colorForIdx(m_record.meta.color2);
    
    m_simplePlayer->setColor(col1);
    m_simplePlayer->setSecondColor(col2);
}
