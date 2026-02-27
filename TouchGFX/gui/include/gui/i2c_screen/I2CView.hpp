#ifndef I2C_VIEW_HPP
#define I2C_VIEW_HPP

#include <gui_generated/i2c_screen/I2CViewBase.hpp>
#include <gui/i2c_screen/I2CPresenter.hpp>

class I2CView : public I2CViewBase
{
public:
    I2CView();
    virtual ~I2CView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    
    virtual void CountUp();
    virtual void CountDown();
    virtual void ManualRead();
    virtual void ContinuousRead();
    virtual void SetCount(uint8_t CountValue);
    virtual void SetLimitsEffects(bool BelowUpper, bool AboveLower);
    virtual void handleTickEvent();
    virtual void setTemperature(float temp);
    
protected:
  private:
  uint8_t count = 3;
  uint8_t ReadRate;
  bool ContinuousReadState;
  bool visible;
};

#endif // I2C_VIEW_HPP
