#ifndef GENUPDATEVIEW_HPP
#define GENUPDATEVIEW_HPP

#include <gui_generated/genupdate_screen/GenUpdateViewBase.hpp>
#include <gui/genupdate_screen/GenUpdatePresenter.hpp>

class GenUpdateView : public GenUpdateViewBase
{
public:
    GenUpdateView();
    virtual ~GenUpdateView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    /* Update ta_info text (ASCII string converted to Unicode). */
    void updateInfo(const char* asciiMsg);

    /* Show or hide the OK and Cancel buttons. */
    void showButtons(bool showOk, bool showCancel);

    /* Button callbacks — delegate to presenter. */
    virtual void cancelClicked();
    virtual void okClicked1();

protected:
};

#endif // GENUPDATEVIEW_HPP
