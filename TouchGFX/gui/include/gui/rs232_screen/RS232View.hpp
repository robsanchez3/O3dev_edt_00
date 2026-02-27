#ifndef RS232_VIEW_HPP
#define RS232_VIEW_HPP

#include <gui_generated/rs232_screen/RS232ViewBase.hpp>
#include <gui/rs232_screen/RS232Presenter.hpp>



class data
{  
      public:
          uint8_t  RxDataBuffer[512];
          uint16_t RxDataCount;
          uint16_t RxDataIndex;
};

class RS232View : public RS232ViewBase
{
public:
    RS232View();
    virtual ~RS232View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    
    void function1(int value);
    void RS232SliderValueChanged(int value);
    void RS232CharReceived(uint8_t *data , uint16_t len);
    
protected:
  
private:
  uint16_t n = 0;
  uint16_t i = 0, j=0;  
  data rs232data;
  
};


#endif // RS232_VIEW_HPP
