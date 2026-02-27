#ifndef RUNNINGVIEW_HPP
#define RUNNINGVIEW_HPP

#include <gui_generated/running_screen/RunningViewBase.hpp>
#include <gui/running_screen/RunningPresenter.hpp>

#define ICON_TEXT_SPACE	10

class RunningView : public RunningViewBase
{
public:
    RunningView();
    virtual ~RunningView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    virtual void handleClickEvent(const ClickEvent& evt);

    void cnlClicked();
    void okClicked();
    void bigPauseClicked();
    void manageBigPauseView();

    void initTherapyContext(THERAPY_CTX *ctx);
    uint16_t getTherapyTargetValue(uint8_t valueID);
    void initTherapyDataInfo();
    void updatePressureBar();
    void InitRunningContext();
    void InitProgressInfo();
    void InitProgressGraphic(int min, int max);

    THERAPY_CTX *therapyCtx;
protected:
};

#endif // RUNNINGVIEW_HPP
