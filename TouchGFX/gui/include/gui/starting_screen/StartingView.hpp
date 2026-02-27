#ifndef STARTINGVIEW_HPP
#define STARTINGVIEW_HPP

#include <gui_generated/starting_screen/StartingViewBase.hpp>
#include <gui/starting_screen/StartingPresenter.hpp>

class StartingView : public StartingViewBase
{
public:
    StartingView();
    virtual ~StartingView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void handleTickEvent();
    void settingsClicked();

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
    uint32_t timeoutCounter;
    bool timeoutActive;
protected:
};

#endif // STARTINGVIEW_HPP
