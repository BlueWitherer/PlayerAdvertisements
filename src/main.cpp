#include <AdsUtils.h>

#include <Advertisements.h>

#include <ui/AdsDashboard.hpp>

#include <argon/argon.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

namespace cw::ads {
    namespace ui {
        static void addAdButton(CCNode* menu, bool alt = true) {
            auto btn = CCMenuItemExt::createSpriteExtra(
                CircleButtonSprite::createWithSpriteFrameName(
                    "adIcon.png"_spr,
                    0.875f,
                    CircleBaseColor::Green,
                    alt ? CircleBaseSize::MediumAlt : CircleBaseSize::Medium),
                [](auto) {
                    pushSceneWithLayer(AdsDashboard::create());
                });
            btn->setID("ads-viewer-btn"_spr);

            menu->addChild(btn);
            menu->updateLayout();
        };
    };
};

$on_game(ModsLoaded) {
    async::spawn(
        fetch::baseRequest().get("https://ads.cheeseworks.gay/api/badges"),
        [](web::WebResponse res) {
            if (auto ads = AdsDirector::get()) {
                if (res.error()) return log::error("{}", res.errorMessage());

                auto jsonRes = res.json();
                if (jsonRes.isErr()) return log::error("{}", std::move(jsonRes).unwrapErr());

                auto json = std::move(jsonRes).unwrap();

                auto arrayRes = std::move(json).asArray();
                if (arrayRes.isErr()) return log::error("{}", std::move(arrayRes).unwrapErr());

                auto array = std::move(arrayRes).unwrap();

                for (auto& i : array) {
                    auto badgeRes = std::move(i).as<AdBadge>();
                    if (badgeRes.isErr()) return log::error("{}", std::move(badgeRes).unwrapErr());

                    auto badge = std::move(badgeRes).unwrap();

                    auto user = badge.user;
                    ads->saveBadge(user, std::move(badge));
                };
            };
        });
};

class $modify(PAHookMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        if (auto menu = getChildByID("bottom-menu")) ui::addAdButton(menu);

        if (Mod::get()->hasSavedValue("authtoken")) Mod::get()->getSaveContainer().erase("authtoken");  // this was never used lol

        return true;
    };
};

class $modify(PAHookLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        if (auto menu = getChildByID("left-side-menu")) ui::addAdButton(menu, false);

        return true;
    };
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

Result<AdBadge> matjson::Serialize<cw::ads::AdBadge>::fromJson(matjson::Value const& value) {
    if (!value.isObject()) return Err("Expected an object");

    GEODE_UNWRAP_INTO(int user, value["user"].asInt());
    GEODE_UNWRAP_INTO(bool owner, value["owner"].asBool());
    GEODE_UNWRAP_INTO(bool dev, value["dev"].asBool());
    GEODE_UNWRAP_INTO(bool admin, value["admin"].asBool());
    GEODE_UNWRAP_INTO(bool staff, value["staff"].asBool());
    GEODE_UNWRAP_INTO(bool verified, value["verified"].asBool());
    GEODE_UNWRAP_INTO(bool contributor, value["contributor"].asBool());
    GEODE_UNWRAP_INTO(std::string discord, value["discord"].asString());

    return Ok(
        AdBadge{
            user,
            owner,
            dev,
            admin,
            staff,
            verified,
            contributor,
            std::move(discord),
        });
};

matjson::Value matjson::Serialize<cw::ads::AdBadge>::toJson(AdBadge const& value) {
    auto obj = matjson::Value();
    obj["user"] = value.user;
    obj["owner"] = value.owner;
    obj["dev"] = value.dev;
    obj["admin"] = value.admin;
    obj["staff"] = value.staff;
    obj["verified"] = value.verified;
    obj["contributor"] = value.contributor;
    obj["discord"] = value.discord;

    return obj;
};

void AdsDirector::registerHooks(std::string id, std::vector<std::weak_ptr<Hook>> hooks) {
    m_hooks[std::move(id)] = std::move(hooks);
};

void AdsDirector::addLevelToCache(GJGameLevel* level) {
    m_seenLevels[level->m_levelID.value()] = AdLevelMetadata{level->m_levelName, level->m_levelID.value(), fetch::getDiffSpriteNum(level), fetch::getRating(level)};
};

