#pragma once

#include <Geode/Geode.hpp>

namespace cw::ads {
    class AdsViewer final : public cocos2d::CCLayer {
    protected:
        void keyBackClicked() override;

        bool init() override;

    public:
        static AdsViewer* create();
    };
};