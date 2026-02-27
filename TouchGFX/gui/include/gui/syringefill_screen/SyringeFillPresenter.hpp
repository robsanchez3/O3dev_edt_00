#ifndef SYRINGEFILLPRESENTER_HPP
#define SYRINGEFILLPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SyringeFillView;

class SyringeFillPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SyringeFillPresenter(SyringeFillView& v);

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

    virtual ~SyringeFillPresenter() {};

    uint16_t getVisibleScreen();
    uint16_t getTherapyTargetValue(uint8_t valueID);
    uint8_t getFsmState();
    void userCancelled(void);
    void userOk(void);
    void userOkRelease(void);
    bool isCompensatingPressure();
    bool isPauseAvailable();
    bool isPaused();

private:
    SyringeFillPresenter();

    SyringeFillView& view;
};

#endif // SYRINGEFILLPRESENTER_HPP
