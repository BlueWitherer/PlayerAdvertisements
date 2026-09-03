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

    struct Fields final {
        asp::Instant lastTime;

        Advertisement* bannerTop = nullptr;
        Advertisement* bannerBottom = nullptr;
        Advertisement* skyscraperRight = nullptr;
        Advertisement* skyscraperLeft = nullptr;
    };

    void setupHasCompleted() {
        auto f = m_fields.self();

        f->bannerBottom = nodes::placeAd(m_uiLayer, AdType::Banner, Anchor::Bottom, {0.f, 30.f});
        f->bannerBottom = nodes::placeAd(m_uiLayer, AdType::Banner, Anchor::Bottom, {0.f, 30.f});
        f->skyscraperLeft = nodes::placeAd(m_uiLayer, AdType::Skyscraper, Anchor::Left, {30.f, 0.f});
        f->skyscraperRight = nodes::placeAd(m_uiLayer, AdType::Skyscraper, Anchor::Right, {-30.f, 0.f});

        log::trace("setting up scheduler for auto ad refresh");
        scheduleOnce(schedule_selector(AdsPlayLayer::schedReload), rng::generate(2.5f, 12.5f));

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

        if (asp::Instant::now().durationSince(f->lastTime).seconds() < 12.5f) return;

        if (f->bannerTop) f->bannerTop->loadRandom();
        if (f->bannerBottom) f->bannerBottom->loadRandom();
        if (f->skyscraperRight) f->skyscraperRight->loadRandom();
        if (f->skyscraperLeft) f->skyscraperLeft->loadRandom();

        log::debug("All ads are now reloading");

        f->lastTime = asp::Instant::now();
    };

    void schedReload(float dt) {
        log::debug("reloading ads after {}s...", dt);
        reloadAllAds();

        scheduleOnce(schedule_selector(AdsPlayLayer::schedReload), rng::generate(2.5f, 12.5f));
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);