#include "../AdPreview.hpp"

#include "../ReportPopup.hpp"

#include <Advertisements.h>

#include <argon/argon.hpp>

#include <Geode/Geode.hpp>

#include <Geode/ui/Button.hpp>

#include <Geode/utils/async.hpp>

using namespace geode::prelude;
using namespace cw::ads;

struct AdPreview::Impl final {
    Ad ad;

    async::TaskHolder<web::WebResponse> announcementListener;
    async::TaskHolder<web::WebResponse> clickListener;

    CCMenuItemSprite* playBtn = nullptr;
    LoadingSpinner* playBtnLoading = nullptr;

    bool hasClicked = false;
};

AdPreview::AdPreview() : m_impl(std::make_unique<Impl>()) {};
AdPreview::~AdPreview() {};

bool AdPreview::init(Ad ad) {
    m_impl->ad = std::move(ad);

    if (!Popup::init(250.f, 200.f, "geode.loader/GE_square03.png")) return false;

    setID("preview"_spr);
    setTitle(fmt::format("Ad ID | {}", m_impl->ad.getID()));
    setCloseButtonSpr(CircleButtonSprite::createWithSpriteFrameName("geode.loader/close.png", 0.875f, CircleBaseColor::DarkAqua, CircleBaseSize::Small));

    auto levelIdLabel = Label::create(
        fmt::format("Level ID | {}", m_impl->ad.getLevel()),
        "bigFont.fnt");
    levelIdLabel->setID("level-id-label");
    levelIdLabel->setPosition({m_mainLayer->getScaledContentWidth() / 2, m_mainLayer->getScaledContentHeight() - 40});
    levelIdLabel->setScale(0.5f);

    m_mainLayer->addChild(levelIdLabel);

    m_impl->playBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png"),
        this,
        menu_selector(AdPreview::onPlayButton));
    m_impl->playBtn->setID("play-btn");

    m_buttonMenu->addChildAtPosition(m_impl->playBtn, Anchor::Center, {0.f, 3.75f});

    m_impl->playBtnLoading = LoadingSpinner::create(m_impl->playBtn->getScaledContentHeight() * 0.875f);
    m_impl->playBtnLoading->setVisible(false);
    m_impl->playBtnLoading->setPosition(m_impl->playBtn->getPosition());

    m_buttonMenu->addChild(m_impl->playBtnLoading, 1);

    auto viewCountLabel = Label::create(
        fmt::format("{} View{}", GameToolbox::pointsToString(m_impl->ad.getViews()), m_impl->ad.getViews() != 1 ? "s" : ""),  // this is so dumb
        "geode.loader/mdFont.fnt");
    viewCountLabel->setID("view-count-label");
    viewCountLabel->setColor({255, 193, 136});
    viewCountLabel->setScale(0.425f);

    m_mainLayer->addChildAtPosition(viewCountLabel, Anchor::Center, {0.f, -45.f});

    auto clickCountLabel = Label::create(
        fmt::format("{} Click{}", GameToolbox::pointsToString(m_impl->ad.getClicks()), m_impl->ad.getClicks() != 1 ? "s" : ""),  // this is still very dumb
        "geode.loader/mdFontB.fnt");
    clickCountLabel->setID("click-count-label");
    clickCountLabel->setColor({96, 167, 206});
    clickCountLabel->setScale(0.375f);

    m_mainLayer->addChildAtPosition(clickCountLabel, Anchor::Center, {0.f, -55.f});

    // report button
    auto reportBtn = Button::createWithNode(
        CircleButtonSprite::createWithSpriteFrameName(
            "exMark_001.png",
            0.875f,
            CircleBaseColor::DarkAqua,
            CircleBaseSize::Medium),
        [this](auto) {
            if (auto reportPopup = ReportPopup::create(m_impl->ad)) reportPopup->show();
        });
    reportBtn->setID("report-ad-btn");
    reportBtn->setScale(0.625f);
    reportBtn->setPosition({0, 0});

    m_mainLayer->addChild(reportBtn);

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

                            if (auto b = btn.lock()) b->setVisible(true);
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
    announcementBtn->setScale(0.625f);

    m_mainLayer->addChildAtPosition(announcementBtn, Anchor::BottomRight, {}, false);

    announcementBtnLoading->setPosition(announcementBtn->getPosition());

    m_mainLayer->addChild(announcementBtnLoading, 1);

    auto btnContainerLayout = RowLayout::create()
                                  ->setGap(2.5f)
                                  ->setAutoScale(false)
                                  ->setAutoGrowAxis(1.25f);

    auto btnContainer = CCNode::create();
    btnContainer->setID("btn-container");
    btnContainer->setAnchorPoint({0.5, 0});
    btnContainer->setLayout(btnContainerLayout);

    m_mainLayer->addChildAtPosition(btnContainer, Anchor::Bottom, {0.f, 7.5f});

    auto btns = std::to_array<LinkButton>(
        {
            {
                "discord-btn",
                "gj_discordIcon_001.png",
                [](auto) {
                    createQuickPopup(
                        "Support Discord",
                        "Join the <cb>support Discord server</c> for help with using this mod?",
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
                        "Go to the <co>Player Ads Manager</c> dashboard?\n<cc>Upload ads for your levels here!</c>",
                        "Cancel",
                        "OK",
                        [](auto, bool ok) {
                            if (ok) web::openLinkInBrowser("https://ads.cheeseworks.gay/");
                        });
                },
            },
            {
                "kofi-btn",
                "btn_kofi.png"_spr,
                [](auto) {
                    createQuickPopup(
                        "Ko-fi",
                        "Would you like to <cd>support the mod through Ko-fi</c>?\n<cy>Earn cool perks like more views on your ads!</c>",
                        "Cancel",
                        "OK",
                        [](auto, bool ok) {
                            if (ok) web::openLinkInBrowser("https://ko-fi.com/playerads");
                        });
                },
            },
        });

    for (auto& b : btns) {
        auto btn = Button::createWithSpriteFrameName(
            b.sprite,
            std::move(b.callback));
        btn->setID(std::move(b.id));
        btn->setScale(0.875f);

        btnContainer->addChild(btn);
    };

    btnContainer->updateLayout();

    auto infoBtn = Button::createWithSpriteFrameName(
        "GJ_infoIcon_001.png",
        [](auto) {
            createQuickPopup(
                "Help",
                "This is the <cg>ad preview</c>. You can see statistics such as the <co>view count</c> & <cl>click count</c>, and <cy>play the level that was advertised</c>!\n\n<cc>If you find issues with the contents of the advertised level</c>, you can <cr>report the advertisement here</c>.",
                "OK",
                nullptr,
                nullptr);
        });
    infoBtn->setID("info-btn");
    infoBtn->setScale(0.75f);

    m_mainLayer->addChildAtPosition(infoBtn, Anchor::TopRight, {-13.75f, -13.75f});

    return true;
};

