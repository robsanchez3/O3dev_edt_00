#include <gui/selector_screen/SelectorView.hpp>
#include <gui/mainmenu_screen/MainMenuView.hpp>

#include <stdio.h>

#define TOUT_INI_DELAY_MS 10000

SelectorView::SelectorView()
{

}

#include <stdarg.h>
#include <touchgfx/hal/Types.hpp>

#include <string.h>

void SelectorView::setupScreen()
{
    SelectorViewBase::setupScreen();
    selectionStep = 0;
    selectorViewStarted = false;

	accelCounter = 5;
	bt_back.setInterval(30);
	bt_forward.setInterval(30);
	tickCounter = 0;
}

void SelectorView::tearDownScreen()
{
    SelectorViewBase::tearDownScreen();
}

void SelectorView::handleTickEvent()
{
	uint8_t percent;
	int32_t elapsedMs;

	if(blinkUnits.replay)
	{
		if(++blinkUnits.count > UNITS_BLINK_DELAY)
		{
//			printf("Blinking units @ %d milliseconds\n", (int16_t)(UNITS_BLINK_DELAY * 16.66));
			blinkUnits.state = !blinkUnits.state;
		    ta_units.setVisible(blinkUnits.state);
		    ta_units.invalidate();
//		    si_magnitudeIcon.setVisible(blinkUnits.state);
//		    si_magnitudeIcon.invalidate();

			if(blinkUnits.state)
			{
				blinkUnits.replay--;
			}

			blinkUnits.count = 0;
		}
	}

	//  Progress bar management
	if( lp_iniDelay.isVisible() || cp_iniDelay.isVisible() )
	{
		tickCounter++;
		// 16.66 ms per tic
		elapsedMs = tickCounter * 16;

		percent = (elapsedMs * 100) / therapyCtx->delayIndicatorTime[selectionStep]; // Calculate percentage of x seconds timeout

		lp_iniDelay.setValue(percent);
		cp_iniDelay.setValue(percent);
		if (elapsedMs >= therapyCtx->delayIndicatorTime[selectionStep])
		{
			tickCounter = 0;
			lp_iniDelay.setVisible(false);
			lp_iniDelay.invalidate();
			cp_iniDelay.setVisible(false);
			cp_iniDelay.invalidate();
//			therapyCtx->selectAvailable[0] = true;
//			therapyCtx->delayIndicatorVisible = false;
			initSelectionContext(++selectionStep);
		}
	}
}

void SelectorView::handleClickEvent(const ClickEvent& event)
{
	if(event.getType() == ClickEvent::RELEASED)
	{
		 // reset acceleration
		accelCounter = 5;
		bt_back.setInterval(30);
		bt_forward.setInterval(30);
	}

	SelectorViewBase::handleClickEvent(event); //
}

void SelectorView::thousandsSep(Unicode::UnicodeChar * dataBuffer)
{
	Unicode::UnicodeChar tmpBuffer[20];
    int len = Unicode::strlen(dataBuffer);
    int numSep = (len - 1) / 3;
    int newLen = len + numSep;
    int indexOrig = len - 1;
    int indexNew = newLen - 1;
    int count = 0;

    Unicode::strncpy(tmpBuffer, (const Unicode::UnicodeChar*)dataBuffer, 20);

    *(dataBuffer + newLen) = '\0'; // end of string

    while (indexOrig >= 0) {
        if (count == 3) {
        	dataBuffer[indexNew--] = '.';
            count = 0;
        } else {
        	dataBuffer[indexNew--] = tmpBuffer[indexOrig--];
            count++;
        }
    }
}

