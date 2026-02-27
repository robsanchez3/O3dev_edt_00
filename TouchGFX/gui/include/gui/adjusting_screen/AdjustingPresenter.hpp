#ifndef ADJUSTINGPRESENTER_HPP
#define ADJUSTINGPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class AdjustingView;

class AdjustingPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    AdjustingPresenter(AdjustingView& v);

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

    virtual ~AdjustingPresenter() {};

    uint16_t getTherapyTargetValue(uint8_t valueID);
    uint8_t getFsmState();
    void StopGeneration();
    uint16_t getVisibleScreen();

private:
    AdjustingPresenter();

    AdjustingView& view;
};

#endif // ADJUSTINGPRESENTER_HPP
