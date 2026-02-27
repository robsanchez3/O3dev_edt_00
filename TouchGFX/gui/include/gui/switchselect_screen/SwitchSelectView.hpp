#ifndef SWITCHSELECTVIEW_HPP
#define SWITCHSELECTVIEW_HPP

#include <gui_generated/switchselect_screen/SwitchSelectViewBase.hpp>
#include <gui/switchselect_screen/SwitchSelectPresenter.hpp>

class SwitchSelectView : public SwitchSelectViewBase
{
public:
    SwitchSelectView();
    virtual ~SwitchSelectView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    void cancelClicked();
    void okClicked();
    void generationModeClicked();
protected:
    bool generationModeOnEntry;
};

#endif // SWITCHSELECTVIEW_HPP
