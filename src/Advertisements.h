#pragma once

#include <Geode/Geode.hpp>

namespace cw::ads {
    enum class AdType : uint8_t {
        Banner = 1,
        Square = 2,
        Skyscraper = 3
    };

    struct Ad final {
    private:
        uint64_t m_id = 0;
        std::string m_image;
        int m_level = 0;
        std::string m_user;
        AdType m_type = AdType::Banner;
        uint64_t m_viewCount = 0;
        uint64_t m_clickCount = 0;
        uint8_t m_glowLevel = 0;

    public:
        Ad() = default;

        Ad(
            uint64_t id,
            std::string image,
            int level,
            AdType type,
            std::string user,
            uint64_t viewCount = 0,
            uint64_t clickCount = 0,
            uint8_t glowLevel = 0);

        uint64_t getID() const noexcept;
        geode::ZStringView getImage() const noexcept;
        int getLevel() const noexcept;
        geode::ZStringView getUser() const noexcept;
        AdType getType() const noexcept;
        uint64_t getViews() const noexcept;
        uint64_t getClicks() const noexcept;
        uint8_t getGlowLevel() const noexcept;
    };

    /**
     * Get the size of an ad based on its type
     * @param type The type of ad
     */
    constexpr cocos2d::CCSize getAdSize(AdType type) noexcept;

    /**
     * Get the particle string based on an ad type
     * @param type The type of ad
     */
    constexpr const char* getParticlesForAdType(AdType type) noexcept;

    class Advertisement final : public cocos2d::CCNode {
    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

        void reloadType();
        void reload();

    protected:
        Advertisement();
        ~Advertisement();

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
        geode::LazySprite* getAdSprite() const;

        /**
         * Handle the response from the advertisement fetch request
         * @param res The web response containing the advertisement data
         */
        void handleAdResponse(geode::utils::web::WebResponse const& res);
    };
};

template <>
struct matjson::Serialize<cw::ads::Ad> final {
    static geode::Result<cw::ads::Ad> fromJson(matjson::Value const& value);
    static matjson::Value toJson(cw::ads::Ad const& value);
};