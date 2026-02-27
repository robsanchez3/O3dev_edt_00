#ifndef CAN_VIEW_HPP
#define CAN_VIEW_HPP

#include <gui_generated/can_screen/CANViewBase.hpp>
#include <gui/can_screen/CANPresenter.hpp>

class CANView : public CANViewBase
{
public:
    CANView();
    virtual ~CANView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    void CANSliderChanged(int value);
    void ModelCanToView(uint8_t *data);
    virtual void Local_Selected();
    virtual void Remote_Selected();
    
protected:
  
private:
  uint8_t size=0;
  uint8_t RX_Buffer[9];  
};

#endif // CAN_VIEW_HPP
