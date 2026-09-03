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

        nodes::placeAd(m_mainLayer, AdType::Banner, Anchor::Bottom, {0.f, 65.f});

        if (win::isWide()) {
            nodes::placeAd(m_mainLayer, AdType::Skyscraper, Anchor::Left, {30.f, 0.f});
            nodes::placeAd(m_mainLayer, AdType::Skyscraper, Anchor::Right, {-30.f, 0.f});
        };

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);