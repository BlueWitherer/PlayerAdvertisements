#pragma once

#include <AdsUtils.h>

#include <Geode/Geode.hpp>

namespace cw::ads {
    class AdsViewerCell final : public cocos2d::CCNode {
    private:
        Ad m_ad;

    protected:
        bool init(Ad ad, float width);

    public:
        static AdsViewerCell* create(Ad ad, float width);
    };

    class AdsViewer final : public cocos2d::CCLayer {
    protected:
        void keyBackClicked() override;

        bool init() override;

    public:
        static AdsViewer* create();
    };
};