#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

class $modify(AdsPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        if (Mod::get()->getSettingValue<bool>("PauseLayer")) {
            auto levelName = typeinfo_cast<CCLabelBMFont*>(getChildByID("level-name"));
            levelName->setVisible(false);

            auto practiceTitle = typeinfo_cast<CCLabelBMFont*>(getChildByID("practice-mode-label"));
            auto practiceProgress = typeinfo_cast<CCLabelBMFont*>(getChildByID("practice-progress-label"));
            auto practiceBar = typeinfo_cast<CCSprite*>(getChildByID("practice-progress-bar"));

            auto normalTitle = typeinfo_cast<CCLabelBMFont*>(getChildByID("normal-mode-label"));
            auto normalProgress = typeinfo_cast<CCLabelBMFont*>(getChildByID("normal-progress-label"));
            auto normalBar = typeinfo_cast<CCSprite*>(getChildByID("normal-progress-bar"));

            auto playTime = typeinfo_cast<CCLabelBMFont*>(getChildByID("play-time"));
            auto pointslabel = typeinfo_cast<CCLabelBMFont*>(getChildByID("points-label"));

            if (practiceTitle && normalTitle) normalTitle->setPositionY(practiceTitle->getPositionY());
            if (practiceProgress && normalProgress) normalProgress->setPositionY(practiceProgress->getPositionY());
            if (practiceBar && normalBar) normalBar->setPositionY(practiceBar->getPositionY());

            if (practiceTitle) practiceTitle->setVisible(false);
            if (practiceProgress) practiceProgress->setVisible(false);
            if (practiceBar) practiceBar->setVisible(false);

            if (playTime) playTime->setPositionY(playTime->getPositionY() - 30.f);
            if (pointslabel) pointslabel->setPositionY(pointslabel->getPositionY() - 30.f);

            if (GJBaseGameLayer::get()->m_isPracticeMode) {
                if (practiceTitle) practiceTitle->setVisible(true);
                if (practiceProgress) practiceProgress->setVisible(true);
                if (practiceBar) practiceBar->setVisible(true);

                if (normalTitle) normalTitle->setVisible(false);
                if (normalProgress) normalProgress->setVisible(false);
                if (normalBar) normalBar->setVisible(false);
            };

            auto const winSize = CCDirector::get()->getWinSize();

            if (auto adBanner = Advertisement::create(AdType::Banner)) {
                adBanner->setID("banner"_spr);
                adBanner->setPosition({winSize.width / 2.f, winSize.height - 50.f});

                addChild(adBanner, 100);

                adBanner->loadRandom();
            };
        };
    };
};