#ifndef WASHINGPRESENTER_HPP
#define WASHINGPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class WashingView;

class WashingPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    WashingPresenter(WashingView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~WashingPresenter() {};

    uint16_t getVisibleScreen();
    uint16_t getTherapyTargetValue(uint8_t valueID);
    bool refreshGenerationInfo();
    void clearGenerationInfoFlag();
    uint8_t  getWashingSeconds();
    void cancelWashing();
    void washingExtesion();

private:
    WashingPresenter();

    WashingView& view;
};

#endif // WASHINGPRESENTER_HPP