void AdPreview::onPlayButton(CCObject* sender) {
    m_impl->playBtn->setVisible(false);
    m_impl->playBtnLoading->setVisible(true);

    if (CCDirector::sharedDirector()->sceneCount() >= 5 && !Mod::get()->getSettingValue<bool>("scene-protection")) {
        createQuickPopup(
            "Hold up!",
            "You have <cr>too many scenes loaded</c> because you're opening too many ads. This may cause your game to become <cr>unstable</c>.\n"
            "<cy>Would you like to return to the main menu?</c>",
            "Cancel",
            "Yes",
            [](auto, bool ok) {
                if (ok) CCDirector::sharedDirector()->popToRootScene();
            });

        return;
    };

    if (PlayLayer::get()) {
        createQuickPopup(
            "Warning",
            "You are already inside of a level, opening this level will <cr>close your current level</c>.\n<cy>Do you still want to proceed</c>?",
            "Cancel",
            "Proceed",
            [this, sender](auto, bool btn) {
                if (btn) {
                    registerClick();
                    tryOpenOrFetchLevel(typeinfo_cast<CCMenuItemSpriteExtra*>(sender), m_impl->ad.getLevel());
                };
            });
    } else {
        auto menuItem = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);

        if (!m_impl->hasClicked) {
            log::debug("click registered for ad_id={}, user_id={}", m_impl->ad.getID(), m_impl->ad.getUser());

            m_impl->hasClicked = true;
            registerClick();
            tryOpenOrFetchLevel(menuItem, m_impl->ad.getLevel());
        } else {
            log::debug("click already registered for ad_id={}, user_id={}",
                m_impl->ad.getID(),
                m_impl->ad.getUser());
            tryOpenOrFetchLevel(menuItem, m_impl->ad.getLevel());
        };
    };
};

