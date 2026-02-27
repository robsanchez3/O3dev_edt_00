#include <gui/rs485_screen/RS485View.hpp>

RS485View::RS485View()
{

}

void RS485View::setupScreen()
{
    RS485ViewBase::setupScreen();
}

void RS485View::tearDownScreen()
{
    RS485ViewBase::tearDownScreen();
}

void RS485View::BacklightSliderValueChanged(int value)
{
// Update text field and report to presenter
    Unicode::snprintf(BacklightSliderValueBuffer, sizeof(BacklightSliderValue), "%d", value);
    BacklightSliderValue.invalidate();
    presenter->BacklightSliderValueChanged(value);
}