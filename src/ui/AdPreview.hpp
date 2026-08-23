#pragma once

#include <Advertisements.h>

#include <Geode/Geode.hpp>

namespace cw::ads {
    class AdPreview final : public geode::Popup {
        struct LinkButton final {
            std::string id;
            std::string sprite;
            geode::Button::ButtonCallback callback = nullptr;
        };

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

    protected:
        AdPreview();
        ~AdPreview();

        bool init(Ad ad);

        void onPlayButton(cocos2d::CCObject* sender);

        void tryOpenOrFetchLevel(CCMenuItemSpriteExtra* menuItem, int levelId);
        void registerClick();

        void update(float dt) override;

    public:
        static AdPreview* create(Ad ad);
    };
};