#ifndef SELECTORVIEW_HPP
#define SELECTORVIEW_HPP

#include <gui_generated/selector_screen/SelectorViewBase.hpp>
#include <gui/selector_screen/SelectorPresenter.hpp>

#include <gui/mainmenu_screen/MainMenuView.hpp>

#define UNITS_BLINK_DELAY	    (int16_t)(250/16.66)  // 1 tick =~ 16.66 ms
#define UNITS_BLINK_REPALY_MAX	2

#define TWO_DIGITS_WIDTH	    60		// Experimental values for a Verdana 25px font
#define THREE_DIGITS_WIDTH	    80
#define FOUR_DIGITS_WIDTH	    100
#define FIVE_DIGITS_WIDTH	    120


struct UNITS_BLINK_CTX{
	bool    state;
    int16_t replay;
    int16_t count;
};

class SelectorView : public SelectorViewBase
{
public:
    SelectorView();
    virtual ~SelectorView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    virtual void handleClickEvent(const ClickEvent& event);



    virtual void sw_selectorUpdateItem(sw_selectContainer& item, int16_t itemIndex);
    virtual void sw_selectorUpdateCenterItem(sw_selectionContainer& item, int16_t itemIndex);
    void okClicked();
    void cancelClicked();
    void sl_slider_changed(int value);
    void forwardClicked();
    void backClicked();

    void initTherapyContext(THERAPY_CTX *ctx);
    void initSelectionContext(int8_t step);
    int16_t getSelectorItemWidth();
    int bufferToInt(const Unicode::UnicodeChar* buffer);

    void thousandsSep(Unicode::UnicodeChar *);

    int get_logical_value(int value);
    int get_physical_value(int value);
    void update_selection(int value);
    void update_second_selection();

    THERAPY_CTX *therapyCtx = NULL;
    int8_t selectionStep;
    bool selectorViewStarted;
    int16_t selectorItemWidth = 0;
    int logicalValue = 0;
    int physicalValue = 0;

    uint8_t accelCounter = 5;
    uint32_t tickCounter = 0;

    UNITS_BLINK_CTX blinkUnits;
protected:
};

#endif // SELECTORVIEW_HPP
