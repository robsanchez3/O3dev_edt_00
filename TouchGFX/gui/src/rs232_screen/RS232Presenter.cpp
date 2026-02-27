#include <gui/rs232_screen/RS232View.hpp>
#include <gui/rs232_screen/RS232Presenter.hpp>
//#include <TestO3.h>
#include <stdio.h>

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

char debugStringBuffer[30];

void RS232Presenter::function1(int value)
{
//	TestO3();

	touchgfx_printf("(Presenter file) Hi everyone !!!!! Slider value: %d\n", value);
	snprintf(debugStringBuffer, sizeof(debugStringBuffer), "tick: %d", value);
	printf("Joool3\r\n");
	printf ("SWO test!\r\n");
	Application::getDebugPrinter()->setString(debugStringBuffer);
//	Application::getDebugPrinter()->setString("Test Debug...");
	Application::invalidateDebugRegion();
}

void RS232Presenter::RS232SliderValueChanged(uint8_t value)
{
  model->SendRS232(value);
}
  
void RS232Presenter::RS232CharReceived(uint8_t *data , uint16_t len)
{
  view.RS232CharReceived(data,len);
}

