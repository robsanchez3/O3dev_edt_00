#ifndef CAN_PRESENTER_HPP
#define CAN_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class CANView;

class CANPresenter : public Presenter, public ModelListener
{
public:
    CANPresenter(CANView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();
    
    void CANSliderChanged(int value);
    void CANReceived(uint8_t *data);
    

    virtual ~CANPresenter() {};

private:
    CANPresenter();

    CANView& view;
};


#endif // CAN_PRESENTER_HPP
