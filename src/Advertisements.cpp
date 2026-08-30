#include <Advertisements.h>

#include "ui/AdPreview.hpp"

#include <argon/argon.hpp>

#include <fmt/core.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cw::ads;

namespace cw::ads {
    namespace particles {
        constexpr auto banner = "50,2065,2,4515,3,855,155,1,156,20,145,20a-1a1a0.3a15a90a0a20a0a100a25a0a25a0a0a0a0a10a5a0a180a1a0a1a0a1a0a1a0a5a0a180a0a1a0a1a0a1a0a1a0a0a1a1a0a0a0a0a0a0a0a0a2a1a0a0a0a41a0a0a0a0a0a0a0a0a0a0a0a0a0a0;";
        constexpr auto square = "50,2065,2,4515,3,855,155,1,156,20,145,20a-1a1a0.3a15a90a0a20a0a50a50a0a25a0a0a0a0a10a5a0a180a1a0a1a0a1a0a1a0a5a0a180a0a1a0a1a0a1a0a1a0a0a1a1a0a0a0a0a0a0a0a0a2a1a0a0a0a41a0a0a0a0a0a0a0a0a0a0a0a0a0a0;";
        constexpr auto skyscraper = "50,2065,2,4515,3,855,155,1,156,20,145,20a-1a1a0.3a15a90a0a20a0a25a100a0a25a0a0a0a0a10a5a0a180a1a0a1a0a1a0a1a0a5a0a180a0a1a0a1a0a1a0a1a0a0a1a1a0a0a0a0a0a0a0a0a2a1a0a0a0a41a0a0a0a0a0a0a0a0a0a0a0a0a0a0;";
    };
};

struct Advertisement::Impl final {
    Ad ad;
    AdType type = AdType::Banner;

    Button* adButton = nullptr;
    Ref<LazySprite> adSprite = nullptr;
    CCSprite* adIcon = nullptr;

    std::string token;

    TaskHolder<web::WebResponse> adListener;

    constexpr auto getAdSize(AdType type) noexcept {
        static constexpr auto banner = CCSize(364.f, 45.f);
        static constexpr auto square = CCSize(122.6f, 122.6f);
        static constexpr auto skyscraper = CCSize(41.f, 314.f);

        CCSize contentSize = banner;

        switch (type) {
            default: [[fallthrough]];

            case AdType::Banner: contentSize = banner; break;
            case AdType::Square: contentSize = square; break;
            case AdType::Skyscraper: contentSize = skyscraper; break;
        };

        return contentSize;
    };

    constexpr auto getParticlesForAdType(AdType type) noexcept {
        switch (type) {
            default: [[fallthrough]];

            case AdType::Banner: return particles::banner;
            case AdType::Square: return particles::square;
            case AdType::Skyscraper: return particles::skyscraper;
        };
    };
};

Advertisement::Advertisement() : m_impl(std::make_unique<Impl>()) {};
Advertisement::~Advertisement() {};

