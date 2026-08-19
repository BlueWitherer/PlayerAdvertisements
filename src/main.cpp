#include <Advertisements.h>

#include <argon/argon.hpp>

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>
#include <Geode/ui/Button.hpp>
#include <Geode/cocos/menu_nodes/CCMenuItem.h>

#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;
using namespace cw::ads;

$on_mod(Loaded) {
    async::spawn(
        argon::startAuth(),
        [](Result<std::string> res) {
            if (res.isOk()) {
                auto token = std::move(res).unwrap();
                Mod::get()->setSavedValue<std::string>("argon_token", token);
            } else {
                log::warn("Auth failed: {}", res.unwrapErr());
            };
        });
};