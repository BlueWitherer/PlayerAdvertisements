#pragma once

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

    class AdsDirector final {
    private:
        asp::SmallVec<Ad, 20> m_seenAds;  // to be used in v1.3
        geode::utils::StringMap<std::vector<std::weak_ptr<geode::Hook>>> m_hooks;

    public:
        static AdsDirector* get() noexcept;

        void registerHooks(std::string id, std::vector<std::weak_ptr<geode::Hook>> hooks);
        void addToViewed(Ad ad);

        std::span<const std::weak_ptr<geode::Hook>> getHooks(std::string_view id) const noexcept;
        std::span<const Ad> getViewedAds() const noexcept;
    };

    namespace hooks {
        void delegateHooks(std::string id, geode::utils::StringMap<std::shared_ptr<geode::Hook>> const& hooks);
        void toggleHooks(std::string_view id, bool on);
    };

    namespace fetch {
        void getLevel(int id, geode::CopyableFunction<void(geode::Result<GJGameLevel*>)>&& callback);
    };
};

template <>
struct matjson::Serialize<cw::ads::Ad> final {
    static geode::Result<cw::ads::Ad> fromJson(matjson::Value const& value);
    static matjson::Value toJson(cw::ads::Ad const& value);
};

#define PLAYERADS_DELEGATE_HOOKS(id)                     \
    static void onModify(auto& self) {                   \
        cw::ads::hooks::delegateHooks(id, self.m_hooks); \
    }

#define PLAYERADS_HOOK_LISTENER(id)                                                    \
    $on_game(ModsLoaded) {                                                             \
        cw::ads::hooks::toggleHooks(id, geode::Mod::get()->getSettingValue<bool>(id)); \
                                                                                       \
        geode::listenForSettingChanges<bool>(                                          \
            id,                                                                        \
            [](bool v) {                                                               \
                cw::ads::hooks::toggleHooks(id, v);                                    \
            });                                                                        \
    };