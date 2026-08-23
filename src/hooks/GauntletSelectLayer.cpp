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

        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        // banner at the bottom center
        if (auto adBanner = Advertisement::create(AdType::Banner)) {
            adBanner->setID("banner"_spr);
            adBanner->setPosition({winSize.width / 2.f, 30.f});

            addChild(adBanner, 1);

            adBanner->loadRandom();
        };

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);