#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJDropDownLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "GJDropDownLayer"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsGJDropDownLayer, GJDropDownLayer) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    bool init(const char* p0, float p1, bool p2) {
        if (!GJDropDownLayer::init(p0, p1, p2)) return false;

        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        if (auto adBanner = Advertisement::create(AdType::Banner)) {
            adBanner->setID("banner"_spr);
            adBanner->setPosition({winSize.width / 2.f, winSize.height - 30.f});

            m_mainLayer->addChild(adBanner, HIGHEST_Z);
        };

        if (!m_mainLayer->getChildByID("hide-dropdown-menu")) {
            if (auto adSkyscraperLeft = Advertisement::create(AdType::Skyscraper)) {
                adSkyscraperLeft->setID("advertisement-menu-skyscraper-left");
                adSkyscraperLeft->setPosition({30.f, winSize.height / 2.f});

                m_mainLayer->addChild(adSkyscraperLeft, HIGHEST_Z);
            };

            if (auto adSkyscraperRight = Advertisement::create(AdType::Skyscraper)) {
                adSkyscraperRight->setID("skyscraper-right"_spr);
                adSkyscraperRight->setPosition({winSize.width - 30.f, winSize.height / 2.f});

                m_mainLayer->addChild(adSkyscraperRight, HIGHEST_Z);
            };
        };

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);