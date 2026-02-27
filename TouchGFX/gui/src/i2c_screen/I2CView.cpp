#include <gui/i2c_screen/I2CView.hpp>
#include "BitmapDatabase.hpp"
#include <touchgfx/hal/Types.hpp>

I2CView::I2CView()
{

}

void I2CView::setupScreen()
{
    I2CViewBase::setupScreen();
    ContinuousReadState = false;
}

void I2CView::tearDownScreen()
{
    I2CViewBase::tearDownScreen();
}

void I2CView::ManualRead()
{
  ContinuousReadState = ContinuousReadOnOff.getState();
  if (ContinuousReadState)
  {  
    ModalWindow.setXY(0,0);  // Avoid misplacement
    ModalWindow.show();
  }
  else
    presenter->ReadTemperatureOnce();
}

void I2CView::CountUp()
{
  if (count < 5)
  {
    count++;
    SetCount(count);
    if (count == 5)
    {
      SetLimitsEffects(false, true);      
    }
    else if (count == 2)
    {
      SetLimitsEffects(true, true);      
    }  
  }  
}

void I2CView::CountDown()
{
  if (count > 1)
  {    count--;
    SetCount(count);
    if (count == 1)
    {
      SetLimitsEffects(true, false);      
    }
    else if (count == 4)
    {
      SetLimitsEffects(true, true);      
    }  
  } 
}

void I2CView:: ContinuousRead()
{
   ContinuousReadState = ContinuousReadOnOff.getState();
   if (ContinuousReadState)
   {
    // ContReadText.setColor(touchgfx::Color::getColorFrom24BitRGB(255,0,0));
     ContReadText.invalidate();
     ReadRate = 60/count;
     visible = false;
   }
   else
   {
//     ContReadText.setColor(touchgfx::Color::getColorFrom24BitRGB(255,255,255));
     ContReadText.setVisible(true);
     ContReadText.invalidate();
     ReadRate = 0;
   }
}
void I2CView::handleTickEvent()
{
   if (ContinuousReadState)
   {
     if (ReadRate > 0)
     {
       ReadRate--;
       if (ReadRate == 0)
       {
         ReadRate = 60/count;
         presenter->ReadTemperatureOnce();
         if (visible)
         {  
             ContReadText.setVisible(true);
             ContReadText.invalidate();
             visible = false;
         }
         else
         {  
             ContReadText.setVisible(false);
             ContReadText.invalidate();
             visible = true;
         }           
       }
     }
   }
}


void I2CView::SetCount(uint8_t CountValue)
{
  Unicode::snprintf(SamplesPerSecondBuffer, 3, "%d", CountValue);
  SamplesPerSecond.invalidate();
}
      
void I2CView:: SetLimitsEffects(bool BelowUpper, bool AboveLower)
{
  if (BelowUpper)
  {
    CountUpButton.setBitmaps(Bitmap(BITMAP_UP_BTN_ID), Bitmap(BITMAP_UP_BTN_PRESSED_ID));
  }
  else
  {
    CountUpButton.setBitmaps(Bitmap(BITMAP_UP_BTN_DISABLED_ID), Bitmap(BITMAP_UP_BTN_DISABLED_ID));
  }  
  if (AboveLower)
  {
     CountDownButton.setBitmaps(Bitmap(BITMAP_DOWN_BTN_ID), Bitmap(BITMAP_DOWN_BTN_PRESSED_ID));
  }
  else
  {
     CountDownButton.setBitmaps(Bitmap(BITMAP_DOWN_BTN_DISABLED_ID), Bitmap(BITMAP_DOWN_BTN_DISABLED_ID));
  }    
  CountDownButton.invalidate();
  CountUpButton.invalidate();
}   
    
void I2CView::setTemperature(float temp)
{
// Update textfield for actual water temperature  
#ifndef SIMULATOR
  Unicode::snprintfFloat(ReadTemperatureBuffer, 6, "% 2.1f",temp);
  ReadTemperature.invalidate();
#endif  
}
