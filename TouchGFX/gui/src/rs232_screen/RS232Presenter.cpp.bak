#include <gui/rs232_screen/RS232View.hpp>
#include <gui/rs232_screen/RS232Presenter.hpp>

RS232Presenter::RS232Presenter(RS232View& v)
    : view(v)
{
}

void RS232Presenter::activate()
{

}

void RS232Presenter::deactivate()
{

}

void RS232Presenter::RS232SliderValueChanged(uint8_t value)
{
  model->SendRS232(value);
}
  
void RS232Presenter::RS232CharReceived(uint8_t *data , uint16_t len)
{
  view.RS232CharReceived(data,len);
}
