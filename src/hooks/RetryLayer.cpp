#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/RetryLevelLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "RetryLevelLayer"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsRetryLevelLayer, RetryLevelLayer) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    void customSetup() {
        RetryLevelLayer::customSetup();

        if (auto children = m_mainLayer->getChildren()) {
            unsigned int count = static_cast<unsigned int>(children->count());

            for (unsigned int i = count - 1; i >= 0; --i) {
                if (auto obj = children->objectAtIndex(i)) {
                    if (auto label = typeinfo_cast<CCLabelBMFont*>(obj)) m_mainLayer->removeChild(label, true);
                    if (auto sprite = typeinfo_cast<CCSprite*>(obj)) m_mainLayer->removeChild(sprite, true);
                };
            };
        };

        if (auto adBanner = Advertisement::create(AdType::Square)) {
            auto const winSize = CCDirector::sharedDirector()->getWinSize();

            adBanner->setID("advertisement-menu");
            adBanner->setPosition({winSize.width / 2.f, winSize.height / 2.f});

            m_mainLayer->addChild(adBanner);

            adBanner->loadRandom();
        };
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);