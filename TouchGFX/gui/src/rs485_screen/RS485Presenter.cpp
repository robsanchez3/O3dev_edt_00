#include <gui/rs485_screen/RS485View.hpp>
#include <gui/rs485_screen/RS485Presenter.hpp>

RS485Presenter::RS485Presenter(RS485View& v)
    : view(v)
{
}

void RS485Presenter::activate()
{

}

void RS485Presenter::deactivate()
{

}

void RS485Presenter::BacklightSliderValueChanged(int value)
{
// Report to presenter and update test field
  model->SendRS485(value);
}