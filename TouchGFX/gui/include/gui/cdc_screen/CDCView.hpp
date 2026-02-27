#ifndef CDC_VIEW_HPP
#define CDC_VIEW_HPP

#include <gui_generated/cdc_screen/CDCViewBase.hpp>
#include <gui/cdc_screen/CDCPresenter.hpp>

class CDCView : public CDCViewBase
{
public:
    CDCView();
    virtual ~CDCView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void USBCDCSendSliderValue(int value);
protected:
    
};

#endif // CDC_VIEW_HPP