void SelectorView::sw_selectorUpdateItem(sw_selectContainer& item, int16_t itemIndex)  // TODO not used, consider removing
{
	uint32_t seconds;
	uint32_t minutes;
	Unicode::UnicodeChar buff[20];

	if(therapyCtx != NULL)
	{
		itemIndex = (itemIndex * therapyCtx->step[selectionStep]) + therapyCtx->minValue[selectionStep];

		Unicode::itoa(itemIndex, buff, 20, 10);
		thousandsSep(buff);

		item.setTextWidth(selectorItemWidth);
		item.setItem(buff);

		if(ta_second_selection.isVisible() && selectorViewStarted)
		{
			// update selection
			presenter->setTherapyTargetValue(therapyCtx->therapyTargetValue[selectionStep], sw_selector.getSelectedItem());

			// update back second selection value (and center it)
			seconds = presenter->getConfiguredTime();
			minutes = seconds / 60;
			seconds = seconds - (minutes * 60);

			Unicode::snprintf(ta_second_selectionBuffer, sizeof(ta_second_selection), "%02d:%02d", minutes, seconds);
			ta_second_selection.resizeToCurrentText();
			ta_second_selection.setX((getScreenWidth() - ta_second_selection.getTextWidth()) / 2);
			ta_second_selection.invalidate();
		}
	}
}

void SelectorView::sw_selectorUpdateCenterItem(sw_selectionContainer& item, int16_t itemIndex) // TODO not used, consider removing
{
	Unicode::UnicodeChar buff[20];

	if(therapyCtx != NULL)
	{
//		printf("SelectorView::sw_selectorUpdateCenterItem....Selected Item: %d\n", sw_selector.getSelectedItem()); // SWV debug

	//  itemIndex = (itemIndex * therapyCtx->step[selectionStep]) + therapyCtx->minValue[selectionStep];
	//	Unicode::snprintf(ta_O3Buffer, sizeof(ta_O3), "%2d", itemIndex);
	//	Unicode::itoa((sw_selector.getSelectedItem() * therapyCtx->step[selectionStep]) + therapyCtx->minValue[selectionStep], ta_selectionBuffer, sizeof(ta_selection), 10);
		Unicode::itoa((sw_selector.getSelectedItem() * therapyCtx->step[selectionStep]) + therapyCtx->minValue[selectionStep], buff, 20, 10);
	    thousandsSep(buff);
		Unicode::snprintf(ta_selectionBuffer, sizeof(ta_selection), "%s", buff);

		ta_selection.invalidate();
	}

    item.setTextWidth(selectorItemWidth);
    item.setBackWidth(selectorItemWidth);
    item.setItem(buff);
}

void SelectorView::okClicked()
{
// printf("SelectorView::okClicked... tag value: %d\n", bufferToInt(ta_selectionBuffer));

// presenter->okClicked(therapyCtx->therapyTargetValue[selectionStep], sw_selector.getSelectedItem());
   presenter->okClicked(therapyCtx->therapyTargetValue[selectionStep], bufferToInt(ta_selectionBuffer));
   presenter->onSelectionAction(selectionStep);
   initSelectionContext(++selectionStep);
}

void SelectorView::cancelClicked()
{
	presenter->cancelClicked();
	presenter->userCancelled();
}

int SelectorView::get_logical_value(int value)
{
	//  value --------------> maxPixelResolution
	//	logicalValue -------> therapyCtx->maxValue[step]
	int logicalValue = 0;
	int maxPixelResolution = sl_slider.getIndicatorMax();
	int maxValue = therapyCtx->maxValue[selectionStep] / therapyCtx->step[selectionStep];
	int minValue = therapyCtx->minValue[selectionStep] / therapyCtx->step[selectionStep];

//  logicalValue = ((value * (maxValue - minValue)) / maxPixelResolution) + minValue;
	logicalValue = ((value * (maxValue - minValue) + (maxPixelResolution / 2)) / maxPixelResolution) + minValue; // rounding

	return logicalValue;
}

int SelectorView::get_physical_value(int value)
{
	// value ---------------> therapyCtx->maxValue[step]
	// physicalValue -------> maxPixelResolution
	int physicalValue = 0;
	int maxPixelResolution = sl_slider.getIndicatorMax();
	int maxValue = therapyCtx->maxValue[selectionStep] / therapyCtx->step[selectionStep];
	int minValue = therapyCtx->minValue[selectionStep] / therapyCtx->step[selectionStep];

//	physicalValue = ((value - minValue) * maxPixelResolution) / (maxValue - minValue);
	physicalValue = (((value - minValue) * maxPixelResolution) + ((maxValue - minValue) / 2)) / (maxValue - minValue); // rounding

	return physicalValue;
}

