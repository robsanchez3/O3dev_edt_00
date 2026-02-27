#ifndef INFO_VIEW_HPP
#define INFO_VIEW_HPP

#include <gui_generated/info_screen/INFOViewBase.hpp>
#include <gui/info_screen/INFOPresenter.hpp>

class INFOView : public INFOViewBase
{
public:
    INFOView();
    virtual ~INFOView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // INFO_VIEW_HPP
