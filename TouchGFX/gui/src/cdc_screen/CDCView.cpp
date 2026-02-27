#include <gui/cdc_screen/CDCView.hpp>

CDCView::CDCView()
{

}

void CDCView::setupScreen()
{
    CDCViewBase::setupScreen();
}

void CDCView::tearDownScreen()
{
    CDCViewBase::tearDownScreen();
}

void CDCView::USBCDCSendSliderValue(int value)
{

    Unicode::snprintf(USBCDCSliderValueBuffer, sizeof (USBCDCSliderValue), "%d", value);
    USBCDCSliderValue.invalidate();
    presenter->SetUSBCDCSliderValue(value);

}
