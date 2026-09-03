#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/SecretLayer.hpp>
#include <Geode/modify/SecretLayer2.hpp>
#include <Geode/modify/SecretLayer3.hpp>
#include <Geode/modify/SecretLayer4.hpp>
#include <Geode/modify/SecretLayer5.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "SecretLayers"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsSecretLayer, SecretLayer) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        asp::Instant lastTime = asp::Instant::now();

        Advertisement* banner = nullptr;
        Advertisement* squareLeft = nullptr;
        Advertisement* squareRight = nullptr;
    };

    bool init() {
        if (!SecretLayer::init()) return false;

        auto f = m_fields.self();

        if (!f->banner) f->banner = nodes::placeAd(this, AdType::Banner, Anchor::Top, {0.f, -30.f});

        if (!f->squareLeft) f->squareLeft = nodes::placeAd(this, AdType::Square, Anchor::BottomLeft, {75.f, 75.f});
        if (!f->squareRight) f->squareRight = nodes::placeAd(this, AdType::Square, Anchor::BottomRight, {-75.f, 75.f});

        return true;
    };

    void reloadAllAds() {
        auto f = m_fields.self();

        if (asp::Instant::now().durationSince(f->lastTime).seconds() < 5) return;

        if (f->banner) f->banner->loadRandom();
        if (f->squareLeft) f->squareLeft->loadRandom();
        if (f->squareRight) f->squareRight->loadRandom();

        log::debug("All ads are now reloading");

        f->lastTime = asp::Instant::now();
    };

    void onSubmit(CCObject* sender) {
        SecretLayer::onSubmit(sender);
        reloadAllAds();
    };
};

class $modify(AdsSecretLayer4, SecretLayer4) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        asp::Instant lastTime = asp::Instant::now();

        Advertisement* banner = nullptr;
        Advertisement* squareLeft = nullptr;
        Advertisement* squareRight = nullptr;
    };

    bool init() {
        if (!SecretLayer4::init()) return false;

        auto f = m_fields.self();

        if (!f->banner) f->banner = nodes::placeAd(this, AdType::Banner, Anchor::Top, {0.f, -30.f});

        if (!f->squareLeft) f->squareLeft = nodes::placeAd(this, AdType::Square, Anchor::BottomLeft, {75.f, 75.f});
        if (!f->squareRight) f->squareRight = nodes::placeAd(this, AdType::Square, Anchor::BottomRight, {-75.f, 75.f});

        return true;
    };

    void reloadAllAds() {
        auto f = m_fields.self();

        if (asp::Instant::now().durationSince(f->lastTime).seconds() < 5) return;

        if (f->banner) f->banner->loadRandom();
        if (f->squareLeft) f->squareLeft->loadRandom();
        if (f->squareRight) f->squareRight->loadRandom();

        log::debug("All ads are now reloading");

        f->lastTime = asp::Instant::now();
    };

    void onSubmit(CCObject* sender) {
        SecretLayer4::onSubmit(sender);
        reloadAllAds();
    };
};

class $modify(AdsSecretLayer5, SecretLayer5) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        asp::Instant lastTime = asp::Instant::now();

        Advertisement* banner = nullptr;
        Advertisement* squareLeft = nullptr;
        Advertisement* squareRight = nullptr;
    };

    bool init() {
        if (!SecretLayer5::init()) return false;

        auto f = m_fields.self();

        if (!f->banner) f->banner = nodes::placeAd(this, AdType::Banner, Anchor::Top, {0.f, -30.f});

        if (!f->squareLeft) f->squareLeft = nodes::placeAd(this, AdType::Square, Anchor::BottomLeft, {75.f, 75.f});
        if (!f->squareRight) f->squareRight = nodes::placeAd(this, AdType::Square, Anchor::BottomRight, {-75.f, 75.f});

        return true;
    };

    void reloadAllAds() {
        auto f = m_fields.self();

        if (asp::Instant::now().durationSince(f->lastTime).seconds() < 5) return;

        if (f->banner) f->banner->loadRandom();
        if (f->squareLeft) f->squareLeft->loadRandom();
        if (f->squareRight) f->squareRight->loadRandom();

        log::debug("All ads are now reloading");

        f->lastTime = asp::Instant::now();
    };

    void onSubmit(CCObject* sender) {
        SecretLayer5::onSubmit(sender);
        reloadAllAds();
    };
};

// has stupid fucking buttons in the way of the right side square ad
class $modify(AdsSecretLayer2, SecretLayer2) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        asp::Instant lastTime = asp::Instant::now();

        Advertisement* banner = nullptr;
    };

    bool init() {
        if (!SecretLayer2::init()) return false;

        auto f = m_fields.self();

        if (!f->banner) f->banner = nodes::placeAd(this, AdType::Banner, Anchor::Top, {0.f, -30.f});

        return true;
    };

    void reloadAllAds() {
        auto f = m_fields.self();

        if (asp::Instant::now().durationSince(f->lastTime).seconds() < 5) return;

        if (f->banner) f->banner->loadRandom();

        log::debug("All ads are now reloading");

        f->lastTime = asp::Instant::now();
    };

    void onSubmit(CCObject* sender) {
        SecretLayer2::onSubmit(sender);
        reloadAllAds();
    };
};

// the basement secretlayer
class $modify(AdsSecretLayer3, SecretLayer3) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    bool init() {
        if (!SecretLayer3::init()) return false;

        nodes::placeAd(this, AdType::Banner, Anchor::Top, {0.f, -30.f});

        nodes::placeAd(this, AdType::Square, Anchor::BottomLeft, {75.f, 75.f});
        nodes::placeAd(this, AdType::Square, Anchor::BottomRight, {-75.f, 75.f});

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);