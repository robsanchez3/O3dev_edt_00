#ifndef GPIO_PRESENTER_HPP
#define GPIO_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class GPIOView;

class GPIOPresenter : public Presenter, public ModelListener
{
public:
    GPIOPresenter(GPIOView& v);

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

    virtual ~GPIOPresenter() {};

    void activateGPIO4();
    void SetGPIO5(bool value);
    //void PresetToggleGPIO5(bool value);
    void PopUpWindowGPIO1();
    void ShowGPIO_0_State();
private:
    GPIOPresenter();

    GPIOView& view;
};


#endif // GPIO_PRESENTER_HPP
