#include <AdsUtils.h>

#include <Advertisements.h>

#include <argon/argon.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "MenuLayer"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsMenuLayer, MenuLayer) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    bool init() {
        if (!MenuLayer::init()) return false;

        nodes::placeAd(this, AdType::Banner, Anchor::Bottom, {0.f, 92.5f});

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);