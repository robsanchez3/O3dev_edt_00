#ifndef OZTDVIEW_HPP
#define OZTDVIEW_HPP

#include <gui_generated/oztd_screen/OZTDViewBase.hpp>
#include <gui/oztd_screen/OZTDPresenter.hpp>

class OZTDView : public OZTDViewBase
{
public:
    OZTDView();
    virtual ~OZTDView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    void StartGeneration(void);
    void StopGeneration(void);
    void UpdateTargetFlow(int);
    void FlowSliderValueChanged(int);
protected:
};

#endif // OZTDVIEW_HPP
