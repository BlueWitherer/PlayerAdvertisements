#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/ChallengesPage.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "ChallengesPage"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsChallengesPage, ChallengesPage) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    bool init() {
        if (!ChallengesPage::init()) return false;

        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        if (auto adBanner = Advertisement::create(AdType::Banner)) {
            adBanner->setID("banner"_spr);
            adBanner->setPosition({winSize.width / 2.f, 30.f});

            m_mainLayer->addChild(adBanner, HIGHEST_Z);
        };

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);