void AdsDirector::addToViewed(Ad ad) {
    if (m_seenAds.size() >= 25) m_seenAds.pop_back();
    m_seenAds.insert(m_seenAds.begin(), std::move(ad));
};

void AdsDirector::saveBadge(int id, AdBadge badge) {
    m_badges[id] = std::move(badge);
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

Result<AdBadge> AdsDirector::getBadge(int id) {
    if (auto it = m_badges.find(id); it != m_badges.end()) return Ok(it->second);
    return Err("Badge not found");
};

AdsDirector* AdsDirector::get() noexcept {
    static AdsDirector inst;
    return &inst;
};

bool ui::AdsTabSprite::init(ZStringView iconFrame, std::string text, float width, bool altColor) {
    if (!CCNode::init()) return false;

    CCSize const itemSize = {width, 35.f};
    CCSize const iconSize = {18.f, 18.f};

    setContentSize(itemSize);
    setAnchorPoint({.5f, .5f});

    m_deselectedBG = NineSlice::createWithSpriteFrameName("tab-bg.png"_spr);
    m_deselectedBG->setScale(.8f);
    m_deselectedBG->setContentSize(itemSize / .8f);
    m_deselectedBG->setColor({54, 31, 16});

    addChildAtPosition(m_deselectedBG, Anchor::Center);

    m_selectedBG = NineSlice::createWithSpriteFrameName("tab-bg.png"_spr);
    m_selectedBG->setScale(.8f);
    m_selectedBG->setContentSize(itemSize / .8f);
    m_selectedBG->setColor(altColor ? ccColor3B{147, 163, 185} : ccColor3B{168, 147, 185});

    addChildAtPosition(m_selectedBG, Anchor::Center);

    m_icon = CCSprite::createWithSpriteFrameName(iconFrame.c_str());
    limitNodeSize(m_icon, iconSize, 3.f, .1f);

    addChildAtPosition(m_icon, Anchor::Left, {16.f, 0.f}, false);

    m_label = Label::create(std::move(text), "bigFont.fnt");
    m_label->setLimitLabelWidth(getScaledContentWidth() - 45.f);

    addChildAtPosition(m_label, Anchor::Left, {(itemSize.width - iconSize.width) / 2.f + iconSize.width, 0.f}, false);

    return true;
};

ui::AdsTabSprite* ui::AdsTabSprite::create(ZStringView iconFrame, std::string text, float width, bool altColor) {
    auto ret = new ui::AdsTabSprite();
    if (ret->init(iconFrame, std::move(text), width, altColor)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};

void ui::AdsTabSprite::select(bool selected) {
    m_deselectedBG->setVisible(!selected);
    m_selectedBG->setVisible(selected);
};

void ui::AdsTabSprite::disable(bool disabled) {
    auto color = disabled ? ccc3(95, 95, 95) : ccc3(255, 255, 255);

    m_deselectedBG->setColor(color);
    m_selectedBG->setColor(color);

    m_icon->setColor(color);
    m_label->setColor(color);
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
                lvl->m_creatorName = std::string{creatorData[1]};
                lvl->setAccountID(numFromString<int>(creatorData[2]).unwrapOrDefault());

                cb(Ok(lvl));
            };
        });
};

void fetch::getBadge(int id, CopyableFunction<void(Result<AdBadge>)>&& callback, bool local) {
    if (auto ads = AdsDirector::get()) {
        auto res = ads->getBadge(id);
        if (res.isOk()) return callback(std::move(res));
    };

    if (local) return callback(Err("Badge not found"));

    auto req = baseRequest()
                   .param("id", id);

    async::spawn(
        req.get("https://ads.cheeseworks.gay/api/badge"),
        [cb = std::move(callback)](web::WebResponse res) {
            auto const fallback = [&cb](std::string err) {
                log::error("{}", err);
                return cb(Err(std::move(err)));
            };

            if (res.error()) return fallback(std::string{res.errorMessage()});

            auto jsonRes = res.json();
            if (jsonRes.isErr()) return fallback(std::move(jsonRes).unwrapErr());

            auto json = std::move(jsonRes).unwrap();

            auto badgeRes = std::move(json).as<AdBadge>();
            if (badgeRes.isErr()) return fallback(std::move(badgeRes).unwrapErr());

            return cb(Ok(std::move(badgeRes).unwrap()));
        });
};