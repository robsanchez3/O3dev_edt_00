#include <gui/gpio_screen/GPIOView.hpp>

GPIOView::GPIOView()
{

}

void GPIOView::setupScreen()
{
    GPIOViewBase::setupScreen();
    ModalWindow.setXY(0,0);  // Default initial placement, avoid when component does not exist
}

void GPIOView::tearDownScreen()
{
    GPIOViewBase::tearDownScreen();
}

void GPIOView::GPIO4Active()
{
  presenter-> activateGPIO4();  
}

void GPIOView::ToggleGPIO5()
{
  presenter->SetGPIO5(GPIO5ToggleButton.getState());
}

void GPIOView::OpenPopUpWindowGPIO1()
{
//  ModalWindow.setXY(0,0);  // Avoid misplacement，move to setupScreen avoid when component does not exist
  ModalWindow.show();  
}

void GPIOView::ShowGPIO_0_State()
{
  BulbYellow.setVisible(!BulbYellow.isVisible());
  BulbYellow.invalidate();  
}