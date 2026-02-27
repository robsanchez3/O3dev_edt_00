#ifndef HID_VIEW_HPP
#define HID_VIEW_HPP

#include <gui_generated/hid_screen/HIDViewBase.hpp>
#include <gui/hid_screen/HIDPresenter.hpp>

class HIDView : public HIDViewBase
{
public:
    HIDView();
    virtual ~HIDView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // HID_VIEW_HPP
