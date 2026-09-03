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

        nodes::placeAd(this, AdType::Banner, Anchor::Bottom, {0.f, 30.f});

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);