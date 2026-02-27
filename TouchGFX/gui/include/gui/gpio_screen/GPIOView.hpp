#ifndef GPIO_VIEW_HPP
#define GPIO_VIEW_HPP

#include <gui_generated/gpio_screen/GPIOViewBase.hpp>
#include <gui/gpio_screen/GPIOPresenter.hpp>

class GPIOView : public GPIOViewBase
{
public:
    GPIOView();
    virtual ~GPIOView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    void GPIO4Active();
    void ToggleGPIO5();
    void OpenPopUpWindowGPIO1();
    void ShowGPIO_0_State();    
protected:
};

#endif // GPIO_VIEW_HPP
