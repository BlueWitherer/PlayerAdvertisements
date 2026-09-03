#pragma once

#include "AdsUtils.h"

#include <Geode/Geode.hpp>

namespace cw::ads {
    class Advertisement final : public cocos2d::CCNode {
    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

        void reload();

    protected:
        Advertisement();
        ~Advertisement();

        void onEnter() override;
        void onExit() override;

        bool init(AdType type);

    public:
        /**
         * Create a new advertisement
         * @param type The type of ad to create
         */
        static Advertisement* create(AdType type = AdType::Banner);

        /**
         * Load a random advertisement
         */
        void loadRandom();

        /**
         * Get the LazySprite associated with the advertisement
         */
        geode::LazySprite* getAdSprite() const noexcept;

        /**
         * Handle the response from the advertisement fetch request
         * @param res The web response containing the advertisement data
         */
        void handleAdResponse(geode::utils::web::WebResponse const& res);
    };

    namespace nodes {
        inline constexpr auto getIDForType(AdType type) noexcept {
            switch (type) {
                default: [[fallthrough]];

                case AdType::Banner: return "banner";
                case AdType::Square: return "square";
                case AdType::Skyscraper: return "skyscraper";
            };
        };

        inline constexpr auto getIDForAnchor(geode::Anchor anchor) noexcept {
            using namespace geode;

            switch (anchor) {
                default: [[fallthrough]];

                case Anchor::TopLeft: return "top-left";
                case Anchor::Top: return "top";
                case Anchor::TopRight: return "top-right";
                case Anchor::Left: return "left";
                case Anchor::Center: return "center";
                case Anchor::Right: return "right";
                case Anchor::BottomLeft: return "bottom-left";
                case Anchor::Bottom: return "bottom";
                case Anchor::BottomRight: return "bottom-right";
            };
        };

        inline auto formatIDForAd(AdType type, geode::Anchor anchor) {
            return fmt::format("{}-{}"_spr, getIDForType(type), getIDForAnchor(anchor));
        };

        Advertisement* placeAd(cocos2d::CCNode* to, std::optional<std::string> id = std::nullopt, AdType type = AdType::Banner, geode::Anchor anchor = geode::Anchor::Center, cocos2d::CCPoint const& offset = {});
        Advertisement* placeAd(cocos2d::CCNode* to, AdType type = AdType::Banner, geode::Anchor anchor = geode::Anchor::Center, cocos2d::CCPoint const& offset = {});
    };
};