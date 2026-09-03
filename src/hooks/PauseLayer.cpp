#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "PauseLayer"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsPauseLayer, PauseLayer) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    void customSetup() {
        PauseLayer::customSetup();

        auto levelName = static_cast<CCLabelBMFont*>(getChildByID("level-name"));
        levelName->setVisible(false);

        auto normalTitle = static_cast<CCLabelBMFont*>(getChildByID("normal-mode-label"));
        auto normalProgress = static_cast<CCLabelBMFont*>(getChildByID("normal-progress-label"));
        auto normalBar = static_cast<CCSprite*>(getChildByID("normal-progress-bar"));

        auto practiceTitle = static_cast<CCLabelBMFont*>(getChildByID("practice-mode-label"));
        auto practiceProgress = static_cast<CCLabelBMFont*>(getChildByID("practice-progress-label"));
        auto practiceBar = static_cast<CCSprite*>(getChildByID("practice-progress-bar"));

        auto playTime = static_cast<CCLabelBMFont*>(getChildByID("play-time"));
        auto pointslabel = static_cast<CCLabelBMFont*>(getChildByID("points-label"));

        auto isPractice = PlayLayer::get()->m_isPracticeMode;

        if (practiceTitle && normalTitle) normalTitle->setPositionY(practiceTitle->getPositionY());
        if (practiceProgress && normalProgress) normalProgress->setPositionY(practiceProgress->getPositionY());
        if (practiceBar && normalBar) normalBar->setPositionY(practiceBar->getPositionY());

        if (normalTitle) normalTitle->setVisible(!isPractice);
        if (normalProgress) normalProgress->setVisible(!isPractice);
        if (normalBar) normalBar->setVisible(!isPractice);

        if (practiceTitle) practiceTitle->setVisible(isPractice);
        if (practiceProgress) practiceProgress->setVisible(isPractice);
        if (practiceBar) practiceBar->setVisible(isPractice);

        if (playTime) playTime->setPositionY(playTime->getPositionY() - 30.f);
        if (pointslabel) pointslabel->setPositionY(pointslabel->getPositionY() - 30.f);

        nodes::placeAd(this, AdType::Banner, Anchor::Top, {0.f, -65.f});
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);