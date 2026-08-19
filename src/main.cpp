#include <Advertisements.h>

#include <argon/argon.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cw::ads;

$on_mod(Loaded) {
    async::spawn(
        argon::startAuth(),
        [](Result<std::string> res) {
            if (res.isOk()) {
                auto token = std::move(res).unwrap();
                Mod::get()->setSavedValue<std::string>("authtoken", token);
            } else {
                log::warn("Auth failed: {}", res.unwrapErr());
            };
        });
};