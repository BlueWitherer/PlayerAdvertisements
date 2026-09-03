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

#define PLAYERADS_SECRETLAYER_HOOK_BODY(Base)                                     \
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);                                            \
                                                                                  \
    struct Fields final {                                                         \
        asp::Instant lastTime;                                                    \
                                                                                  \
        Advertisement* banner = nullptr;                                          \
        Advertisement* squareLeft = nullptr;                                      \
        Advertisement* squareRight = nullptr;                                     \
    };                                                                            \
                                                                                  \
    bool init() {                                                                 \
        if (!Base::init()) return false;                                          \
                                                                                  \
        auto const winSize = CCDirector::sharedDirector()->getWinSize();          \
                                                                                  \
        auto f = m_fields.self();                                                 \
                                                                                  \
        if (!f->banner) {                                                         \
            f->banner = Advertisement::create(AdType::Banner);                    \
            f->banner->setID("banner"_spr);                                       \
            f->banner->setPosition({winSize.width / 2.f, winSize.height - 30.f}); \
                                                                                  \
            addChild(f->banner, HIGHEST_Z);                                       \
        };                                                                        \
                                                                                  \
        if (!f->squareLeft) {                                                     \
            f->squareLeft = Advertisement::create(AdType::Square);                \
            f->squareLeft->setID("square-left"_spr);                              \
            f->squareLeft->setPosition({75.f, 75.f});                             \
                                                                                  \
            addChild(f->squareLeft, HIGHEST_Z);                                   \
        };                                                                        \
                                                                                  \
        if (!f->squareRight) {                                                    \
            f->squareRight = Advertisement::create(AdType::Square);               \
            f->squareRight->setID("square-right"_spr);                            \
            f->squareRight->setPosition({winSize.width - 75.f, 75.f});            \
                                                                                  \
            addChild(f->squareRight, HIGHEST_Z);                                  \
        };                                                                        \
                                                                                  \
        return true;                                                              \
    };                                                                            \
                                                                                  \
    void reloadAllAds() {                                                         \
        auto f = m_fields.self();                                                 \
                                                                                  \
        if (asp::Instant::now().durationSince(f->lastTime).seconds() < 5) return; \
                                                                                  \
        if (f->banner) f->banner->loadRandom();                                   \
        if (f->squareLeft) f->squareLeft->loadRandom();                           \
        if (f->squareRight) f->squareRight->loadRandom();                         \
                                                                                  \
        log::debug("All ads are now reloading");                                  \
                                                                                  \
        f->lastTime = asp::Instant::now();                                        \
    };                                                                            \
                                                                                  \
    void onSubmit(CCObject* sender) {                                             \
        Base::onSubmit(sender);                                                   \
        reloadAllAds();                                                           \
    }

class $modify(AdsSecretLayer, SecretLayer) {
    PLAYERADS_SECRETLAYER_HOOK_BODY(SecretLayer);
};

class $modify(AdsSecretLayer2, SecretLayer2) {
    PLAYERADS_SECRETLAYER_HOOK_BODY(SecretLayer2);
};

class $modify(AdsSecretLayer4, SecretLayer4) {
    PLAYERADS_SECRETLAYER_HOOK_BODY(SecretLayer4);
};

class $modify(AdsSecretLayer5, SecretLayer5) {
    PLAYERADS_SECRETLAYER_HOOK_BODY(SecretLayer5);
};

// the basement secretlayer
class $modify(AdsSecretLayer3, SecretLayer3) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    bool init() {
        if (!SecretLayer3::init()) return false;

        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        if (auto banner = Advertisement::create(AdType::Banner)) {
            banner->setID("banner"_spr);
            banner->setPosition({winSize.width / 2.f, winSize.height - 30.f});

            addChild(banner, HIGHEST_Z);
        };

        if (auto squareLeft = Advertisement::create(AdType::Square)) {
            squareLeft->setID("square-left"_spr);
            squareLeft->setPosition({75.f, 75.f});

            addChild(squareLeft, HIGHEST_Z);
        };

        if (auto squareRight = Advertisement::create(AdType::Square)) {
            squareRight->setID("square-right"_spr);
            squareRight->setPosition({winSize.width - 75.f, 75.f});

            addChild(squareRight, HIGHEST_Z);
        };

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);