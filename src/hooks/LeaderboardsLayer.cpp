#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/LeaderboardsLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "LeaderboardsLayer"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsLeaderboardsLayer, LeaderboardsLayer) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    bool init(LeaderboardType type, LeaderboardStat stat) {
        if (!LeaderboardsLayer::init(type, stat)) return false;

        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        if (auto adBanner = Advertisement::create(AdType::Banner)) {
            adBanner->setID("advertisement-leaderboards-bottom");
            adBanner->setPosition({winSize.width / 2.f, 30.f});

            addChild(adBanner, HIGHEST_Z);
        };

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);