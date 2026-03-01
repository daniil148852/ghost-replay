#include "GhostPlayer.hpp"
#include <Geode/modify/PlayerObject.hpp>

// Глобальный набор указателей на PlayerObject-призраков
static std::unordered_set<PlayerObject*> s_ghostPlayers;

void GhostPlayer::markAsGhost(PlayerObject* player) {
    if (player) {
        s_ghostPlayers.insert(player);
    }
}

bool GhostPlayer::isGhost(PlayerObject* player) {
    return s_ghostPlayers.count(player) > 0;
}

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
    setupTrail();
    playSpawnEffect();
    
    return true;
}

void GhostPlayer::setupVisuals() {
    m_simplePlayer = SimplePlayer::create(m_record.meta.iconID);
    if (!m_simplePlayer) {
        // Fallback
        m_mainSprite = CCSprite::create("player_01_001.png");
        if (m_mainSprite) {
            m_mainSprite->setOpacity(static_cast<GLubyte>(m_opacity));
            this->addChild(m_mainSprite);
        }
        return;
    }
    
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
    m_simplePlayer->setScale(0.85f);
    
    this->addChild(m_simplePlayer, 1);
    
    log::info("Ghost setup: icon={}, colors=({},{}), glow={}", 
        m_record.meta.iconID, m_record.meta.color1, 
        m_record.meta.color2, m_record.meta.hasGlow);
}

void GhostPlayer::setupTrail() {
    auto gm = GameManager::sharedState();
    ccColor3B col1 = gm->colorForIdx(m_record.meta.color1);
    
    // Лёгкий трейл за призраком
    m_trail = CCMotionStreak::create(
        0.3f,   // fade time
        3.0f,   // min segment size
        8.0f,   // stroke width
        ccc3(col1.r, col1.g, col1.b),
        "streak.png"
    );
    
    if (m_trail) {
        m_trail->setOpacity(static_cast<GLubyte>(m_opacity * 0.5f));
        m_trail->setBlendFunc({GL_SRC_ALPHA, GL_ONE}); // Аддитивное смешивание
        this->addChild(m_trail, 0);
    }
}

void GhostPlayer::updateGameMode(uint8_t gameMode) {
    if (gameMode == m_lastGameMode) return;
    m_lastGameMode = gameMode;
    
    if (!m_simplePlayer) return;
    
    switch (gameMode) {
        case 0:
            m_simplePlayer->updatePlayerFrame(m_record.meta.iconID, IconType::Cube);
            break;
        case 1:
            m_simplePlayer->updatePlayerFrame(m_record.meta.shipID, IconType::Ship);
            break;
        case 2:
            m_simplePlayer->updatePlayerFrame(m_record.meta.ballID, IconType::Ball);
            break;
        case 3:
            m_simplePlayer->updatePlayerFrame(m_record.meta.ufoID, IconType::Ufo);
            break;
        case 4:
            m_simplePlayer->updatePlayerFrame(m_record.meta.waveID, IconType::Wave);
            break;
        case 5:
            m_simplePlayer->updatePlayerFrame(m_record.meta.robotID, IconType::Robot);
            break;
        case 6:
            m_simplePlayer->updatePlayerFrame(m_record.meta.spiderID, IconType::Spider);
            break;
        case 7:
            m_simplePlayer->updatePlayerFrame(m_record.meta.swingID, IconType::Swing);
            break;
    }
}

void GhostPlayer::applyFrameData(const FrameData& frame) {
    this->setPosition(ccp(frame.posX, frame.posY));
    this->setRotation(frame.rotation);
    
    if (frame.playerScale != 0.f) {
        float baseScale = 0.85f;
        float scale = frame.playerScale * baseScale;
        this->setScaleX(scale);
        
        if (frame.isUpsideDown) {
            this->setScaleY(-std::abs(scale));
        } else {
            this->setScaleY(std::abs(scale));
        }
    }
    
    this->setVisible(frame.isVisible && m_visible);
    
    updateGameMode(frame.gameMode);
    
    // Обновляем позицию трейла
    if (m_trail) {
        m_trail->setPosition(this->getPosition());
    }
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
            playDeathEffect();
            this->setVisible(false);
            return;
        }
    }
    
    FrameData frame = m_record.getInterpolatedFrame(
        static_cast<float>(m_currentFrame)
    );
    
    if (frame.isDead) {
        m_dead = true;
        bool loop = Mod::get()->getSettingValue<bool>("loop-ghosts");
        if (!loop) {
            playDeathEffect();
            this->setVisible(false);
            return;
        } else {
            m_dead = false;
            m_currentFrame = 0;
            frame = m_record.getFrameAt(0);
        }
    }
    
    applyFrameData(frame);
}

void GhostPlayer::playDeathEffect() {
    // Эффект исчезновения — fade out + scale
    if (m_simplePlayer) {
        auto fadeOut = CCFadeOut::create(0.3f);
        auto scaleUp = CCScaleTo::create(0.3f, 1.5f);
        auto spawn = CCSpawn::create(fadeOut, scaleUp, nullptr);
        m_simplePlayer->runAction(spawn);
    }
    
    // FIX: Добавлен второй аргумент false, так как в Android байндингах GD 2.2 сигнатура требует его
    auto particles = CCParticleSystemQuad::create("explodeEffect.plist", false);
    if (particles) {
        particles->setPosition(this->getPosition());
        particles->setAutoRemoveOnFinish(true);
        particles->setScale(0.5f);
        
        auto gm = GameManager::sharedState();
        ccColor3B col = gm->colorForIdx(m_record.meta.color1);
        particles->setStartColor(ccc4FFromccc3B(col));
        
        if (this->getParent()) {
            this->getParent()->addChild(particles, 100);
        }
    }
}

void GhostPlayer::playSpawnEffect() {
    // Fade in при появлении
    if (m_simplePlayer) {
        m_simplePlayer->setOpacity(0);
        auto fadeIn = CCFadeTo::create(0.5f, static_cast<GLubyte>(m_opacity));
        m_simplePlayer->runAction(fadeIn);
    }
    
    // Небольшой scale-эффект
    this->setScale(0.1f);
    auto scaleTo = CCEaseBackOut::create(
        CCScaleTo::create(0.4f, 0.85f)
    );
    this->runAction(scaleTo);
}

void GhostPlayer::show() {
    m_visible = true;
    this->setVisible(true);
    if (m_simplePlayer) {
        m_simplePlayer->setOpacity(static_cast<GLubyte>(m_opacity));
    }
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
    
    // Сбрасываем визуал
    this->stopAllActions();
    if (m_simplePlayer) {
        m_simplePlayer->stopAllActions();
        m_simplePlayer->setOpacity(static_cast<GLubyte>(m_opacity));
        m_simplePlayer->setScale(0.85f);
    }
    
    // Сбрасываем трейл
    if (m_trail) {
        m_trail->reset();
    }
    
    show();
    playSpawnEffect();
}

void GhostPlayer::updateColors() {
    if (!m_simplePlayer) return;
    
    auto gm = GameManager::sharedState();
    ccColor3B col1 = gm->colorForIdx(m_record.meta.color1);
    ccColor3B col2 = gm->colorForIdx(m_record.meta.color2);
    
    m_simplePlayer->setColor(col1);
    m_simplePlayer->setSecondColor(col2);
}
