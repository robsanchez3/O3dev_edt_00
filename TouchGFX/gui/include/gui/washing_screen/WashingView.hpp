#ifndef WASHINGVIEW_HPP
#define WASHINGVIEW_HPP

#include <gui_generated/washing_screen/WashingViewBase.hpp>
#include <gui/washing_screen/WashingPresenter.hpp>

#define ICON_TEXT_SPACE	10

class WashingView : public WashingViewBase
{
public:
    WashingView();
    virtual ~WashingView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    void cnlClicked();
    void okClicked();

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

#endif // WASHINGVIEW_HPP
