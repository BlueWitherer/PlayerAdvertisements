#include "../AdPreview.hpp"

#include "../ReportPopup.hpp"

#include <Advertisements.h>

#include <argon/argon.hpp>

#include <Geode/Geode.hpp>

#include <Geode/ui/Button.hpp>

#include <Geode/utils/async.hpp>

using namespace geode::prelude;
using namespace cw::ads;

namespace cw::ads {
    enum class LevelRating {
        None = 0,
        Star = 1,
        Featured = 2,
        Epic = 3,
        Legendary = 4,
        Mythic = 5,
    };
};

struct AdPreview::Impl final {
    Ad ad;

    Ref<GJGameLevel> level;

    async::TaskHolder<web::WebResponse> announcementListener;
    async::TaskHolder<web::WebResponse> clickListener;

    CCMenuItemSprite* playBtn = nullptr;
    Ref<CCSprite> playBtnSprite = nullptr;
    LoadingSpinner* playBtnLoading = nullptr;

    bool hasClicked = false;

    constexpr auto getLengthText(int length, bool plat = false) {
        if (plat) return "Plat.";

        switch (length) {
            default: [[fallthrough]];

            case 0: return "Tiny";
            case 1: return "Short";
            case 2: return "Medium";
            case 3: return "Long";
            case 4: return "XL";
        };
    };

    constexpr auto getDiffSpriteNum(GJGameLevel* level) {
        if (level->m_demon.value() == 1) {
            switch (level->m_demonDifficulty) {
                default: return 6;

                case 3: return 7;
                case 4: return 8;
                case 5: return 9;
                case 6: return 10;
            };
        };

        if (level->m_autoLevel) return -1;
        return level->getAverageDifficulty();
    };

    constexpr auto getRating(GJGameLevel* level) {
        switch (level->m_isEpic) {
            default: break;

            case 1: return LevelRating::Epic;
            case 2: return LevelRating::Legendary;
            case 3: return LevelRating::Mythic;
        };

        if (level->m_featured != 0) return LevelRating::Featured;
        if (level->m_stars.value() != 0) return LevelRating::Star;

        return LevelRating::None;
    };

    constexpr auto getFeatureState(LevelRating r) {
        switch (r) {
            case LevelRating::None: return GJFeatureState::None;
            case LevelRating::Star: return GJFeatureState::None;
            case LevelRating::Featured: return GJFeatureState::Featured;
            case LevelRating::Epic: return GJFeatureState::Epic;
            case LevelRating::Legendary: return GJFeatureState::Legendary;
            case LevelRating::Mythic: return GJFeatureState::Mythic;
        };
    };
};

AdPreview::AdPreview() : m_impl(std::make_unique<Impl>()) {};
AdPreview::~AdPreview() {};

