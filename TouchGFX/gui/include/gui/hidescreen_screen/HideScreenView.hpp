#ifndef HIDESCREENVIEW_HPP
#define HIDESCREENVIEW_HPP

#include <gui_generated/hidescreen_screen/HideScreenViewBase.hpp>
#include <gui/hidescreen_screen/HideScreenPresenter.hpp>

class HideScreenView : public HideScreenViewBase
{
public:
    HideScreenView();
    virtual ~HideScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // HIDESCREENVIEW_HPP
