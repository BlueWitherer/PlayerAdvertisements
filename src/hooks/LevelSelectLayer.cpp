#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/LevelSelectLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "LevelSelectLayer"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsLevelSelectLayer, LevelSelectLayer) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    bool init(int page) {
        if (!LevelSelectLayer::init(page)) return false;

        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        if (auto adBanner = Advertisement::create(AdType::Banner)) {
            adBanner->setID("banner"_spr);
            adBanner->setPosition({winSize.width / 2.f, winSize.height - 30.f});

            addChild(adBanner, HIGHEST_Z);
        };

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);