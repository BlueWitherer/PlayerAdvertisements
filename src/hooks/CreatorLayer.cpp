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

        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        // banner ad at the top
        if (auto adBanner = Advertisement::create(AdType::Banner)) {
            adBanner->setID("banner"_spr);
            adBanner->setPosition({winSize.width / 2.f, winSize.height - 30.f});

            addChild(adBanner);

            adBanner->loadRandom();
        };

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);