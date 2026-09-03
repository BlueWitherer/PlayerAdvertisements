#include "../AdsViewer.hpp"

#include <ui/AdPreview.hpp>

#include <AdsUtils.h>

#include <cue/RepeatingBackground.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cw::ads;

namespace cw::ads {
    namespace str {
        constexpr auto getStringForAdType(AdType type) {
            switch (type) {
                default: return "Unknown";

                case AdType::Banner: return "Banner";
                case AdType::Skyscraper: return "Skyscraper";
                case AdType::Square: return "Square";
            };
        };
    };
};

bool AdsViewerCell::init(Ad ad, float width) {
    m_ad = std::move(ad);

    if (!CCNode::init()) return false;

    setContentSize({width, 75.f});

    cue::attachBackground(
        this,
        {
            .opacity = 255,
            .sidePadding = 0.f,
            .verticalPadding = 0.f,
            .texture = "geode.loader/GE_square02.png",
            .id = "",
        });

    auto title = Label::create(fmt::format("Ad #{}", m_ad.getID()), "goldFont.fnt");
    title->setID("ad-title-label");
    title->setScale(0.375f);
    title->setAlignment(Label::Alignment::Center);

    addChildAtPosition(title, Anchor::Top, {0.f, -10.f});

    auto imgContainer = NineSlice::create("geode.loader/black-square.png");
    imgContainer->setID("ad-thumbnail-container");
    imgContainer->setAnchorPoint({0, 0.5});
    imgContainer->setContentSize({112.5f, 60.f});

    addChildAtPosition(imgContainer, Anchor::Left, {12.5f, 0.f});

    auto border = cue::createBackground(
        imgContainer->getScaledContentSize(),
        {
            .opacity = 255,
            .cornerRoundness = -0.75f,
            .texture = "GJ_square07.png",
            .id = "",
        });
    border->setPosition(imgContainer->getPosition());
    border->setAnchorPoint(imgContainer->getAnchorPoint());

    addChild(border, 9);

    auto imgClipper = CCClippingNode::create(imgContainer);
    imgClipper->setContentSize(imgContainer->getScaledContentSize());
    imgClipper->setAlphaThreshold(0.f);

    addChild(imgClipper, 1);

    auto img = LazySprite::create(imgClipper->getScaledContentSize());
    img->setID("ad-image");
    img->setAnchorPoint({0.5, 0.5});

    img->setLoadCallback([type = m_ad.getType(), img, imgClipper](Result<> res) {
        if (res.isErr()) {
            log::error("Failed to load ad thumbnail: {}", std::move(res).unwrapErr());
            return;
        };

        type == AdType::Skyscraper ? img->setScale(imgClipper->getScaledContentHeight() * 2.f / img->getScaledContentHeight()) : img->setScale(imgClipper->getScaledContentWidth() * 1.125f / img->getScaledContentWidth());
    });

    imgClipper->addChildAtPosition(img, Anchor::Center, {imgContainer->getPositionX(), 7.5f}, false);
    img->loadFromUrl(m_ad.getImage());

    auto viewCountLabel = Label::create(
        fmt::format("{} View{}", GameToolbox::pointsToString(m_ad.getViews()), m_ad.getViews() != 1 ? "s" : ""),  // this is so dumb
        "geode.loader/mdFont.fnt");
    viewCountLabel->setID("view-count-label");
    viewCountLabel->setScale(0.5f);
    viewCountLabel->setColor({255, 193, 136});
    viewCountLabel->setAlignment(Label::Alignment::Center);

    addChildAtPosition(viewCountLabel, Anchor::Center, {0.f, 5.f});

    auto clickCountLabel = Label::create(
        fmt::format("{} Click{}", GameToolbox::pointsToString(m_ad.getClicks()), m_ad.getClicks() != 1 ? "s" : ""),  // this is still very dumb
        "geode.loader/mdFontB.fnt");
    clickCountLabel->setID("click-count-label");
    clickCountLabel->setScale(0.425f);
    clickCountLabel->setColor({104, 173, 209});
    clickCountLabel->setAlignment(Label::Alignment::Center);

    addChildAtPosition(clickCountLabel, Anchor::Center, {0.f, -5.f});

    auto adTypeLabel = Label::create(str::getStringForAdType(m_ad.getType()), "chatFont.fnt");
    adTypeLabel->setID("ad-type-label");
    adTypeLabel->setScale(0.5f);
    adTypeLabel->setOpacity(125);
    adTypeLabel->setAlignment(Label::Alignment::Center);

    addChildAtPosition(adTypeLabel, Anchor::Bottom, {0.f, 10.f});

    auto viewBtn = Button::createWithNode(
        ButtonSprite::create(
            "View Ad",
            "goldFont.fnt",
            "GJ_button_05.png"),
        [this](auto) {
            if (auto preview = AdPreview::create(m_ad, false)) preview->show();
        });
    viewBtn->setID("view-ad-btn");
    viewBtn->setScale(0.875f);

    addChildAtPosition(viewBtn, Anchor::Right, {-1.f * (viewBtn->getScaledContentWidth() * 0.75f), 0.f});

    return true;
};

