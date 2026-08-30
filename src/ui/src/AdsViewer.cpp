#include "../AdsViewer.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cw::ads;

bool AdsViewer::init() {
    if (!CCLayer::init()) return false;

    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    addChild(createLayerBG(), -9);

    addBackButton(this);

    auto const winSize = CCDirector::sharedDirector()->getWinSize();

    auto title = Label::create("Recent Advertisements", "bigFont.fnt");
    title->setID("title");
    title->setScale(0.875f);
    title->setAlignment(Label::Alignment::Center);

    addChildAtPosition(title, Anchor::Top, {0.f, -1.f * (title->getScaledContentHeight() * 0.875f)}, false);

    return true;
};

void AdsViewer::keyBackClicked() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
};

AdsViewer* AdsViewer::create() {
    auto ret = new AdsViewer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};