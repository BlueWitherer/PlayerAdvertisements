#pragma once

#include <Advertisements.h>

#include <Geode/Geode.hpp>

namespace cw::ads {

    class AdPreview final : public geode::Popup {
    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

    protected:
        AdPreview();
        ~AdPreview();

        bool init(cw::ads::Ad ad);

        void onPlayButton(cocos2d::CCObject* sender);

        void tryOpenOrFetchLevel(CCMenuItemSpriteExtra* menuItem, int levelId);
        void registerClick();

        void update(float dt) override;

    public:
        static AdPreview* create(cw::ads::Ad ad);
    };
};