AdsViewerCell* AdsViewerCell::create(Ad ad, float width) {
    auto ret = new AdsViewerCell();
    if (ret->init(std::move(ad), width)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};

bool AdsViewer::init() {
    if (!CCLayer::init()) return false;

    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    addBackButton(this, BackButtonStyle::Blue);

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
                "trailer-btn",
                "gj_ytIcon_001.png",
                [](auto) {
                    createQuickPopup(
                        "Trailer",
                        "Would you like to <cr>watch the Player Advertisements trailer on YouTube</c>?",
                        "Cancel",
                        "OK",
                        [](auto, bool ok) {
                            if (ok) web::openLinkInBrowser("https://www.youtube.com/watch?v=P-IZ6ZJzTNI&list=PLdNiksXmbukw");
                        });
                },
            },
            {
                "discord-btn",
                "gj_discordIcon_001.png",
                [](auto) {
                    createQuickPopup(
                        "Discord Community",
                        "Join <cd>Cheeseworks</c>'s <cb>Discord server</c>?\n"
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

    auto announcementBtnSprite = CircleButtonSprite::createWithSpriteFrameName(
        "geode.loader/news.png",
        0.75f,
        CircleBaseColor::DarkAqua);

    auto announcementBtnLoading = LoadingSpinner::create(announcementBtnSprite->getScaledContentHeight() * 0.5f);
    announcementBtnLoading->setVisible(false);

    auto announcementBtn = Button::createWithNode(
        announcementBtnSprite,
        [announcementBtnLoading](Button* sender) {
            announcementBtnLoading->setVisible(true);
            sender->setVisible(false);

            auto req = web::WebRequest()
                           .userAgent("PlayerAdvertisements/1.2")
                           .timeout(std::chrono::seconds(15));

            async::spawn(
                req.get("https://ads.cheeseworks.gay/api/announcement"),
                [btn = WeakRef(sender), btnLoad = WeakRef(announcementBtnLoading)](web::WebResponse res) {
                    if (auto b = btn.lock()) {
                        if (res.error()) {
                            log::error("Failed to get announcement ({}): {}", res.code(), res.errorMessage());
                            Notification::create("Failed to get announcement", NotificationIcon::Error)->show();

                            b->setVisible(true);
                            if (auto bLoad = btnLoad.lock()) bLoad->setVisible(false);

                            return;
                        };

                        auto jsonRes = res.json();
                        if (jsonRes.isErr()) return log::error("Failed to parse announcement JSON: {}", std::move(jsonRes).unwrapErr());

                        auto const json = std::move(jsonRes).unwrap();

                        auto titleRes = json["title"].asString();
                        if (titleRes.isErr()) return log::error("Failed to parse announcement title JSON: {}", std::move(titleRes).unwrapErr());

                        auto contentRes = json["content"].asString();
                        if (contentRes.isErr()) return log::error("Failed to parse announcement content JSON: {}", std::move(contentRes).unwrapErr());

                        auto const title = std::move(titleRes).unwrap();
                        auto const content = std::move(contentRes).unwrap();

                        b->setVisible(true);
                        if (auto bLoad = btnLoad.lock()) bLoad->setVisible(false);

                        if (auto popup = MDPopup::create(title, content, "Close")) popup->show();
                    };
                });
        });
    announcementBtn->setID("latest-announcement-btn");
    announcementBtn->setScale(0.875f);

    addChildAtPosition(announcementBtn, Anchor::BottomLeft, {32.5f, 32.5f}, false);

    announcementBtnLoading->setPosition(announcementBtn->getPosition());

    addChild(announcementBtnLoading, 1);

    static constexpr auto maxContainerWidth = 425.f;
    static constexpr auto minSidesGap = 50.f;

    auto containerWidth = std::min(maxContainerWidth, winSize.width - minSidesGap * 2.f);

    auto menuContainer = NineSlice::create("geode.loader/GE_square03.png");
    menuContainer->setID("menu-container");
    menuContainer->setContentSize({containerWidth, 240.f});

    addChildAtPosition(menuContainer, Anchor::Center, {0.f, -8.75f}, false);

    auto recentAdsLabel = Label::create("Recently Viewed Ads", "bigFont.fnt");
    recentAdsLabel->setID("recent-ads-label");
    recentAdsLabel->setScale(0.425f);
    recentAdsLabel->setAlignment(Label::Alignment::Center);

    menuContainer->addChildAtPosition(recentAdsLabel, Anchor::Top, {0.f, -15.f});

    auto adList = ScrollLayer::create({containerWidth - 37.5f, 175.f});
    adList->setID("ad-list");
    adList->setZOrder(1);
    adList->setAnchorPoint({0.5f, 0.5f});
    adList->ignoreAnchorPointForPosition(false);

    adList->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout());

    menuContainer->addChildAtPosition(adList, Anchor::Center, {0.f, -12.5f});

    auto adListBg = cue::createBackground(
        {adList->getScaledContentWidth() + 12.5f, adList->getScaledContentHeight() + 15.f},
        {
            .cornerRoundness = -0.75f,
            .zOrder = 0,
            .id = "",
        });
    adListBg->setPosition(adList->getPosition());

    menuContainer->addChild(adListBg);

    if (auto ads = AdsDirector::get()) {
        auto countLabel = Label::create(fmt::format("{} Ads", ads->getViewedAds().size()), "chatFont.fnt");
        countLabel->setID("ad-count-label");
        countLabel->setScale(0.625f);
        countLabel->setOpacity(200);
        countLabel->setAlignment(Label::Alignment::Center);

        menuContainer->addChildAtPosition(countLabel, Anchor::Top, {0.f, -26.5f}, false);

        auto recentAds = ads->getViewedAds();

        for (auto const& ad : recentAds) {
            auto cell = AdsViewerCell::create(ad, adList->getScaledContentWidth());
            adList->m_contentLayer->addChild(cell);
        };

        adList->m_contentLayer->updateLayout();
    };

    adList->scrollToTop();

    auto infoBtn = Button::createWithSpriteFrameName(
        "GJ_infoIcon_001.png",
        [](auto) {
            createQuickPopup(
                "Help",
                "This is the <cg>advertisement viewer</c>. You can see <cc>a list of up to 25 ads you were exposed to most recently</c> during your current session, and <cy>play the levels from those ads again</c>!",
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