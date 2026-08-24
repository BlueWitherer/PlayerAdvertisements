#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/ProfilePage.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "ProfilePage"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsProfilePage, ProfilePage) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    bool init(int p0, bool p1) {
        if (!ProfilePage::init(p0, p1)) return false;

        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        if (auto adSkyscraperRight = Advertisement::create(AdType::Skyscraper)) {
            adSkyscraperRight->setID("skyscraper-right"_spr);
            adSkyscraperRight->setPosition({winSize.width - 30.f, winSize.height / 2.f});

            m_mainLayer->addChild(adSkyscraperRight, HIGHEST_Z);
        };

        if (auto adSkyscraperLeft = Advertisement::create(AdType::Skyscraper)) {
            adSkyscraperLeft->setID("skyscraper-left"_spr);
            adSkyscraperLeft->setPosition({30.f, winSize.height / 2.f});

            m_mainLayer->addChild(adSkyscraperLeft, HIGHEST_Z);
        };

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);