void Advertisement::reload() {
    removeAllChildren();

    setContentSize(m_impl->getAdSize(m_impl->type));

    m_impl->adSprite = LazySprite::create(getScaledContentSize(), Mod::get()->getSettingValue<bool>("loading-circles"));
    if (!m_impl->adSprite) {
        log::error("Failed to create LazySprite");
        return;
    };

    log::debug("Created LazySprite with size: {}x{}", getScaledContentSize().width, getScaledContentSize().height);

    m_impl->adSprite->setPosition({getScaledContentWidth() / 2.f, getScaledContentHeight() / 2.f});
    m_impl->adSprite->setVisible(true);

    log::trace("setting up callbacks");

    m_impl->adSprite->setLoadCallback([this](Result<> res) {
        if (res.isErr()) {
            log::error("Failed to load ad image: {}", res.unwrapErr());

            if (m_impl->adSprite) {
                m_impl->adSprite->setVisible(false);
                m_impl->adSprite->cancelLoad();
            };

            if (m_impl->adButton) m_impl->adButton->setEnabled(false);

            return;
        };

        log::info("Ad image loaded successfully");

        m_impl->adIcon = CCSprite::createWithSpriteFrameName("adIcon.png"_spr);
        m_impl->adIcon->setAnchorPoint({0.f, 0.f});
        m_impl->adIcon->setPosition({3.f, 3.f});
        m_impl->adIcon->setScale(0.25f);
        m_impl->adIcon->setOpacity(125);

        if (m_impl->adButton) {
            m_impl->adButton->addChild(m_impl->adIcon, 9);
            m_impl->adButton->setScaleMultiplier(1.025f);
        };

        if (!m_impl->adSprite) {
            log::warn("Load callback: ad sprite is null");
            return;
        };

        m_impl->adSprite->setAnchorPoint({0.5, 0.5});

        float posx = m_impl->adSprite->getScaledContentWidth() * m_impl->adSprite->getScale() / 2.f;
        float posy = m_impl->adSprite->getScaledContentHeight() * m_impl->adSprite->getScale() / 2.f;

        if (m_impl->adButton) {
            posx = m_impl->adButton->getScaledContentWidth() / 2.f;
            posy = m_impl->adButton->getScaledContentHeight() / 2.f;
        };

        m_impl->adSprite->setPosition({posx, posy});
        m_impl->adSprite->setVisible(true);

        auto const natural = m_impl->adSprite->getContentSize();
        if (natural.width <= 0.f || natural.height <= 0.f) {
            log::warn("Ad sprite has invalid natural size ({}x{})", natural.width, natural.height);
        } else {
            auto target = m_impl->adSprite->getContentSize();
            if (m_impl->adButton) target = m_impl->adButton->getContentSize();

            float sx = target.width / natural.width;
            float sy = target.height / natural.height;

            float scale = std::min(sx, sy);

            m_impl->adSprite->setScale(scale);
            log::debug("Scaled ad sprite by {} to fit target {}x{} (natural {}x{})", scale, target.width, target.height, natural.width, natural.height);
        };

        if (m_impl->ad.getGlowLevel() > 0) {
            auto const size = m_impl->adSprite->getScaledContentSize();

            auto featuredStar = CCSprite::createWithSpriteFrameName("featuredIcon.png"_spr);
            featuredStar->setAnchorPoint({1.f, 0.f});
            featuredStar->setScale(0.35f);

            auto xpos = 3.f;
            if (m_impl->adButton) xpos = m_impl->adButton->getScaledContentWidth() - 3.f;

            featuredStar->setPosition({xpos, 4.25f});
            featuredStar->setOpacity(200);
            featuredStar->setColor({255, 255, 255});

            if (m_impl->adButton) m_impl->adButton->addChild(featuredStar, 9);

            auto glowNode = NineSlice::create("glow.png"_spr);
            glowNode->setContentSize(size);
            glowNode->setAnchorPoint({0.5, 0.5});

            if (m_impl->adButton) glowNode->setPosition(m_impl->adButton->getContentSize() / 2);

            auto particles = GameToolbox::particleFromString(m_impl->getParticlesForAdType(m_impl->ad.getType()), CCParticleSystemQuad::create(), false);
            particles->setScale(1.25f);
            particles->setAnchorPoint({0.5, 0.5});
            particles->setPosition(glowNode->getPosition());
            particles->resetSystem();
            particles->update(0.15f);

            auto tag = Label::create("Featured", "bigFont.fnt");
            tag->setScale(0.375f);
            tag->setAnchorPoint({1, 0});
            tag->setAlignment(Label::Alignment::Right);

            float tagx = 12.f;
            if (m_impl->adButton) tagx = m_impl->adButton->getScaledContentWidth() - 12.f;

            tag->setPosition({tagx, 3.f});
            tag->setOpacity(200);

            if (m_impl->ad.getType() == AdType::Skyscraper) tag->setVisible(false);

            switch (m_impl->ad.getGlowLevel()) {
                case 1: {
                    glowNode->setOpacity(175);
                    glowNode->setColor({250, 250, 75});
                    glowNode->setContentSize({size.width + 6.25f, size.height + 6.25f});
                    particles->setStartColorVar({250, 250, 75, 255});
                    tag->setColor({250, 250, 75});

                    if (featuredStar) featuredStar->setColor({250, 250, 75});
                } break;

                case 2: {
                    glowNode->setOpacity(200);
                    glowNode->setColor({50, 250, 250});
                    glowNode->setContentSize({size.width + 7.5f, size.height + 7.5f});
                    particles->setStartColorVar({50, 250, 250, 255});
                    tag->setColor({50, 250, 250});

                    if (featuredStar) featuredStar->setColor({50, 250, 250});
                } break;

                case 3: {
                    glowNode->setOpacity(250);
                    glowNode->setColor({255, 125, 175});
                    glowNode->setContentSize({size.width + 8.75f, size.height + 8.75f});
                    particles->setStartColorVar({255, 125, 175, 255});
                    tag->setColor({255, 125, 175});

                    if (featuredStar) featuredStar->setColor({255, 125, 175});
                } break;

                default: {
                    cue::resetNode(glowNode);
                    cue::resetNode(particles);
                    cue::resetNode(tag);
                } break;
            };

            if (glowNode) {
                glowNode->setContentSize({glowNode->getScaledContentWidth() * 2.5f, glowNode->getScaledContentHeight() * 2.5f});
                glowNode->setScale(glowNode->getScale() / 2.5f);

                if (m_impl->adButton) m_impl->adButton->addChild(glowNode, -5);

                if (m_impl->ad.getType() != AdType::Skyscraper) {
                    if (particles) {
                        if (m_impl->adButton && m_impl->adButton->getParent()) m_impl->adButton->getParent()->addChild(particles, 2);
                    };
                };

                if (tag && m_impl->adButton) m_impl->adButton->addChild(tag, 9);
            };
        };

        if (auto am = AdsDirector::get()) am->addToViewed(m_impl->ad);
    });

    m_impl->adButton = Button::createWithNode(
        m_impl->adSprite,
        [this](auto) {
            if (!m_impl->ad.getImage().empty()) {
                if (auto popup = AdPreview::create(m_impl->ad)) popup->show();
                return;
            };

            Notification::create("Ad not loaded", NotificationIcon::Error)->show();
        });
    m_impl->adButton->setID("advertisement-btn");

    addChildAtPosition(m_impl->adButton, Anchor::Center);
};

