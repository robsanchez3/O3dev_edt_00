#ifndef RS485_VIEW_HPP
#define RS485_VIEW_HPP

#include <gui_generated/rs485_screen/RS485ViewBase.hpp>
#include <gui/rs485_screen/RS485Presenter.hpp>

class RS485View : public RS485ViewBase
{
public:
    RS485View();
    virtual ~RS485View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    
    virtual void BacklightSliderValueChanged(int value);    
protected:
};

#endif // RS485_VIEW_HPP
