#include <gui/calibration_screen/CalibrationView.hpp>
#include <BitmapDatabase.hpp>
//#include "stm32f7xx_hal.h"  TODO clean when validated
#include "stm32u5xx_hal.h" // TODO needed for HAL_NVIC_SystemReset(); probably better if implemented on model.cc

CalibrationView::CalibrationView() : pinLength(0)
{
	//
	ta_pin.setWildcard(pinDisplay);
	Unicode::snprintf(pinRaw, PIN_BUFFER_SIZE, "%s", "");
}

void CalibrationView::setupScreen()
{
    CalibrationViewBase::setupScreen();
    originalColor = ta_pin.getColor();
    hidePinpad();

    if(presenter->configOptionsAllowed())
	{
        si_locked.setBitmap(touchgfx::Bitmap(BITMAP_LOCK_OPEN_100_ID));
	}
	else
	{
	    si_locked.setBitmap(touchgfx::Bitmap(BITMAP_LOCK_100_ID));
	}
    si_locked.invalidate();

    if(presenter->isGenerationModeAvailable() == false)
	{
    	bt_save_param.setXY(bt_gen_type.getX(), bt_gen_type.getY());
    	bt_gen_type.setVisible(false);
    	printf("CalibrationView, no generation type option available.\n");
	}
}

void CalibrationView::tearDownScreen()
{
    CalibrationViewBase::tearDownScreen();
}


void CalibrationView::handleTickEvent()
{
    CalibrationViewBase::handleTickEvent();

    static int tickCounter = 0;
    if (blinking)
    {
        tickCounter++;

        // change color every 18 ticks approx. (~300ms at 60fps)
        if (tickCounter >= 8)
        {
            tickCounter = 0;
            // alternate color
            if (blinkCount % 2 == 0)
            {
                ta_pin.setColor(0xF80000); // red color
            }
            else
            {
                ta_pin.setColor(originalColor);
            }
            ta_pin.invalidate();
            blinkCount++;
            if (blinkCount >= MAX_BLINKS)
            {
                blinking = false;
            }
        }
    }
}

void CalibrationView::showPinpad()
{
	clearPin();

	bx_pinPadBack.setVisible(true);
	bt_pinpad_0.setVisible(true);
	bt_pinpad_1.setVisible(true);
	bt_pinpad_2.setVisible(true);
	bt_pinpad_3.setVisible(true);
	bt_pinpad_4.setVisible(true);
	bt_pinpad_5.setVisible(true);
	bt_pinpad_6.setVisible(true);
	bt_pinpad_7.setVisible(true);
	bt_pinpad_8.setVisible(true);
	bt_pinpad_9.setVisible(true);
	bt_pinpad_del.setVisible(true);
	bt_pinpad_ok.setVisible(true);
	ta_pin.setVisible(true);

	bx_pinPadBack.invalidate();
	bt_pinpad_0.invalidate();
	bt_pinpad_1.invalidate();
	bt_pinpad_2.invalidate();
	bt_pinpad_3.invalidate();
	bt_pinpad_4.invalidate();
	bt_pinpad_5.invalidate();
	bt_pinpad_6.invalidate();
	bt_pinpad_7.invalidate();
	bt_pinpad_8.invalidate();
	bt_pinpad_9.invalidate();
	bt_pinpad_del.invalidate();
	bt_pinpad_ok.invalidate();
	ta_pin.invalidate();
}

void CalibrationView::hidePinpad()
{
	bx_pinPadBack.setVisible(false);
	bt_pinpad_0.setVisible(false);
	bt_pinpad_1.setVisible(false);
	bt_pinpad_2.setVisible(false);
	bt_pinpad_3.setVisible(false);
	bt_pinpad_4.setVisible(false);
	bt_pinpad_5.setVisible(false);
	bt_pinpad_6.setVisible(false);
	bt_pinpad_7.setVisible(false);
	bt_pinpad_8.setVisible(false);
	bt_pinpad_9.setVisible(false);
	bt_pinpad_del.setVisible(false);
	bt_pinpad_ok.setVisible(false);
	ta_pin.setVisible(false);

	bx_pinPadBack.invalidate();
	bt_pinpad_0.invalidate();
	bt_pinpad_1.invalidate();
	bt_pinpad_2.invalidate();
	bt_pinpad_3.invalidate();
	bt_pinpad_4.invalidate();
	bt_pinpad_5.invalidate();
	bt_pinpad_6.invalidate();
	bt_pinpad_7.invalidate();
	bt_pinpad_8.invalidate();
	bt_pinpad_9.invalidate();
	bt_pinpad_del.invalidate();
	bt_pinpad_ok.invalidate();
	ta_pin.invalidate();
}

void CalibrationView::updatePinDisplay()
{
    // format pin with spaces
    int displayIndex = 0;
    for (int i = 0; i < pinLength; i++)
    {
        pinDisplay[displayIndex++] = pinRaw[i];
        if (i < pinLength - 1)
        {
            pinDisplay[displayIndex++] = ' ';  // space between digits
        }
    }
    pinDisplay[displayIndex] = '\0'; // null-terminate

    ta_pin.invalidate();
}

void CalibrationView::appendDigit(uint8_t digit)
{
    if (pinLength < MAX_PIN_DIGITS)
    {
        pinRaw[pinLength++] = '0' + digit;
        pinRaw[pinLength] = '\0';
        updatePinDisplay();

        if (pinLength == MAX_PIN_DIGITS)
        {
           // presenter->validatePin(pinRaw);
        }
    }
}