bool Advertisement::init(AdType type) {
    m_impl->type = type;

    if (!CCNode::init()) return false;

    setAnchorPoint({0.5, 0.5});
    setContentSize(m_impl->getAdSize(type));

    return true;
};

void Advertisement::handleAdResponse(web::WebResponse const& res) {
    if (res.error()) return log::error("Failed to fetch ad: HTTP {} ({})", res.code(), res.errorMessage());

    auto jsonRes = res.json();
    if (!jsonRes) {
        log::error("Failed to parse ad json: {}", std::move(jsonRes).unwrapErr());
        cue::resetNode(m_impl->adSprite);

        return;
    };

    auto json = std::move(jsonRes).unwrap();

    auto adRes = json.as<Ad>();
    if (adRes.isErr()) {
        log::error("Failed to parse ad: {}", std::move(adRes).unwrapErr());
        cue::resetNode(m_impl->adSprite);

        return;
    };

    m_impl->ad = std::move(adRes).unwrap();

    log::trace("Sending view tracking request for ad_id={}, user_id={}", m_impl->ad.getID(), m_impl->ad.getUser());

    auto viewRequest = web::WebRequest();
    viewRequest.userAgent("PlayerAdvertisements/1.2");
    viewRequest.timeout(std::chrono::seconds(15));

    matjson::Value viewBody = matjson::Value::object();
    viewBody["ad_id"] = m_impl->ad.getID();
    viewBody["authtoken"] = m_impl->token;
    viewBody["account_id"] = GJAccountManager::sharedState()->m_accountID;

    viewRequest.bodyJSON(viewBody);

    async::spawn(
        viewRequest.post("https://ads.cheeseworks.gay/api/view"),
        [self = WeakRef(this)](web::WebResponse res) {
            if (auto s = self.lock()) {
                if (res.error()) return log::error("View failed with code {} for ad_id={}, user_id={}: {}", res.code(), s->m_impl->ad.getID(), s->m_impl->ad.getUser(), res.errorMessage());

                log::info("View passed ad_id={}, user_id={}", s->m_impl->ad.getID(), s->m_impl->ad.getUser());

                log::debug("View request completed for ad_id={}, user_id={}", s->m_impl->ad.getID(), s->m_impl->ad.getUser());
            };
        });

    log::debug("Sent view tracking request for ad_id={}, user_id={}", m_impl->ad.getID(), m_impl->ad.getUser());

    if (m_impl->adSprite && !m_impl->ad.getImage().empty()) {
        log::debug("Loading ad image from URL: {}", m_impl->ad.getImage());
        m_impl->adSprite->loadFromUrl(utils::string::replace(m_impl->ad.getImage(), "arcticwoof.xyz", "cheeseworks.gay"), CCImage::kFmtUnKnown, true);
    } else if (m_impl->ad.getImage().empty()) {
        log::warn("Ad image URL is empty, skipping image load");
    } else {
        log::warn("Ad sprite missing when trying to load image");
    };
};

void Advertisement::loadRandom() {
    reload();  // refresh any existing nodes :3

    log::trace("Preparing request for random advertisement...");

    auto request = web::WebRequest();
    request.userAgent("PlayerAdvertisements/1.2");
    request.timeout(std::chrono::seconds(15));
    request.param("type", static_cast<uint8_t>(m_impl->type));

    async::spawn(
        request.get("https://ads.cheeseworks.gay/api/ad"),
        [self = WeakRef(this)](web::WebResponse res) {
            if (auto s = self.lock()) s->handleAdResponse(res);
        });

    log::debug("Sent request for random advertisement");
};

void Advertisement::onEnter() {
    CCNode::onEnter();

    if (m_impl->adSprite) m_impl->adSprite->cancelLoad();
    m_impl->adListener.cancel();

    loadRandom();
};

void Advertisement::onExit() {
    if (m_impl->adSprite) m_impl->adSprite->cancelLoad();
    m_impl->adListener.cancel();

    CCNode::onExit();
};

LazySprite* Advertisement::getAdSprite() const noexcept {
    return m_impl->adSprite;
};

Advertisement* Advertisement::create(AdType type) {
    auto ret = new Advertisement();
    if (ret->init(type)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};