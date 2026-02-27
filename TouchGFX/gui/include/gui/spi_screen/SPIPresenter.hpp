#ifndef SPI_PRESENTER_HPP
#define SPI_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SPIView;

class SPIPresenter : public Presenter, public ModelListener
{
public:
    SPIPresenter(SPIView& v);

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

    virtual ~SPIPresenter() {};

private:
    SPIPresenter();

    SPIView& view;
};


#endif // SPI_PRESENTER_HPP
