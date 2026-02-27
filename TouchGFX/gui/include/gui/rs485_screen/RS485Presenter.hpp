#ifndef RS485_PRESENTER_HPP
#define RS485_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class RS485View;

class RS485Presenter : public Presenter, public ModelListener
{
public:
    RS485Presenter(RS485View& v);

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

    virtual ~RS485Presenter() {};
    
    virtual void BacklightSliderValueChanged(int value);

private:
    RS485Presenter();

    RS485View& view;
};


#endif // RS485_PRESENTER_HPP
