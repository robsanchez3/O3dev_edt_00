#ifndef CALIBRATIONVIEW_HPP
#define CALIBRATIONVIEW_HPP

#include <gui_generated/calibration_screen/CalibrationViewBase.hpp>
#include <gui/calibration_screen/CalibrationPresenter.hpp>

class CalibrationView : public CalibrationViewBase
{
public:
    CalibrationView();
    virtual ~CalibrationView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    void calPressClicked();
    void calFlowClicked();
    void calO3Clicked();
    void saveParamClicked();
    void generationTypeClicked();
    void swVersionClicked();
    void pressTempClicked();
    void showParamClicked();
    void loadParamClicked();
    void calPeriodClicked();
    void showStartClicked();
    void pinPadClicked_0();
    void pinPadClicked_1();
    void pinPadClicked_2();
    void pinPadClicked_3();
    void pinPadClicked_4();
    void pinPadClicked_5();
    void pinPadClicked_6();
    void pinPadClicked_7();
    void pinPadClicked_8();
    void pinPadClicked_9();
    void pinPadClicked_del();
    void pinPadClicked_ok();
    void cancelClicked();
    void clearPin();
    void showPinValidationResult(bool success);
    void hidePinpad();
    void showPinpad();


protected:

    // pin edition
    static const uint8_t MAX_PIN_DIGITS = 4;
    static const uint8_t PIN_BUFFER_SIZE = MAX_PIN_DIGITS + 1;
    static const uint8_t DISPLAY_BUFFER_SIZE = (MAX_PIN_DIGITS * 2); // including spaces
    Unicode::UnicodeChar pinRaw[PIN_BUFFER_SIZE];         // raw pin input
    Unicode::UnicodeChar pinDisplay[DISPLAY_BUFFER_SIZE]; //
    uint8_t pinLength;
    void appendDigit(uint8_t digit);
    void deleteLastDigit();
    void updatePinDisplay();
    void validatePin();

    // blink control
    static const uint8_t MAX_BLINKS = 8;
    bool blinking = false;
    uint8_t blinkCount = 0;
    colortype originalColor;
    void showPinError();
};

#endif // CALIBRATIONVIEW_HPP
