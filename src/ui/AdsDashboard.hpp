#pragma once

#include <AdsUtils.h>

#include <Geode/Geode.hpp>

namespace cw::ads {
    class AdsDashboardCell final : public cocos2d::CCNode {
    private:
        Ad m_ad;

    protected:
        bool init(Ad ad, bool managed, bool pending, float width);

    public:
        static AdsDashboardCell* create(Ad ad, bool managed, bool pending, float width);
    };

    class AdsDashboardSection : public cocos2d::CCNode {  // yea idk
    protected:
        virtual bool init(cocos2d::CCSize const& size);

    public:
        static AdsDashboardSection* create(cocos2d::CCSize const& size);
    };

    class AdsDashboardRecent final : public AdsDashboardSection {
    private:
        geode::ScrollLayer* m_list = nullptr;

    protected:
        bool init(cocos2d::CCSize const& size) override;

    public:
        static AdsDashboardRecent* create(cocos2d::CCSize const& size);
    };

    class AdsDashboardManaged final : public AdsDashboardSection {
    private:
        geode::ScrollLayer* m_list = nullptr;

        geode::async::TaskHolder<fetch::AuthResult> m_authTask;
        geode::async::TaskHolder<geode::utils::web::WebResponse> m_getTask;

    protected:
        bool init(cocos2d::CCSize const& size) override;

    public:
        static AdsDashboardManaged* create(cocos2d::CCSize const& size);
    };

    class AdsDashboardKofi final : public AdsDashboardSection {
    private:
        geode::ScrollLayer* m_list = nullptr;

    protected:
        bool init(cocos2d::CCSize const& size) override;

    public:
        static AdsDashboardKofi* create(cocos2d::CCSize const& size);
    };

    class AdsDashboard final : public cocos2d::CCLayer {
    private:
        asp::SmallVec<geode::Ref<AdsDashboardSection>, 4> m_tabs;

    protected:
        void keyBackClicked() override;

        bool init() override;

    public:
        static AdsDashboard* create();
    };
};