bool AdPreview::init(Ad ad) {
    m_impl->ad = std::move(ad);

    if (!Popup::init(250.f, 200.f, "geode.loader/GE_square03.png")) return false;

    setID("preview"_spr);
    setTitle(fmt::format("Ad ID | {}", m_impl->ad.getID()));
    setCloseButtonSpr(CircleButtonSprite::createWithSpriteFrameName("geode.loader/close.png", 0.875f, CircleBaseColor::DarkAqua, CircleBaseSize::Small));

    m_impl->playBtnSprite = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");  // iq 99999 (send help)
    m_impl->playBtnLoading = LoadingSpinner::create(m_impl->playBtnSprite->getScaledContentHeight() * 0.875f);

    m_buttonMenu->addChildAtPosition(m_impl->playBtnLoading, Anchor::Center, {0.f, 3.75f});

    log::trace("Requesting data for level of ID {} for ad of ID {}", m_impl->ad.getLevel(), m_impl->ad.getID());

    fetch::getLevel(m_impl->ad.getLevel(), [self = WeakRef(this)](Result<GJGameLevel*> res) {
        if (auto s = self.lock()) {
            s->m_impl->playBtnLoading->setVisible(false);

            if (res.isErr()) {
                log::error("Failed to parse level: {}", std::move(res).unwrapErr());

                auto errLabel = Label::create("Something went wrong...", "goldFont.fnt");
                errLabel->setID("error-label");
                errLabel->setScale(0.375f);
                errLabel->setAlignment(Label::Alignment::Center);

                s->m_mainLayer->addChildAtPosition(errLabel, Anchor::Center);

                return;
            };

            s->m_impl->level = std::move(res).unwrap();  // it's soooo verbose
            auto& lvl = s->m_impl->level;

            log::debug("Retrieved level {} ({}) for ad of ID {}", lvl->m_levelName, lvl->m_levelID, s->m_impl->ad.getID());

            auto levelLabel = Label::create(
                lvl->m_levelName,
                "bigFont.fnt");
            levelLabel->setID("level-name-label");
            levelLabel->setAlignment(Label::Alignment::Center);
            levelLabel->setLimitLabelWidth(s->m_mainLayer->getScaledContentWidth() - 1.25f, 0.5f);
            levelLabel->setPosition({s->m_mainLayer->getScaledContentWidth() / 2.f, s->m_mainLayer->getScaledContentHeight() - 37.5f});
            levelLabel->setScale(0.5f);

            s->m_mainLayer->addChild(levelLabel);

            auto levelIDStr = numToString(lvl->m_levelID.value());

            auto levelIDLabel = Button::createWithLabel(
                levelIDStr,
                "chatFont.fnt",
                [id = levelIDStr](auto) {
                    utils::clipboard::write(id);
                    Notification::create("Copied to clipboard", NotificationIcon::Success)->show();
                });
            levelIDLabel->setID("copy-level-id-btn");
            levelIDLabel->setOpacity(200);
            levelIDLabel->setPosition({levelLabel->getPositionX(), levelLabel->getPositionY() - 12.5f});
            levelIDLabel->setScale(0.625f);

            s->m_mainLayer->addChild(levelIDLabel, 1);

            s->m_impl->playBtn = CCMenuItemSpriteExtra::create(
                s->m_impl->playBtnSprite.take(),
                s,
                menu_selector(AdPreview::onPlayButton));
            s->m_impl->playBtn->setID("play-btn");
            s->m_impl->playBtn->setPosition(s->m_impl->playBtnLoading->getPosition());

            s->m_buttonMenu->addChild(s->m_impl->playBtn, 1);

            auto statsContainerLayout = ColumnLayout::create()
                                            ->setGap(5.f)
                                            ->setAutoScale(false)
                                            ->setCrossAxisLineAlignment(AxisAlignment::Start)
                                            ->setCrossAxisAlignment(AxisAlignment::Start)
                                            ->setAutoGrowAxis(0.f)
                                            ->setAxisReverse(true);

            auto statsContainer = CCNode::create();
            statsContainer->setID("level-stats-container");
            statsContainer->setAnchorPoint({0, 0.5});
            statsContainer->setPosition({s->m_impl->playBtn->getPositionX() + (s->m_impl->playBtn->getScaledContentWidth() * 0.625f), s->m_impl->playBtn->getPositionY()});
            statsContainer->setLayout(statsContainerLayout);

            s->m_mainLayer->addChild(statsContainer, 9);

            log::info("level {} length: {}",  lvl->m_levelName, lvl->m_levelLength);

            statsContainer->addChild(AdPreviewStat::create("GJ_downloadsIcon_001.png", numToAbbreviatedString(lvl->m_downloads)));
            statsContainer->addChild(AdPreviewStat::create(lvl->m_likes >= 0 ? "GJ_likesIcon_001.png" : "GJ_dislikesIcon_001.png", numToAbbreviatedString(lvl->m_likes)));
            statsContainer->addChild(AdPreviewStat::create("GJ_timeIcon_001.png", s->m_impl->getLengthText(lvl->m_levelLength, lvl->isPlatformer())));

            statsContainer->updateLayout();

            auto diff = GJDifficultySprite::create(s->m_impl->getDiffSpriteNum(lvl), GJDifficultyName::Long);
            diff->setID("difficulty-icon");
            diff->setPosition({s->m_impl->playBtn->getPositionX() - (s->m_impl->playBtn->getScaledContentWidth() * 0.875f), s->m_impl->playBtn->getPositionY()});
            diff->updateFeatureState(s->m_impl->getFeatureState(s->m_impl->getRating(lvl)));

            s->m_mainLayer->addChild(diff, 9);

            auto viewCountLabel = Label::create(
                fmt::format("{} View{}", GameToolbox::pointsToString(s->m_impl->ad.getViews()), s->m_impl->ad.getViews() != 1 ? "s" : ""),  // this is so dumb
                "geode.loader/mdFont.fnt");
            viewCountLabel->setID("view-count-label");
            viewCountLabel->setScale(0.425f);
            viewCountLabel->setColor({255, 193, 136});
            viewCountLabel->setAlignment(Label::Alignment::Center);

            s->m_mainLayer->addChildAtPosition(viewCountLabel, Anchor::Center, {0.f, -45.f});

            auto clickCountLabel = Label::create(
                fmt::format("{} Click{}", GameToolbox::pointsToString(s->m_impl->ad.getClicks()), s->m_impl->ad.getClicks() != 1 ? "s" : ""),  // this is still very dumb
                "geode.loader/mdFontB.fnt");
            clickCountLabel->setID("click-count-label");
            clickCountLabel->setScale(0.375f);
            clickCountLabel->setColor({104, 173, 209});
            clickCountLabel->setAlignment(Label::Alignment::Center);

            s->m_mainLayer->addChildAtPosition(clickCountLabel, Anchor::Center, {0.f, -55.f});
        }; }, false);

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
    m_impl->playBtnLoading->setVisible(true);
    m_impl->playBtn->setVisible(false);

    if (CCDirector::sharedDirector()->sceneCount() >= 5 && !Mod::get()->getSettingValue<bool>("scene-protection")) {
        createQuickPopup(
            "Hold up!",
            "You have <cr>too many scenes loaded</c> from opening too many ads. This may cause your game to become <cr>unstable</c>.\n"
            "<cy>Would you like to return?</c>",
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
            [this, sender](auto, bool ok) {
                if (ok) {
                    registerClick();
                    switchToLevel();
                };
            });
    } else {
        if (!m_impl->hasClicked) {
            log::debug("click registered for ad_id={}, user_id={}", m_impl->ad.getID(), m_impl->ad.getUser());

            m_impl->hasClicked = true;

            registerClick();
            switchToLevel();
        } else {
            log::debug("click already registered for ad_id={}, user_id={}",
                m_impl->ad.getID(),
                m_impl->ad.getUser());
            switchToLevel();
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

void AdPreview::switchToLevel() {
    auto layer = LevelInfoLayer::create(m_impl->level, false);
    pushSceneWithLayer(layer);

    m_impl->playBtn->setVisible(true);
    m_impl->playBtnLoading->setVisible(false);

    layer->downloadLevel();  // idk rlly :p
    layer->onUpdate(layer->querySelector("right-side-menu > refresh-button"));

    if (auto gm = GameLevelManager::get()) gm->saveLevel(m_impl->level);
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

bool AdPreviewStat::init(ZStringView sprite, std::string label) {
    if (!CCNode::init()) return false;

    auto layout = RowLayout::create()
                      ->setGap(2.5f)
                      ->setAutoScale(false)
                      ->setAutoGrowAxis(0.f)
                      ->setAxisAlignment(AxisAlignment::Start);

    setContentSize({0.f, 12.5f});
    setLayout(layout);

    auto icon = CCSprite::createWithSpriteFrameName(sprite.c_str());
    cue::rescaleToMatch(icon, getScaledContentHeight());

    addChild(icon);

    auto text = Label::create(std::move(label), "bigFont.fnt");
    text->setLimitLabelWidth(37.5f, 0.425f);
    text->setScale(0.425f);

    addChild(text);

    updateLayout();

    return true;
};

AdPreviewStat* AdPreviewStat::create(ZStringView sprite, std::string label) {
    auto ret = new AdPreviewStat();
    if (ret->init(sprite, std::move(label))) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};