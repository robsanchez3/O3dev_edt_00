#ifndef DEVUPDATEVIEW_HPP
#define DEVUPDATEVIEW_HPP

#include <gui_generated/devupdate_screen/DevUpdateViewBase.hpp>
#include <gui/devupdate_screen/DevUpdatePresenter.hpp>

class DevUpdateView : public DevUpdateViewBase
{
public:
    DevUpdateView();
    virtual ~DevUpdateView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void updateInfo(const char* asciiMsg);
    void showButtons(bool showOk, bool showCancel);
    void showImage(uint16_t bitmapId);

    virtual void cancelClicked();
    virtual void okClicked1();
protected:
    uint16_t m_lastBitmapId;
};

#endif // DEVUPDATEVIEW_HPP
