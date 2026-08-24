#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/InfoLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "InfoLayer"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsInfoLayer, InfoLayer) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    bool init(GJGameLevel* p0, GJUserScore* p1, GJLevelList* p2) {
        if (!InfoLayer::init(p0, p1, p2)) return false;

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