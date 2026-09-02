#include "../AdsViewer.hpp"

#include <AdsUtils.h>

#include <cue/RepeatingBackground.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cw::ads;

bool AdsViewer::init() {
    if (!CCLayer::init()) return false;

    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    addBackButton(this);

    auto const winSize = CCDirector::sharedDirector()->getWinSize();

    auto bg = cue::RepeatingBackground::create("game_bg_11_001.png", 0.875f, cue::RepeatMode::X, winSize);
    bg->setColor({44, 49, 83});
    bg->setSpeed(1.25f);

    addChild(bg, -9);

    auto title = Button::createWithLabel(
        "Player Ads",
        "goldFont.fnt",
        [](auto) {
            openInfoPopup(Mod::get());
        });
    title->setID("view-mod-info-btn");

    addChildAtPosition(title, Anchor::Top, {0.f, -1.f * (title->getScaledContentHeight() * 0.875f)}, false);

    auto version = Label::create(Mod::get()->getVersion().toVString(), "chatFont.fnt");
    version->setID("version-label");
    version->setScale(0.5f);
    version->setAlignment(Label::Alignment::Center);
    version->setOpacity(100);

    addChildAtPosition(version, Anchor::Bottom, {0.f, 17.5f}, false);

    auto btnContainerLayout = ColumnLayout::create()
                                  ->setGap(5.f)
                                  ->setAutoScale(false)
                                  ->setAutoGrowAxis(1.25f)
                                  ->setAxisReverse(true);

    auto btnContainer = CCNode::create();
    btnContainer->setID("social-button-menu");
    btnContainer->setAnchorPoint({1, 0});
    btnContainer->setLayout(btnContainerLayout);

    addChildAtPosition(btnContainer, Anchor::BottomRight, {-12.5f, 12.5f}, false);

    auto btns = std::to_array<LinkButton>(
        {
            {
                "kofi-btn",
                "btn_kofi.png"_spr,
                [](auto) {
                    createQuickPopup(
                        "Ko-fi",
                        "Would you like to <cd>support the mod through Ko-fi</c>?\n"
                        "<cy>Earn cool perks like more views on your ads!</c>",
                        "Cancel",
                        "OK",
                        [](auto, bool ok) {
                            if (ok) web::openLinkInBrowser("https://ko-fi.com/playerads");
                        });
                },
            },
            {
                "discord-btn",
                "gj_discordIcon_001.png",
                [](auto) {
                    createQuickPopup(
                        "Community Discord",
                        "Join the <cd>Cheeseworks</c> <cb>community Discord server</c>?\n"
                        "<cs>Get help, report bugs, and chat with other players!</c>",
                        "Cancel",
                        "OK",
                        [](auto, bool ok) {
                            if (ok) web::openLinkInBrowser("https://www.dsc.gg/cheeseworks");
                        });
                },
            },
            {
                "ads-dashboard-btn",
                "btn_dashboard.png"_spr,
                [](auto) {
                    createQuickPopup(
                        "Ads Manager",
                        "Go to the <co>Player Ads Manager</c> dashboard?\n"
                        "<cc>Upload ads for your levels here!</c>",
                        "Cancel",
                        "OK",
                        [](auto, bool ok) {
                            if (ok) web::openLinkInBrowser("https://ads.cheeseworks.gay/");
                        });
                },
            },
        });

    for (auto& b : btns) {
        auto btn = Button::createWithSpriteFrameName(
            b.sprite,
            std::move(b.callback));
        btn->setID(std::move(b.id));

        btnContainer->addChild(btn);
    };

    btnContainer->updateLayout();

    auto menuContainer = NineSlice::create("geode.loader/GE_square03.png");
    menuContainer->setID("menu-container");
    menuContainer->setContentSize({430.f, 240.f});

    addChildAtPosition(menuContainer, Anchor::Center, {0.f, -8.75f}, false);

    auto recentAdsLabel = Label::create("Recently Viewed Ads", "bigFont.fnt");
    recentAdsLabel->setID("recent-ads-label");
    recentAdsLabel->setScale(0.425f);
    recentAdsLabel->setAlignment(Label::Alignment::Center);

    menuContainer->addChildAtPosition(recentAdsLabel, Anchor::Top, {0.f, -15.f});

    auto infoBtn = Button::createWithSpriteFrameName(
        "GJ_infoIcon_001.png",
        [](auto) {
            createQuickPopup(
                "Help",
                "This is the <cg>advertisement viewer</c>. You can see <cc>a list of ads you were exposed to recently</c> during your current session, and <cy>play the levels from those ads again</c>!",
                "OK",
                nullptr,
                nullptr);
        });
    infoBtn->setID("info-btn");
    infoBtn->setScale(0.875f);

    addChildAtPosition(infoBtn, Anchor::TopRight, {-17.5f, -17.5f}, false);

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