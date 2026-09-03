#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/ProfilePage.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "ProfilePage"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsProfilePage, ProfilePage) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    bool init(int p0, bool p1) {
        if (!ProfilePage::init(p0, p1)) return false;

        if (win::isWide()) {
            nodes::placeAd(m_mainLayer, AdType::Skyscraper, Anchor::Left, {30.f, 0.f});
            nodes::placeAd(m_mainLayer, AdType::Skyscraper, Anchor::Right, {-30.f, 0.f});
        };

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);