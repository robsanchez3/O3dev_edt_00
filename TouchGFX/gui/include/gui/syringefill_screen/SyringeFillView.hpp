#ifndef SYRINGEFILLVIEW_HPP
#define SYRINGEFILLVIEW_HPP

#include <gui_generated/syringefill_screen/SyringeFillViewBase.hpp>
#include <gui/syringefill_screen/SyringeFillPresenter.hpp>

#define ICON_TEXT_SPACE	10

class SyringeFillView : public SyringeFillViewBase
{
public:
    SyringeFillView();
    virtual ~SyringeFillView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    void cnlClicked();
    virtual void handleClickEvent(const ClickEvent& evt);

    void test_quitar();

    void initTherapyContext(THERAPY_CTX *ctx);
    uint16_t getTherapyTargetValue(uint8_t valueID);
    void initTherapyDataInfo();


    void startAnimation();
    void initVisualElemnets();
    int16_t computeAnimationSteps();

    THERAPY_CTX *therapyCtx;

protected:
};

#endif // SYRINGEFILLVIEW_HPP
