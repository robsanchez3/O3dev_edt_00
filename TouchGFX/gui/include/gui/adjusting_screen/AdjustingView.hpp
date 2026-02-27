#ifndef ADJUSTINGVIEW_HPP
#define ADJUSTINGVIEW_HPP

#include <gui_generated/adjusting_screen/AdjustingViewBase.hpp>
#include <gui/adjusting_screen/AdjustingPresenter.hpp>

#define ICON_TEXT_SPACE	10

class AdjustingView : public AdjustingViewBase
{
public:
    AdjustingView();
    virtual ~AdjustingView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    void okClicked();
    void cnlClicked();

    void initTherapyContext(THERAPY_CTX *ctx);
    uint16_t getTherapyTargetValue(uint8_t valueID);
    void initTherapyDataInfo();

    THERAPY_CTX *therapyCtx;

private:
    float xAngle3D;
    float yAngle3D;
    float zAngle3D;
    float deltaXangle3D;
    float deltaYangle3D;
    float deltaZangle3D;
    int step;
    int delay;
    bool stop;

protected:
};

#endif // ADJUSTINGVIEW_HPP
