#include <gui/gpio_screen/GPIOView.hpp>
#include <gui/gpio_screen/GPIOPresenter.hpp>

GPIOPresenter::GPIOPresenter(GPIOView& v)
    : view(v)
{
}

void GPIOPresenter::activate()
{

}

void GPIOPresenter::deactivate()
{

}

void GPIOPresenter::activateGPIO4()
{
  model->SetGPIO4Active(); 
}

void GPIOPresenter::SetGPIO5(bool value)
{
  model->SetGPIO5(value);
}

void GPIOPresenter::PopUpWindowGPIO1()
{
// Activate Modal Window
  view.OpenPopUpWindowGPIO1();  
}
void GPIOPresenter::ShowGPIO_0_State()
{
  view.ShowGPIO_0_State();  
}