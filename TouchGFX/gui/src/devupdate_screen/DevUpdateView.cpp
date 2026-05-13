#include <gui/devupdate_screen/DevUpdateView.hpp>
#include <gui/devupdate_screen/DevUpdatePresenter.hpp>
#include <touchgfx/Unicode.hpp>
#include <touchgfx/Bitmap.hpp>
#include <images/BitmapDatabase.hpp>

DevUpdateView::DevUpdateView() : m_lastBitmapId(0xFFFF)
{
}

void DevUpdateView::setupScreen()
{
    DevUpdateViewBase::setupScreen();
}

void DevUpdateView::tearDownScreen()
{
    DevUpdateViewBase::tearDownScreen();
}

void DevUpdateView::updateInfo(const char* asciiMsg)
{
    uint16_t i = 0;
    while (asciiMsg[i] && i < TA_INFO_SIZE - 1) {
        ta_infoBuffer[i] = (Unicode::UnicodeChar)(uint8_t)asciiMsg[i];
        i++;
    }
    ta_infoBuffer[i] = 0;
    ta_info.invalidate();
}

void DevUpdateView::showButtons(bool showOk, bool showCancel)
{
    bt_OK.setVisible(showOk);
    bt_cancel.setVisible(showCancel);
    bt_OK.invalidate();
    bt_cancel.invalidate();
}

void DevUpdateView::showImage(uint16_t bitmapId)
{
    if (m_lastBitmapId == bitmapId)
        return;
    m_lastBitmapId = bitmapId;
    si_update.setBitmap(Bitmap(bitmapId));
    si_update.invalidate();
}

void DevUpdateView::cancelClicked()
{
    presenter->cancelClicked();
}

void DevUpdateView::okClicked1()
{
    presenter->okClicked1();
}
