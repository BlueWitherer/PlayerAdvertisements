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
        if (!ShareCommentLayer::init(std::move(title), charLimit, type, ID, std::move(desc))) return false;

        if (auto adSquareLeft = nodes::placeAd(m_mainLayer, nodes::formatIDForAd(AdType::Square, Anchor::BottomLeft), AdType::Square, Anchor::Bottom, {-144.f, 90.f})) positionForCommentType(adSquareLeft, type);
        if (auto adSquareCenter = nodes::placeAd(m_mainLayer, AdType::Square, Anchor::Bottom, {0.f, 90.f})) positionForCommentType(adSquareCenter, type);
        if (auto adSquareRight = nodes::placeAd(m_mainLayer, nodes::formatIDForAd(AdType::Square, Anchor::BottomRight), AdType::Square, Anchor::Bottom, {144.f, 90.f})) positionForCommentType(adSquareRight, type);

        if (win::isWide()) {
            nodes::placeAd(m_mainLayer, AdType::Skyscraper, Anchor::Left, {30.f, 0.f});
            nodes::placeAd(m_mainLayer, AdType::Skyscraper, Anchor::Right, {-30.f, 0.f});
        };

        return true;
    };

    // for da squarez
    void positionForCommentType(Advertisement* ad, CommentType type) {
        if (type == CommentType::FriendRequest) {
            log::debug("comment type is friend request");
            if (ad) ad->setPositionY(ad->getPositionY() - 25.f);
        };
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);