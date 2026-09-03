#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/ShareCommentLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "ShareCommentLayer"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsShareCommentLayer, ShareCommentLayer) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    bool init(gd::string title, int charLimit, CommentType type, int ID, gd::string desc) {
        if (!ShareCommentLayer::init(title, charLimit, type, ID, desc)) return false;

        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        if (auto adSquareLeft = Advertisement::create(AdType::Square)) {
            adSquareLeft->setID("square-left"_spr);
            adSquareLeft->setPosition({winSize.width / 2.f - 140.f, winSize.height / 2.f - 70.f});

            m_mainLayer->addChild(adSquareLeft, HIGHEST_Z);
            positionForCommentType(adSquareLeft, type);
        };

        if (auto adSquareCenter = Advertisement::create(AdType::Square)) {
            adSquareCenter->setID("square-center"_spr);
            adSquareCenter->setPosition({winSize.width / 2.f, winSize.height / 2.f - 70.f});

            m_mainLayer->addChild(adSquareCenter, HIGHEST_Z);
            positionForCommentType(adSquareCenter, type);
        };

        if (auto adSquareRight = Advertisement::create(AdType::Square)) {
            adSquareRight->setID("square-right"_spr);
            adSquareRight->setPosition({winSize.width / 2.f + 140.f, winSize.height / 2.f - 70.f});

            m_mainLayer->addChild(adSquareRight, HIGHEST_Z);
            positionForCommentType(adSquareRight, type);
        };

        if (auto adSkyscraperRight = Advertisement::create(AdType::Skyscraper)) {
            adSkyscraperRight->setID("skyscraper-right"_spr);
            adSkyscraperRight->setPosition({winSize.width - 30.f, winSize.height / 2.f});

            m_mainLayer->addChild(adSkyscraperRight, HIGHEST_Z);
        };

        if (auto adSkyscraperLeft = Advertisement::create(AdType::Skyscraper)) {
            adSkyscraperLeft->setID("skyscraper-left"_spr);
            adSkyscraperLeft->setPosition({30.f, winSize.height / 2.f});

            m_mainLayer->addChild(adSkyscraperLeft, HIGHEST_Z);
        };

        return true;
    };

    // for da square adz
    void positionForCommentType(Advertisement* ad, CommentType type) {
        if (type == CommentType::FriendRequest) {
            log::debug("comment type is friend request");
            if (ad) ad->setPositionY(ad->getPositionY() - 25.f);
        };
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);