void CalibrationView::deleteLastDigit()
{
    if (pinLength > 0)
    {
        pinLength--;
        pinRaw[pinLength] = '\0';
        updatePinDisplay();
    }
}

void CalibrationView::validatePin()
{
	if (pinLength == MAX_PIN_DIGITS)
	{
		presenter->validatePin(pinRaw);
	}
	else
	{
		showPinError();
	}
}

void CalibrationView::clearPin()
{
    pinLength = 0;
    pinRaw[0] = '\0';
    updatePinDisplay();
}

void CalibrationView::showPinValidationResult(bool success)
{
    if (success)
    {
        clearPin();
        si_locked.setBitmap(touchgfx::Bitmap(BITMAP_LOCK_OPEN_100_ID));
        si_locked.invalidate();
        hidePinpad();
    }
    else
    {
        showPinError();
    }
}


void CalibrationView::showPinError()
{
    if (!blinking)
    {
        blinking = true;
        blinkCount = 0;
        originalColor = ta_pin.getColor();
    }
}
void CalibrationView::pinPadClicked_0() { appendDigit(0); }
void CalibrationView::pinPadClicked_1() { appendDigit(1); }
void CalibrationView::pinPadClicked_2() { appendDigit(2); }
void CalibrationView::pinPadClicked_3() { appendDigit(3); }
void CalibrationView::pinPadClicked_4() { appendDigit(4); }
void CalibrationView::pinPadClicked_5() { appendDigit(5); }
void CalibrationView::pinPadClicked_6() { appendDigit(6); }
void CalibrationView::pinPadClicked_7() { appendDigit(7); }
void CalibrationView::pinPadClicked_8() { appendDigit(8); }
void CalibrationView::pinPadClicked_9() { appendDigit(9); }
void CalibrationView::pinPadClicked_del() { deleteLastDigit(); }
void CalibrationView::pinPadClicked_ok() { validatePin(); }


void CalibrationView::calPressClicked()
{
	printf("CalibrationView::calPressClicked()....\n");
	if(presenter->configOptionsAllowed()){
		presenter->setGuiTherapy(CAL_PRESS_MODE);
		application().gotoSelectorScreenNoTransition();
	}
	else
	{
		showPinpad();
	}
}

void CalibrationView::calFlowClicked()
{
	printf("CalibrationView::calFlowClicked()....\n");
	if(presenter->configOptionsAllowed())
	{
		presenter->setGuiTherapy(CAL_FLOW_MODE);
		presenter->flowCalibrationStart();
		application().gotoSelectorScreenNoTransition();
	}
	else
	{
		showPinpad();
	}
}

void CalibrationView::calO3Clicked()
{
	printf("CalibrationView::calO3Clicked()....\n");
	if(presenter->configOptionsAllowed())
	{
		presenter->setGuiTherapy(CAL_O3_MODE);
		presenter->o3CalibrationStart();
		application().gotoSelectorScreenNoTransition();
	}
	else
	{
		showPinpad();
	}
}

void CalibrationView::saveParamClicked()
{
	printf("CalibrationView::saveParamClicked()....\n");
	if(presenter->configOptionsAllowed())
	{
		presenter->setGuiTherapy(SAVE_PARAMS_MODE);
		application().gotoProcessingScreenNoTransition();
	}
	else
	{
		showPinpad();
	}
}
void CalibrationView::generationTypeClicked()
{
	printf("CalibrationView::generationTypeClicked()....\n");
	if(presenter->configOptionsAllowed())
	{
		presenter->setGuiTherapy(GENERATION_TYPE_MODE);
		application().gotoSwitchSelectScreenNoTransition();
	}
	else
	{
		showPinpad();
	}
}

void CalibrationView::swVersionClicked()
{
	printf("CalibrationView::swVersionClicked()....\n");
	presenter->setGuiTherapy(SW_VERSION_MODE);
	application().gotoInformationScreenNoTransition();
}

void CalibrationView::pressTempClicked()
{
	printf("CalibrationView::pressTempClicked()....\n");
	presenter->setGuiTherapy(PRESSURE_TEMP_MODE);
	application().gotoInformationScreenNoTransition();
}

void CalibrationView::showParamClicked()
{
	printf("CalibrationView::showParamClicked()....\n");
	presenter->setGuiTherapy(SHOW_PARAMS_MODE);
	application().gotoInformationScreenNoTransition();
}

void CalibrationView::showStartClicked()
{
	printf("CalibrationView::showStartClicked()....\n");
	presenter->setGuiTherapy(SHOW_STARTUP_MODE);
	application().gotoInformationScreenNoTransition();
}

void CalibrationView::loadParamClicked()
{
	printf("CalibrationView::loadParamClicked()....\n");
	presenter->setGuiTherapy(LOAD_PARAMS_MODE);
	application().gotoProcessingScreenNoTransition();
}

void CalibrationView::calPeriodClicked()
{
	printf("CalibrationView::calPeriodClicked()....\n");
	presenter->setGuiTherapy(CAL_PERIOD_MODE);
	application().gotoProcessingScreenNoTransition();
}

void CalibrationView::cancelClicked()
{
	//presenter->cancelClicked();
	if(bx_pinPadBack.isVisible())
	{
		hidePinpad();
	}
	else
	{
		HAL_NVIC_SystemReset();
	}
}

