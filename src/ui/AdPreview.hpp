#pragma once

#include <Advertisements.h>

#include <Geode/Geode.hpp>
#include "Geode/cocos/menu_nodes/CCMenuItem.h"

using namespace geode::prelude;
using namespace cw::ads;

class AdPreview final : public Popup {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
    CCMenuItemSprite* m_playAdLevelBtn;

protected:
    AdPreview();
    ~AdPreview();

    bool init(uint64_t adId, int levelId, std::string userId, AdType type, uint64_t viewCount, uint64_t clickCount);

    void onPlayButton(CCObject* sender);

    void registerClick(uint64_t adId, std::string_view userId);
    void tryOpenOrFetchLevel(CCMenuItemSpriteExtra* menuItem, int levelId);

    void update(float dt) override;

public:
    static AdPreview* create(uint64_t adId, int levelId, std::string userId, AdType type, uint64_t viewCount, uint64_t clickCount);
};