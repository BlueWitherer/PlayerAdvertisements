#include <Advertisements.h>

#include <Geode/Geode.hpp>

namespace cw::ads {
    class ReportPopup final : public geode::Popup {
    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;

    protected:
        ReportPopup();
        ~ReportPopup();

        void onSubmitButton(cocos2d::CCObject* sender);

        bool init(Ad ad);

    public:
        static ReportPopup* create(Ad ad);
    };
};