#include "../AdPreview.hpp"

#include "../ReportPopup.hpp"

#include <Advertisements.h>

#include <argon/argon.hpp>

#include <Geode/Geode.hpp>

#include <Geode/ui/Button.hpp>

#include <Geode/utils/async.hpp>

using namespace geode::prelude;
using namespace geode::utils;

using namespace cw::ads;

struct AdPreview::Impl final {
    Ad ad;

    async::TaskHolder<web::WebResponse> announcementListener;
    async::TaskHolder<web::WebResponse> clickListener;

    std::string pendingKey;
    int pendingLevelId = -1;
    float pendingTimeout = 0.0f;

    LoadingSpinner* pendingSpinner = nullptr;
    CCMenuItemSprite* playBtn;

    bool hasClicked = false;
};

AdPreview::AdPreview() : m_impl(std::make_unique<Impl>()) {};
AdPreview::~AdPreview() {};

bool AdPreview::init(Ad ad) {
    m_impl->ad = std::move(ad);

    // @geode-ignore(unknown-resource)
    if (!Popup::init(250.f, 200.f, "geode.loader/GE_square03.png")) return false;

    setID("preview"_spr);
    setTitle("Ad ID: " + numToString(m_impl->ad.getID()));
    setCloseButtonSpr(CircleButtonSprite::createWithSpriteFrameName("geode.loader/close.png", 0.875f, CircleBaseColor::DarkAqua, CircleBaseSize::Small));

    auto levelIdLabel = Label::create(
        ("Level ID: " + numToString(m_impl->ad.getLevel())).c_str(),
        "bigFont.fnt");
    levelIdLabel->setID("level-id-label");
    levelIdLabel->setPosition({m_mainLayer->getScaledContentWidth() / 2, m_mainLayer->getScaledContentHeight() - 40});
    levelIdLabel->setScale(0.5f);

    m_mainLayer->addChild(levelIdLabel);

    auto playAdLevelSprite = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
    m_impl->playBtn = CCMenuItemSpriteExtra::create(
        playAdLevelSprite,
        this,
        menu_selector(AdPreview::onPlayButton));
    m_impl->playBtn->setID("play-btn");
    m_impl->playBtn->setPosition({m_mainLayer->getScaledContentWidth() / 2, m_mainLayer->getScaledContentHeight() / 2});

    // store the menu for spinner placement / restoring state
    m_buttonMenu->addChild(m_impl->playBtn);

    // view and click counts
    auto viewCountLabel = Label::create(
        ("Views: " + numToString(m_impl->ad.getViews())).c_str(),
        "goldFont.fnt");
    viewCountLabel->setID("view-count-label");
    viewCountLabel->setColor({255, 125, 0});
    viewCountLabel->setPosition({m_mainLayer->getScaledContentWidth() / 2, m_mainLayer->getScaledContentHeight() / 2 - 55});
    viewCountLabel->setScale(0.7f);

    m_mainLayer->addChild(viewCountLabel);

    auto clickCountLabel = Label::create(
        ("Clicks: " + numToString(m_impl->ad.getClicks())).c_str(),
        "goldFont.fnt");
    clickCountLabel->setID("click-count-label");
    clickCountLabel->setColor({0, 175, 255});
    clickCountLabel->setPosition({m_mainLayer->getScaledContentWidth() / 2, m_mainLayer->getScaledContentHeight() / 2 - 75});
    clickCountLabel->setScale(0.7f);

    m_mainLayer->addChild(clickCountLabel);

    // report button
    auto reportBtn = Button::createWithNode(
        CircleButtonSprite::createWithSpriteFrameName(
            // @geode-ignore(unknown-resource)
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

    auto announcementBtn = Button::createWithNode(
        CircleButtonSprite::createWithSpriteFrameName(
            // @geode-ignore(unknown-resource)
            "geode.loader/news.png",
            0.75f,
            CircleBaseColor::DarkAqua,
            CircleBaseSize::Medium),
        [](auto) {
            // fetch from /api/announcement
            auto request = web::WebRequest();
            request.userAgent("PlayerAdvertisements/1.2");
            request.timeout(std::chrono::seconds(15));

            async::spawn(
                request.get("https://ads.cheeseworks.gay/api/announcement"),
                [](web::WebResponse res) {
                    if (res.ok()) {
                        auto data = res.json();
                        if (!data.isOk()) {
                            log::error("Failed to parse announcement JSON");
                            return;
                        };

                        auto const val = data.unwrap();

                        std::string const title =
                            val.contains("title") && val["title"].asString().isOk()
                                ? val["title"].asString().unwrap()
                                : "Announcement";

                        std::string const content =
                            val.contains("content") && val["content"].asString().isOk()
                                ? val["content"].asString().unwrap()
                                : "";

                        if (auto popup = MDPopup::create(title.c_str(), content.c_str(), "Close")) popup->show();
                    } else {
                        log::error("Failed to fetch announcement: (code: {})", res.code());
                        Notification::create("Failed to fetch announcement",
                            NotificationIcon::Error)
                            ->show();
                    };
                });
        });
    announcementBtn->setID("latest-announcement-btn");
    announcementBtn->setScale(0.625f);
    announcementBtn->setPosition({m_mainLayer->getScaledContentWidth(), 0});

    m_mainLayer->addChild(announcementBtn);

    scheduleUpdate();

    return true;
};

void AdPreview::onPlayButton(CCObject* sender) {
    if (CCDirector::sharedDirector()->sceneCount() >= 5 &&
        Mod::get()->getSettingValue<bool>("scene-protection") == false) {
        createQuickPopup(
            "Hold up!",
            "You have <cr>too many scenes loaded</c> because you're opening too "
            "many ads. This may cause your game to become "
            "<cr>unstable</c>.\n<cy>Would you like to return to the main menu?</c>",
            "Cancel",
            "Yes",
            [](auto, bool ok) {
                if (ok) {
                    // pop to root scene
                    CCDirector::sharedDirector()->popToRootScene();
                };
            });

        return;
    };

    if (PlayLayer::get()) {
        createQuickPopup(
            "Warning",
            "You are already inside of a level, opening this level will <cr>close your current level</c>.\n<cy>Do you still want to proceed?</c>",
            "Cancel",
            "Proceed",
            [this, sender](auto, bool btn) {
                if (btn) {
                    auto menuItem = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);
                    registerClick();
                    tryOpenOrFetchLevel(menuItem, m_impl->ad.getLevel());
                };
            });
    } else {
        auto menuItem = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);
        if (!m_impl->hasClicked) {
            m_impl->hasClicked = true;
            registerClick();
            log::debug("click registered for ad_id={}, user_id={}", m_impl->ad.getID(), m_impl->ad.getUser());
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

// open LevelInfo if stored otherwise prepare pending state and request
void AdPreview::tryOpenOrFetchLevel(CCMenuItemSpriteExtra* menuItem, int levelId) {
    if (!menuItem) return;

    auto searchObj = GJSearchObject::create(SearchType::Search, numToString(levelId));
    auto key = std::string(searchObj->getKey());
    auto glm = GameLevelManager::sharedState();

    // check stored cache first
    auto stored = glm->getStoredOnlineLevels(key.c_str());
    if (stored && stored->count() > 0) {
        auto level = typeinfo_cast<GJGameLevel*>(stored->objectAtIndex(0));
        if (level && level->m_levelID == levelId) {
            auto scene = LevelInfoLayer::scene(level, false);
            auto transitionFade = CCTransitionFade::create(0.5f, scene);
            if (PlayLayer::get()) {
                CCDirector::sharedDirector()->replaceScene(transitionFade);
                FMODAudioEngine::sharedEngine()->resumeAllAudio();
            } else {
                CCDirector::sharedDirector()->pushScene(transitionFade);
            }
            glm->m_levelManagerDelegate = nullptr;
            return;
        };
    };

    // prepare pending state
    m_impl->pendingKey = std::move(key);
    m_impl->pendingLevelId = levelId;
    m_impl->pendingTimeout = 10.0f;  // seconds

    // show spinner on the clicked button and disable it
    if (m_impl->pendingSpinner) {
        m_impl->pendingSpinner->removeFromParent();
        m_impl->pendingSpinner = nullptr;
        m_impl->playBtn->setVisible(true);
    };

    if (auto spinner = LoadingSpinner::create(100.f)) {
        spinner->setPosition(menuItem->getPosition());
        spinner->setVisible(true);
        m_impl->playBtn->setVisible(false);

        m_buttonMenu->addChild(spinner);

        m_impl->pendingSpinner = spinner;
    };

    glm->getOnlineLevels(searchObj);
};

void AdPreview::update(float dt) {
    if (!m_impl->pendingKey.empty()) {
        auto glm = GameLevelManager::sharedState();
        auto stored = glm->getStoredOnlineLevels(m_impl->pendingKey.c_str());

        if (stored && stored->count() > 0) {
            auto level = typeinfo_cast<GJGameLevel*>(stored->objectAtIndex(0));

            if (level && level->m_levelID == m_impl->pendingLevelId) {
                // open level info
                auto scene = LevelInfoLayer::scene(level, false);
                auto transitionFade = CCTransitionFade::create(0.5f, scene);
                if (PlayLayer::get()) {
                    CCDirector::sharedDirector()->replaceScene(transitionFade);
                    FMODAudioEngine::sharedEngine()->resumeAllAudio();
                } else {
                    CCDirector::sharedDirector()->pushScene(transitionFade);
                };

                // cleanup pending state and spinner
                if (m_impl->pendingSpinner) {
                    m_impl->pendingSpinner->removeFromParent();
                    m_impl->pendingSpinner = nullptr;
                };
                m_impl->playBtn->setVisible(true);

                m_impl->pendingKey.clear();
                m_impl->pendingLevelId = -1;
                m_impl->pendingTimeout = 0.0;

                glm->m_levelManagerDelegate = nullptr;
                return;
            };
        };

        m_impl->pendingTimeout -= dt;
        if (m_impl->pendingTimeout <= 0.0) {
            if (m_impl->pendingSpinner) {
                m_impl->pendingSpinner->removeFromParent();
                m_impl->pendingSpinner = nullptr;
            };

            m_impl->playBtn->setVisible(true);

            Notification::create("Level not found", NotificationIcon::Warning)->show();

            m_impl->pendingKey.clear();
            m_impl->pendingLevelId = -1;
            m_impl->pendingTimeout = 0.0;
        };
    };
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