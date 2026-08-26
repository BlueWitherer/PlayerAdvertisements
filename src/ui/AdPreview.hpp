#pragma once

#include <Advertisements.h>

#include <Geode/Geode.hpp>

namespace cw::ads {
    class AdPreviewStat final : public cocos2d::CCNode {
    protected:
        bool init(geode::ZStringView sprite, std::string label);

    public:
        static AdPreviewStat* create(geode::ZStringView sprite, std::string label);
    };

    class AdPreview final : public geode::Popup {
        struct LinkButton final {
            std::string id;
            std::string sprite;
            geode::Button::ButtonCallback callback = nullptr;
        };

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

        void setupMetaLabels(AdLevelMetadata const& data);

    protected:
        AdPreview();
        ~AdPreview();

        bool init(Ad ad);

        void onPlayButton(cocos2d::CCObject* sender);

        void switchToLevel();
        void registerClick();

    public:
        static AdPreview* create(Ad ad);
    };
};