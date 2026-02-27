#ifndef SPI_VIEW_HPP
#define SPI_VIEW_HPP

#include <gui_generated/spi_screen/SPIViewBase.hpp>
#include <gui/spi_screen/SPIPresenter.hpp>

class SPIView : public SPIViewBase
{
public:
    SPIView();
    virtual ~SPIView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SPI_VIEW_HPP
