#include <Advertisements.h>

#include <argon/argon.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

class $modify(AdsMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        async::spawn(
            argon::startAuth(),
            [this](Result<std::string> res) {
                if (res.isOk()) {
                    auto token = std::move(res).unwrap();
                    Mod::get()->setSavedValue<std::string>("authtoken", token);
                } else {
                    log::warn("Auth failed: {}", res.unwrapErr());
                };
            });

        auto const winSize = CCDirector::sharedDirector()->getWinSize();

        if (Mod::get()->getSettingValue<bool>("MenuLayer")) {
            if (auto adBanner = Advertisement::create()) {
                adBanner->setID("banner"_spr);
                adBanner->setType(AdType::Banner);
                adBanner->setPosition({winSize.width / 2.f, winSize.height / 2.f - 70.f});

                this->addChild(adBanner);

                adBanner->loadRandom();
            };
        };

        return true;
    };
};