#include <AdsUtils.h>
#include <Advertisements.h>

#include <argon/argon.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cw::ads;

$on_mod(Loaded) {
    async::spawn(
        argon::startAuth(),
        [](Result<std::string> res) {
            if (res.isOk()) {
                auto token = std::move(res).unwrap();
                Mod::get()->setSavedValue<std::string>("authtoken", token);
            } else {
                log::warn("Auth failed: {}", std::move(res).unwrapErr());
            };
        });
};

Result<Ad> matjson::Serialize<Ad>::fromJson(matjson::Value const& value) {
    if (!value.isObject()) return Err("Expected an object");

    GEODE_UNWRAP_INTO(uint64_t id, value["ad_id"].asUInt());
    GEODE_UNWRAP_INTO(std::string image, value["image_url"].asString());
    GEODE_UNWRAP_INTO(int level, value["level_id"].asInt());
    GEODE_UNWRAP_INTO(uint8_t type, value["type"].asUInt());
    GEODE_UNWRAP_INTO(std::string user, value["user_id"].asString());
    GEODE_UNWRAP_INTO(uint64_t viewCount, value["views"].asUInt());
    GEODE_UNWRAP_INTO(uint64_t clickCount, value["clicks"].asUInt());

    uint8_t glow = 0;
    GEODE_UNWRAP_INTO_IF_OK(glow, value["glow"].asUInt());

    return Ok(
        Ad{
            id,
            std::move(image),
            level,
            static_cast<AdType>(type),
            std::move(user),
            viewCount,
            clickCount,
            glow,
        });
};

matjson::Value matjson::Serialize<Ad>::toJson(Ad const& value) {
    auto obj = matjson::Value();
    obj["ad_id"] = value.getID();
    obj["image_url"] = value.getImage();
    obj["level_id"] = value.getLevel();
    obj["type"] = static_cast<uint8_t>(value.getType());
    obj["user_id"] = value.getUser();
    obj["views"] = value.getViews();
    obj["clicks"] = value.getClicks();
    obj["glow"] = value.getGlowLevel();

    return obj;
};

Ad::Ad(
    uint64_t id,
    std::string image,
    int level,
    AdType type,
    std::string user,
    uint64_t viewCount,
    uint64_t clickCount,
    uint8_t glowLevel) :
    m_id(id),
    m_image(std::move(image)),
    m_level(level),
    m_type(type),
    m_user(std::move(user)),
    m_viewCount(viewCount),
    m_clickCount(clickCount),
    m_glowLevel(glowLevel) {};

uint64_t Ad::getID() const noexcept {
    return m_id;
};

ZStringView Ad::getImage() const noexcept {
    return m_image;
};

int Ad::getLevel() const noexcept {
    return m_level;
};

AdType Ad::getType() const noexcept {
    return m_type;
};

ZStringView Ad::getUser() const noexcept {
    return m_user;
};

uint64_t Ad::getViews() const noexcept {
    return m_viewCount;
};

uint64_t Ad::getClicks() const noexcept {
    return m_clickCount;
};

uint8_t Ad::getGlowLevel() const noexcept {
    return m_glowLevel;
};

void AdsDirector::registerHooks(std::string id, std::vector<std::weak_ptr<Hook>> hooks) {
    m_hooks[std::move(id)] = std::move(hooks);
};

void AdsDirector::addLevelToCache(GJGameLevel* level) {
    m_seenLevels[level->m_levelID.value()] = AdLevelMetadata{level->m_levelName, level->m_levelID.value(), fetch::getDiffSpriteNum(level), fetch::getRating(level)};
};

void AdsDirector::addToViewed(Ad ad) {
    if (m_seenAds.size() >= 30) m_seenAds.erase(m_seenAds.end());
    m_seenAds.push_back(std::move(ad));
};

std::span<const std::weak_ptr<Hook>> AdsDirector::getHooks(std::string_view id) const noexcept {
    if (auto it = m_hooks.find(id); it != m_hooks.end()) return it->second;
    return {};
};

Result<AdLevelMetadata> AdsDirector::getLevelMeta(int id) const {
    if (auto it = m_seenLevels.find(id); it != m_seenLevels.end()) return Ok(it->second);
    return Err("Level not in cache");
};

