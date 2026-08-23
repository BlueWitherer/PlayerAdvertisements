#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/DailyLevelPage.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "DailyLevelPage"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsDailyLevelPage, DailyLevelPage) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    bool init(GJTimedLevelType levelType) {
        if (!DailyLevelPage::init(levelType)) return false;

        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        // banner ad at the top
        if (auto adBanner = Advertisement::create(AdType::Banner)) {
            adBanner->setID("banner"_spr);
            adBanner->setPosition({winSize.width / 2.f, 70.f});

            m_mainLayer->addChild(adBanner, 8);

            adBanner->loadRandom();
        };

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);