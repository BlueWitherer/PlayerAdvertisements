#pragma once

#include <AdsUtils.h>

#include <Geode/Geode.hpp>

namespace cw::ads {
    class AdsViewerCell final : public cocos2d::CCNode {
    private:
        Ad m_ad;

    protected:
        bool init(Ad ad, bool managed, float width);

    public:
        static AdsViewerCell* create(Ad ad, bool managed, float width);
    };

    class AdsViewerSection : public cocos2d::CCNode {  // yea idk
    protected:
        virtual bool init(cocos2d::CCSize const& size);

    public:
        static AdsViewerSection* create(cocos2d::CCSize const& size);
    };

    class AdsViewerRecent final : public AdsViewerSection {
    private:
        geode::ScrollLayer* m_list = nullptr;

    protected:
        bool init(cocos2d::CCSize const& size) override;

    public:
        static AdsViewerRecent* create(cocos2d::CCSize const& size);
    };

    class AdsViewerManaged final : public AdsViewerSection {
    private:
        geode::ScrollLayer* m_list = nullptr;

    protected:
        bool init(cocos2d::CCSize const& size) override;

    public:
        static AdsViewerManaged* create(cocos2d::CCSize const& size);
    };

    class AdsViewerKofi final : public AdsViewerSection {
    private:
        geode::ScrollLayer* m_list = nullptr;

    protected:
        bool init(cocos2d::CCSize const& size) override;

    public:
        static AdsViewerKofi* create(cocos2d::CCSize const& size);
    };

    class AdsViewer final : public cocos2d::CCLayer {
    private:
        asp::SmallVec<geode::Ref<AdsViewerSection>, 3> m_tabs;

    protected:
        void keyBackClicked() override;

        bool init() override;

    public:
        static AdsViewer* create();
    };
};