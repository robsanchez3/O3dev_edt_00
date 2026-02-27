#include <gui/cdc_screen/CDCView.hpp>
#include <gui/cdc_screen/CDCPresenter.hpp>

CDCPresenter::CDCPresenter(CDCView& v)
    : view(v)
{
}

void CDCPresenter::activate()
{

}

void CDCPresenter::deactivate()
{

}

void CDCPresenter::SetUSBCDCSliderValue(uint8_t value)
{
    model->USBCDCSend(value);
}
