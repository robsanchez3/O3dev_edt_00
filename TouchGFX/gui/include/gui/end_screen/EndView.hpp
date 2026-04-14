#ifndef ENDVIEW_HPP
#define ENDVIEW_HPP

#include <gui_generated/end_screen/EndViewBase.hpp>
#include <gui/end_screen/EndPresenter.hpp>

#define ICON_TEXT_SPACE	10

class EndView : public EndViewBase
{
public:
    EndView();
    virtual ~EndView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void initTherapyContext(THERAPY_CTX *ctx);
    uint16_t getTherapyTargetValue(uint8_t valueID);
    void initTherapyDataInfo();
    void initTherapyEndInfo();
    void initVisibleItems();
    void okClicked();
    void cnlClicked();

    THERAPY_CTX *therapyCtx;
protected:
    uint16_t two_items_top_text_y;
    uint16_t two_items_down_text_y;
};

#endif // ENDVIEW_HPP
