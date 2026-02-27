#include <gui/can_screen/CANView.hpp>
#include <gui/can_screen/CANPresenter.hpp>

CANPresenter::CANPresenter(CANView& v)
    : view(v)
{
}

void CANPresenter::activate()
{

}

void CANPresenter::deactivate()
{

}

void CANPresenter::CANSliderChanged(int value)
{
  model -> SendCAN(value);  
}

void CANPresenter::CANReceived(uint8_t *data)
{
#ifndef SIMULATOR
   view.ModelCanToView(data);
#endif
}