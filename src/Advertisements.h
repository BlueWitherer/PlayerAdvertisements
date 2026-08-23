#pragma once

#include "AdsUtils.h"

#include <Geode/Geode.hpp>

namespace cw::ads {
    class Advertisement final : public cocos2d::CCNode {
    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

        void reloadType();
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
         * Set the expected type of advertisement
         * @param type The type of ad to set
         */
        void setType(AdType type);

        /**
         * Load a random advertisement
         */
        void loadRandom();

        /**
         * Load a specific advertisement by its ID
         * @param id The ID of the ad to load
         * @warning This will override the current set type of ad
         */
        void load(int id);

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