void SelectorView::update_selection(int value)
{
	Unicode::UnicodeChar buff[20];
	Unicode::snprintf(buff, sizeof(sl_slider), "%d", value);
	thousandsSep(buff);
	Unicode::snprintf(ta_selectionBuffer, sizeof(ta_selection), therapyCtx->negativeValue[selectionStep] ? "-%s" : "%s", buff);
	ta_selection.invalidate();
	update_second_selection();
}

void SelectorView::update_second_selection()
{
	if(ta_second_selection.isVisible() && selectorViewStarted)
	{
		uint32_t seconds;
		uint32_t minutes;
		Unicode::UnicodeChar mlBuffer[4];
		Unicode::strncpy(mlBuffer, "ml", 3);

		if (Unicode::strncmp(ta_unitsBuffer, mlBuffer, 3) == 0)
		{
			// convert to minutes and seconds
			seconds = presenter->getConfiguredTime();
			minutes = seconds / 60;
			seconds = seconds - (minutes * 60);

			Unicode::snprintf(ta_second_selectionBuffer, sizeof(ta_second_selection), "%02d:%02d", minutes, seconds);
		}

		Unicode::strncpy(mlBuffer, "min", 4);
		if (Unicode::strncmp(ta_unitsBuffer, mlBuffer, 4) == 0) { // principal en ml }
			// convert to volume
			Unicode::snprintf(ta_second_selectionBuffer, sizeof(ta_second_selection), "%d ml", presenter->getConfiguredVolume());
		}

		ta_second_selection.invalidate();
	}
}

void SelectorView::sl_slider_changed(int value)
{
	int updateValue;

	physicalValue = value;
	logicalValue = get_logical_value(value);

	updateValue = logicalValue * therapyCtx->step[selectionStep];
	updateValue = (updateValue < therapyCtx->minValue[selectionStep]) ? therapyCtx->minValue[selectionStep] : updateValue;
	updateValue = (updateValue > therapyCtx->maxValue[selectionStep]) ? therapyCtx->maxValue[selectionStep] : updateValue;

	presenter->onSliderAction(selectionStep, therapyCtx->therapyTargetValue[selectionStep], updateValue);

	update_selection(updateValue);
}

void SelectorView::forwardClicked()
{
	int maxValue = therapyCtx->maxValue[selectionStep] / therapyCtx->step[selectionStep];
	int value = (logicalValue < maxValue) ? ++logicalValue : logicalValue;

	if(physicalValue != get_physical_value(value))
	{
		sl_slider.setValue(get_physical_value(value));
	}
	//  acceleration
	if(!(--accelCounter))
	{
		bt_forward.setInterval(5);
	}
}

void SelectorView::backClicked()
{
	int minValue = therapyCtx->minValue[selectionStep] / therapyCtx->step[selectionStep];
	int value = (logicalValue > minValue) ? --logicalValue : logicalValue;

	if(physicalValue != get_physical_value(value))
	{
		sl_slider.setValue(get_physical_value(value));
	}
	//  acceleration
	if(!(--accelCounter))
	{
		bt_back.setInterval(5);
	}
}

void SelectorView::initTherapyContext(THERAPY_CTX *ctx)
{
//	printf("SelectorView::initTherapyContext....\n");
	therapyCtx = ctx;
}

