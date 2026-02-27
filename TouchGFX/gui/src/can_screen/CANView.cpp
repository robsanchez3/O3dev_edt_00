#include <gui/can_screen/CANView.hpp>
#include "string.h"
#include <stdio.h>

#ifndef SIMULATOR
extern "C"
{
  #include "edt_f7xx_can.h"
}
#endif

extern bool CANHOST;


CANView::CANView()
{

}

void CANView::setupScreen()
{
    CANViewBase::setupScreen();
    Local.setSelected (true);
}

void CANView::tearDownScreen()
{
    CANViewBase::tearDownScreen();
}

void CANView::CANSliderChanged(int value)
{
  presenter -> CANSliderChanged(value); 
}

void CANView::ModelCanToView(uint8_t *data)
{
#ifndef SIMULATOR  
  memset(CANRxBuffer,0,sizeof(CANRxBuffer));  
  Unicode::strncpy(CANRxBuffer, (char*)data, 8);
  CANRx.invalidate();    
#endif  
}

void CANView::Local_Selected()
{
   CANHOST = 1;
}

void CANView::Remote_Selected()
{
  CANHOST = 0;
}