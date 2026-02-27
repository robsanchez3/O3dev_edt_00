#ifndef I2C_PRESENTER_HPP
#define I2C_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class I2CView;

class I2CPresenter : public Presenter, public ModelListener
{
public:
    I2CPresenter(I2CView& v);

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

    virtual ~I2CPresenter() {};
    
    virtual void ReadTemperatureOnce();
    virtual void GetModelTemperature();     

private:
    I2CPresenter();

    I2CView& view;
};


#endif // I2C_PRESENTER_HPP