void SelectorView::initSelectionContext(int8_t step)
{
//	printf("%s: Step: %d\n", __func__, step);

	if(step == therapyCtx->stepsNum)
	{
//		application().gotoAdjustingScreenNoTransition();
//		presenter->StartGeneration();
		presenter->EndSelection();
	}
	else
	{
	//  update mode bitmap
	    si_modeIcon.setBitmap(touchgfx::Bitmap(therapyCtx->mainIcon));
	    si_modeIcon.invalidate();

    //  update magnitude bitmap
	    si_magnitudeIcon.setBitmap(touchgfx::Bitmap(therapyCtx->icon[step]));
	    si_magnitudeIcon.invalidate();

	//  update units
	    Unicode::strncpy(ta_unitsBuffer, therapyCtx->units[step],  Unicode::strlen(therapyCtx->units[step]));
	    *(ta_unitsBuffer +  Unicode::strlen(therapyCtx->units[step])) = 0;
	    ta_units.invalidate();

 #if 0
	//  sw_selector initialization according therapy data
	    selectorItemWidth = getSelectorItemWidth();
	    sw_selector.setDrawableSize(selectorItemWidth, 0);

	//    sw_selector.setSelectedItemExtraSize(100, 100);
	    //sw_selector.setSelectedItemMargin(100, 100);


	//  update selection range
	    sw_selector.setNumberOfItems( 1 + ((therapyCtx->maxValue[step] - therapyCtx->minValue[step]) / therapyCtx->step[step]) );

	//  update default selection value
	    sw_selector.animateToItem((therapyCtx->defValue[step] - therapyCtx->minValue[step]) / therapyCtx->step[step]);//, animationSteps);

	//  update selection offset
		sw_selector.setSelectedItemOffset((800 - selectorItemWidth)/2);
//		sw_selector.setSelectedItemOffset(400);

	//  end of initialization
	    sw_selector.initialize();
#endif

    //  update slider
	    logicalValue = therapyCtx->defValue[step] / therapyCtx->step[step];
	    physicalValue = get_physical_value(therapyCtx->defValue[step] / therapyCtx->step[step]);
	    sl_slider.setValueRange(0, sl_slider.getIndicatorMax(), physicalValue);
//	    sl_slider.setValueRange(0, sl_slider.getIndicatorMax(), get_physical_value(therapyCtx->defValue[step] / therapyCtx->step[step]));
	    sl_slider.setVisible(therapyCtx->selectAvailable[step]);
	    sl_slider.invalidate();

	//  update second selection visibility
	    ta_selection.setVisible(therapyCtx->selectAvailable[step]);
	    ta_selection.invalidate();

    //  update second selection visibility
		ta_second_selection.setVisible(therapyCtx->secondSelectionVisible[step]);
		update_second_selection();
		ta_second_selection.invalidate();

	//  update forward/back button visibility
		bt_back.setVisible(therapyCtx->selectAvailable[step]);
		bt_back.invalidate();
		bt_forward.setVisible(therapyCtx->selectAvailable[step]);
		bt_forward.invalidate();

    //  update OK button visibility
	//	bt_OK.setVisible(therapyCtx->selectAvailable[step]);
	//	bt_OK.invalidate();


	//  reset blink effect counter
	    blinkUnits.count = 0;
	    blinkUnits.replay = UNITS_BLINK_REPALY_MAX;
	    blinkUnits.state = true;

	//  update initial delay visibility
	    lp_iniDelay.setVisible(false);
//	    lp_iniDelay.setVisible( therapyCtx->delayIndicatorTime[step] ? true : false );
	    lp_iniDelay.invalidate();

	    cp_iniDelay.setVisible( therapyCtx->delayIndicatorTime[step] ? true : false );
	    cp_iniDelay.invalidate();
	    cp_iniDelay.setRange(0, 100);
	    cp_iniDelay.setValue(0);

    //  others
	    selectorViewStarted = true;
	    tickCounter = 0;
	}
}

int16_t SelectorView::getSelectorItemWidth()
{
	uint8_t width;

	if(therapyCtx->maxValue[selectionStep] <= 99){
		width = TWO_DIGITS_WIDTH;
	}else if(therapyCtx->maxValue[selectionStep] <= 999){
		width = THREE_DIGITS_WIDTH;
	}else if(therapyCtx->maxValue[selectionStep] <= 9999){
		width = FOUR_DIGITS_WIDTH;
	}else if(therapyCtx->maxValue[selectionStep] <= 99999){
		width = FIVE_DIGITS_WIDTH;
	}

	printf("Selected width: %d\n", width);

	return width;
}

int SelectorView::bufferToInt(const Unicode::UnicodeChar* buffer)
{
    unsigned int j = 0;
    Unicode::UnicodeChar asciiBuffer[20];

    // Convert Unicode to ASCII, ignoring non-numeric characters
    for (int i = 0; (buffer[i] != 0) && (j < sizeof(asciiBuffer) - 1); ++i)
    {
        if (buffer[i] >= '0' && buffer[i] <= '9')
        {
            asciiBuffer[j++] = static_cast<char>(buffer[i]);
        }
    }
    asciiBuffer[j] = '\0';

    return Unicode::atoi(asciiBuffer);
}

