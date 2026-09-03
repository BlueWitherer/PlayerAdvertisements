#include <AdsUtils.h>

#include <Advertisements.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/GJDropDownLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

#define THIS_ID "GJDropDownLayer"
static constexpr auto g_hookId = THIS_ID;

class $modify(AdsGJDropDownLayer, GJDropDownLayer) {
    PLAYERADS_DELEGATE_HOOKS(THIS_ID);

    bool init(const char* p0, float p1, bool p2) {
        if (!GJDropDownLayer::init(p0, p1, p2)) return false;

        nodes::placeAd(m_mainLayer, AdType::Banner, Anchor::Top, {0.f, -30.f});

        if (win::isWide()) {
            nodes::placeAd(m_mainLayer, AdType::Skyscraper, Anchor::Left, {30.f, 0.f});
            nodes::placeAd(m_mainLayer, AdType::Skyscraper, Anchor::Right, {-30.f, 0.f});
        };

        return true;
    };
};

PLAYERADS_HOOK_LISTENER(g_hookId);