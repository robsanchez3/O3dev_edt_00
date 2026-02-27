#ifndef RUNNINGPRESENTER_HPP
#define RUNNINGPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class RunningView;

class RunningPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    RunningPresenter(RunningView& v);

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

    virtual ~RunningPresenter() {};

    uint16_t getTherapyTargetValue(uint8_t valueID);
    uint8_t  getRemainingMinutes();
    uint8_t  getRemainingSeconds();
    int16_t  getCurrentOperatingPressure();
    uint32_t getCurrentTotalDose();
    uint32_t getCurrentOutputVolume();
    uint32_t getConfiguredTime();
    bool refreshGenerationInfo();
    void clearGenerationInfoFlag();
    void userCancelled(void);
    void userOk(void);
    void userOkRelease(void);
    uint16_t getVisibleScreen();
    virtual void updateProgressRange(int min, int max);
    bool isPaused();
    bool isPauseAvailable();
private:
    RunningPresenter();

    RunningView& view;
};

#endif // RUNNINGPRESENTER_HPP
