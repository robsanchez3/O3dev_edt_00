#ifndef ERRORVIEW_HPP
#define ERRORVIEW_HPP

#include <gui_generated/error_screen/ErrorViewBase.hpp>
#include <gui/error_screen/ErrorPresenter.hpp>

#define ICON_TEXT_SPACE	10

class ErrorView : public ErrorViewBase
{
public:
    ErrorView();
    virtual ~ErrorView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void initTherapyContext(THERAPY_CTX *ctx);
    uint16_t getTherapyTargetValue(uint8_t valueID);
    void initTherapyDataInfo();
    void initErrorCode();
    void cnlClicked();

    THERAPY_CTX *therapyCtx;
protected:
};

#endif // ERRORVIEW_HPP