std::span<const Ad> AdsDirector::getViewedAds() const noexcept {
    return m_seenAds;
};

AdsDirector* AdsDirector::get() noexcept {
    static AdsDirector inst;
    return &inst;
};

void hooks::delegateHooks(std::string id, utils::StringMap<std::shared_ptr<Hook>> const& hooks) {
    if (auto ads = AdsDirector::get()) {
        std::vector<std::weak_ptr<Hook>> out;
        out.reserve(hooks.size());

        for (auto const& hook : hooks) out.push_back(hook.second);

        ads->registerHooks(std::move(id), std::move(out));
    };
};

void hooks::toggleHooks(std::string_view id, bool on) {
    if (auto ads = AdsDirector::get()) {
        for (auto const& hook : ads->getHooks(id)) {
            if (auto h = hook.lock()) (void)h->toggle(on);
        };
    };
};

void fetch::getLevel(int id, CopyableFunction<void(Result<GJGameLevel*>)>&& callback, bool download, GJGameLevel* data) {
    if (auto glm = GameLevelManager::sharedState()) {
        if (auto level = glm->getSavedLevel(id)) return callback(Ok(level));
    };

    auto reqForm = fmt::format("secret=Wmfd2893gb7&{}={}", download ? "levelID" : "type=0&str", id);

    if (argon::signedIn()) {
        auto const auth = argon::getGameAccountData();
        reqForm = fmt::format("{}&accountID={}&gjp2={}", reqForm, auth.accountId, auth.gjp2);
    };

    auto req = web::WebRequest()
                   .bodyString(reqForm)
                   .userAgent("");

    // this might be a little bit cursed owo
    async::spawn(
        req.post(fmt::format("https://www.boomlings.com/database/{}.php", download ? "downloadGJLevel22" : "getGJLevels21")),
        [cb = std::move(callback), id, download, data](web::WebResponse res) {
            if (res.error()) {
                log::error("Error getting user information: {}", res.errorMessage());
                return cb(Err("An error occurred while fetching user information"));
            };

            auto strRes = res.string();
            if (strRes.isErr()) return cb(Err(fmt::format("An error occurred while processing user information: {}", std::move(strRes).unwrapErr())));

            auto const str = std::move(strRes).unwrap();
            if (str == "-1") return cb(Err("Boomlings request returned an unknown error"));

            auto const parts = asp::iter::split(str, "#").collect();
            if (parts.empty()) return cb(Err("Malformed Boomlings response"));

            if (download) {
                if (!data) return cb(Err("Missing level data"));
                if (data->m_levelID != id) return cb(Err("Requested ID and level ID do not match"));

                auto const& levelStr = parts[0];
                auto dict = Ref(CCDictionary::create());

                auto const kv = asp::iter::split(levelStr, ":").collect();
                for (size_t i = 0; i + 1 < kv.size(); i += 2) dict->setObject(CCString::create(std::string{kv[i + 1]}), std::string{kv[i]});

                data->m_levelString = dict->charForKey(numToString(4));

                cb(Ok(data));
            } else {
                auto levelsRaw = asp::iter::split(parts[0], "|").collect();
                if (levelsRaw.empty()) return cb(Err("No levels returned"));

                auto creatorsRaw = asp::iter::split(parts[1], "|").collect();
                if (creatorsRaw.empty()) return cb(Err("No creators returned"));

                auto const& levelStr = levelsRaw[0];
                auto const kvLvl = asp::iter::split(levelStr, ":").collect();

                auto const& creatorStr = creatorsRaw[0];
                auto const creatorData = asp::iter::split(creatorStr, ":").collect();

                auto dict = CCDictionary::create();
                for (size_t i = 0; i + 1 < kvLvl.size(); i += 2) dict->setObject(CCString::create(std::string{kvLvl[i + 1]}), std::string{kvLvl[i]});

                auto lvl = GJGameLevel::create(dict, download);  // siiiiigh
                lvl->m_creatorName = creatorData[1];
                lvl->setAccountID(numFromString<int>(creatorData[2]).unwrapOrDefault());

                cb(Ok(lvl));
            };
        });
};