void AdPreview::registerClick() {
    log::debug("Sending click tracking request for ad_id={}, user_id={}", m_impl->ad.getID(), m_impl->ad.getUser());

    async::spawn(
        argon::startAuth(),
        [self = WeakRef(this)](Result<std::string> res) {
            if (auto s = self.lock()) {
                if (res.isErr()) {
                    log::warn("Auth failed: {}", std::move(res).unwrapErr());
                    return;
                };

                auto token = std::move(res).unwrapOrDefault();
                Mod::get()->setSavedValue<std::string>("authtoken", token);
                log::debug("Token: {}", token);

                log::debug("Sending click tracking request for ad_id={}, user_id={}", s->m_impl->ad.getID(), s->m_impl->ad.getUser());

                auto clickRequest = web::WebRequest();
                clickRequest.userAgent("PlayerAdvertisements/1.2");
                clickRequest.timeout(std::chrono::seconds(15));

                matjson::Value clickBody = matjson::Value::object();
                clickBody["ad_id"] = s->m_impl->ad.getID();
                clickBody["authtoken"] = std::move(token);
                clickBody["account_id"] = GJAccountManager::sharedState()->m_accountID;

                clickRequest.bodyJSON(clickBody);

                async::spawn(
                    clickRequest.post("https://ads.cheeseworks.gay/api/click"),
                    [self](web::WebResponse res) {
                        if (auto s = self.lock()) {
                            if (res.ok()) {
                                log::info("Click passed ad_id={}, user_id={}", s->m_impl->ad.getID(), s->m_impl->ad.getUser());
                            } else {
                                log::error(
                                    "Click failed with code {} for ad_id={}, user_id={}: {}",
                                    res.code(),
                                    s->m_impl->ad.getID(),
                                    s->m_impl->ad.getUser(),
                                    res.errorMessage());
                            };

                            log::debug("Click request completed for ad_id={}, user_id={}", s->m_impl->ad.getID(), s->m_impl->ad.getUser());
                        };
                    });

                log::debug("Sent click tracking request for ad_id={}, user_id={}", s->m_impl->ad.getID(), s->m_impl->ad.getUser());
            };
        });
};

void AdPreview::tryOpenOrFetchLevel(CCMenuItemSpriteExtra* menuItem, int levelId) {
    fetch::getLevel(levelId, [self = WeakRef(this)](Result<GJGameLevel*> res) {
        if (auto s = self.lock()) {
            if (res.isErr()) {
                log::error("Failed to get level: {}", std::move(res).unwrapErr());

                s->m_impl->playBtn->setVisible(true);
                s->m_impl->playBtnLoading->setVisible(false);

                return;
            };

            s->m_impl->playBtn->setVisible(true);
            s->m_impl->playBtnLoading->setVisible(false);

            pushSceneWithLayer(LevelInfoLayer::create(std::move(res).unwrap(), false));
        };
    });
};

AdPreview* AdPreview::create(Ad ad) {
    auto ret = new AdPreview();
    if (ret->init(std::move(ad))) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};