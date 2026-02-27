#ifndef CDC_PRESENTER_HPP
#define CDC_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class CDCView;

class CDCPresenter : public Presenter, public ModelListener
{
public:
    CDCPresenter(CDCView& v);

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

    virtual ~CDCPresenter() {};
    virtual void SetUSBCDCSliderValue(uint8_t value);

private:
    CDCPresenter();

    CDCView& view;
};


#endif // CDC_PRESENTER_HPP
