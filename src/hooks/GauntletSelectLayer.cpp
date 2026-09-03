#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/GauntletSelectLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "GauntletSelectLayer"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsGauntletSelectLayer, GauntletSelectLayer) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    bool init(int p0) {
        if (!GauntletSelectLayer::init(p0)) return false;

        nodes::placeAd(this, AdType::Banner, Anchor::Bottom, {0.f, 30.f});

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);