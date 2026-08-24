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
};