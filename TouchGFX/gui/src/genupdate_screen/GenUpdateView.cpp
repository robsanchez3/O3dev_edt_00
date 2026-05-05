#include <gui/genupdate_screen/GenUpdateView.hpp>
#include <gui/genupdate_screen/GenUpdatePresenter.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Unicode.hpp>

GenUpdateView::GenUpdateView()
{
}

void GenUpdateView::setupScreen()
{
    GenUpdateViewBase::setupScreen();
}

void GenUpdateView::tearDownScreen()
{
    GenUpdateViewBase::tearDownScreen();
}

void GenUpdateView::updateInfo(const char* asciiMsg)
{
    uint16_t i = 0;
    while (asciiMsg[i] && i < TA_INFO_SIZE - 1) {
        ta_infoBuffer[i] = (Unicode::UnicodeChar)(uint8_t)asciiMsg[i];
        i++;
    }
    ta_infoBuffer[i] = 0;
    ta_info.invalidate();
}

void GenUpdateView::showButtons(bool showOk, bool showCancel)
{
    bt_OK.setVisible(showOk);
    bt_cancel.setVisible(showCancel);
    bt_OK.invalidate();
    bt_cancel.invalidate();
}

void GenUpdateView::cancelClicked()
{
    presenter->cancelClicked();
}

void GenUpdateView::okClicked1()
{
    presenter->okClicked1();
}
