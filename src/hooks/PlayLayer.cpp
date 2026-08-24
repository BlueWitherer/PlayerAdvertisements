#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "PlayLayer"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsPlayLayer, PlayLayer) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    struct Fields {
        Advertisement* bannerTop = nullptr;
        Advertisement* bannerBottom = nullptr;
        Advertisement* skyscraperRight = nullptr;
        Advertisement* skyscraperLeft = nullptr;
    };

    void setupHasCompleted() {
        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        auto f = m_fields.self();

        if (!f->bannerTop) {
            f->bannerTop = Advertisement::create(AdType::Banner);
            f->bannerTop->setID("banner-top"_spr);
            f->bannerTop->setPosition({winSize.width / 2.f, winSize.height - 30.f});

            m_uiLayer->addChild(f->bannerTop, HIGHEST_Z);
        };

        if (!f->bannerBottom) {
            f->bannerBottom = Advertisement::create(AdType::Banner);
            f->bannerBottom->setID("banner-bottom"_spr);
            f->bannerBottom->setPosition({winSize.width / 2.f, 30.f});

            m_uiLayer->addChild(f->bannerBottom, HIGHEST_Z);
        };

        if (!f->skyscraperRight) {
            f->skyscraperRight = Advertisement::create(AdType::Skyscraper);
            f->skyscraperRight->setID("skyscraper-right"_spr);
            f->skyscraperRight->setPosition({winSize.width - 30.f, winSize.height / 2.f});

            m_uiLayer->addChild(f->skyscraperRight, HIGHEST_Z);
        };

        if (!f->skyscraperLeft) {
            f->skyscraperLeft = Advertisement::create(AdType::Skyscraper);
            f->skyscraperLeft->setID("skyscraper-left"_spr);
            f->skyscraperLeft->setPosition({30.f, winSize.height / 2.f});

            m_uiLayer->addChild(f->skyscraperLeft, HIGHEST_Z);
        };

        log::trace("setting up scheduler for auto ad refresh");
        schedule(schedule_selector(AdsPlayLayer::schedReload), 12.5f);

        PlayLayer::setupHasCompleted();
    };

    void fullReset() {
        reloadAllAds();
        PlayLayer::fullReset();
    };

    void resetLevelFromStart() {
        reloadAllAds();
        PlayLayer::resetLevelFromStart();
    };

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        PlayLayer::destroyPlayer(player, object);
        if (player->m_isDead) reloadAllAds();
    };

    void reloadAllAds() {
        auto f = m_fields.self();

        if (f->bannerTop) f->bannerTop->loadRandom();
        if (f->bannerBottom) f->bannerBottom->loadRandom();
        if (f->skyscraperRight) f->skyscraperRight->loadRandom();
        if (f->skyscraperLeft) f->skyscraperLeft->loadRandom();

        log::debug("All ads are now reloading");
    };

    void schedReload(float dt) {
        log::debug("reloading ads after {}s...", dt);
        reloadAllAds();
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);