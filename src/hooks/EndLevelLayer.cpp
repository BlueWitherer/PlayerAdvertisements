#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/EndLevelLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "EndLevelLayer"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsEndLevelLayer, EndLevelLayer) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    void customSetup() {
        EndLevelLayer::customSetup();

        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        if (auto adBanner = Advertisement::create(AdType::Banner)) {
            adBanner->setID("banner"_spr);
            adBanner->setPosition({winSize.width / 2.f, winSize.height - 30.f});

            m_mainLayer->addChild(adBanner, HIGHEST_Z);
        };

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
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);