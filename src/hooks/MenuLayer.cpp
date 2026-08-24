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

        async::spawn(
            argon::startAuth(),
            [this](Result<std::string> res) {
                if (res.isOk()) {
                    auto token = std::move(res).unwrap();
                    Mod::get()->setSavedValue<std::string>("authtoken", token);
                } else {
                    log::warn("Auth failed: {}", res.unwrapErr());
                };
            });

        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        if (auto adBanner = Advertisement::create()) {
            adBanner->setID("banner"_spr);

            addChildAtPosition(adBanner, Anchor::Center, {0.f, -70.f}, false);

            adBanner->loadRandom();
        };

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);