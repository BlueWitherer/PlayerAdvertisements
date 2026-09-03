#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/CreatorLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "CreatorLayer"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsCreatorLayer, CreatorLayer) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    bool init() {
        if (!CreatorLayer::init()) return false;

        nodes::placeAd(this, AdType::Banner, Anchor::Top, {0.f, -30.f});

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);