#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJPathsLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "GJPathsLayer"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsGJPathsLayer, GJPathsLayer) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    bool init() {
        if (!GJPathsLayer::init()) return false;

        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        if (auto adBannerLeft = Advertisement::create(AdType::Skyscraper)) {
            adBannerLeft->setID("banner-left"_spr);
            adBannerLeft->setPosition({30.f, winSize.height / 2.f});

            addChild(adBannerLeft, HIGHEST_Z);
        };

        if (auto adBannerRight = Advertisement::create(AdType::Skyscraper)) {
            adBannerRight->setID("banner-right"_spr);
            adBannerRight->setPosition({winSize.width - 30.f, winSize.height / 2.f});

            addChild(adBannerRight, HIGHEST_Z);
        };

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);