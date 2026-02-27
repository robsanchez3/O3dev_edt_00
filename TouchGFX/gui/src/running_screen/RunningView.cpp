#include <gui/running_screen/RunningView.hpp>
#include <BitmapDatabase.hpp>

RunningView::RunningView()
{

}

void RunningView::setupScreen()
{
    RunningViewBase::setupScreen();
	cp_progress.setRange(0, presenter->getConfiguredTime());
	cp_progress.setValue(presenter->getConfiguredTime() - 0);
}

void RunningView::tearDownScreen()
{
    RunningViewBase::tearDownScreen();
}

void RunningView::cnlClicked()  // not used because handleClickEvent intercepts the event (needed to implement the release event)
{
	presenter->userCancelled();
}

void RunningView::okClicked()   // not used because handleClickEvent intercepts the event (needed to implement the release event)
{
//	printf("User Ok (viewer)\n");
	presenter->userOk();
}

void RunningView::bigPauseClicked()  // not used because handleClickEvent intercepts the event (needed to implement the release event)
{
//	printf("Big Pause clicked...\n");
	presenter->userOk();
}

void RunningView::handleClickEvent(const ClickEvent& evt)
{
    if (evt.getType() == ClickEvent::PRESSED)
    {
        // Check if the event comes from bt_OK
        if (bt_OK.getRect().intersect(evt.getX(), evt.getY()))
        {
        	if(bt_OK.isVisible())
        	{
//				printf("User Ok (handle click viewer)\n");   // TODO this only has to be done when visible (check syringe screen)
				presenter->userOk();
        	}
        }
        else if (bt_cancel.getRect().intersect(evt.getX(), evt.getY()))
        {
        	if(bt_cancel.isVisible())
        	{
//        		printf("User cancelled (handle click viewer)\n");
        		presenter->userCancelled();
        	}
        }
        else if (bt_bigPauseView.getRect().intersect(evt.getX(), evt.getY()))
        {
        	if(bt_bigPauseView.isVisible())
        	{
//        		printf("User manage big pause button (handle click viewer)\n");
        		manageBigPauseView();
        	}
        }
        else if (bt_bigPause.getRect().intersect(evt.getX(), evt.getY()))
        {
        	if(bt_bigPause.isVisible())
        	{
            	printf("User Ok big button (handle click viewer)\n");
        		presenter->userOk();
        	}
        }
    }
    else if (evt.getType() == ClickEvent::RELEASED)
    {
        // Check if the event comes from bt_OK
        if (bt_OK.getRect().intersect(evt.getX(), evt.getY()))
        {
        	if(bt_OK.isVisible())
        	{
//				printf("User Ok release (handle click viewer)\n");
				presenter->userOkRelease();
        	}
        }
    }
}

void RunningView::handleTickEvent()
{
#ifndef SIMULATOR
	static uint32_t lastTotalDose = -1;
	static uint32_t lastOutputVolume = -1;
	static uint8_t  lastRemainingMinutes = -1;
	static uint8_t  lastRemainingSeconds = -1;
	static uint16_t lastOperatingPressure = -1;

	if(presenter->refreshGenerationInfo())
	{
//      printf("Updating current total dose value:         %u\n",  presenter->getCurrentTotalDose());
//		printf("Updating current output volume value:      %u\n",  presenter->getCurrentOutputVolume());
//		printf("Updating remaining minutes value:          %2d\n", presenter->getRemainingMinutes());
//		printf("Updating remaining seconds value:          %2d\n", presenter->getRemainingSeconds());
//		printf("Updating current operating pressure value: %d\n",  presenter->getCurrentOperatingPressure());
//		printf("Configured seconds value:                  %d\n",  presenter->getConfiguredTime());

		if(presenter->getCurrentTotalDose() != lastTotalDose)
		{
			lastTotalDose = presenter->getCurrentTotalDose();
			Unicode::snprintf(ta_doseBuffer, sizeof(ta_dose), "%u ug", lastTotalDose);
			ta_dose.invalidate();
		}
		if(presenter->getCurrentOutputVolume() != lastOutputVolume)
		{
			lastOutputVolume = presenter->getCurrentOutputVolume();
			Unicode::snprintf(ta_volumeBuffer, sizeof(ta_volume), "%u ml", lastOutputVolume);
			ta_volume.invalidate();
		}
		if( (presenter->getRemainingMinutes() != lastRemainingMinutes) || (presenter->getRemainingSeconds() != lastRemainingSeconds) )
		{
			lastRemainingMinutes = presenter->getRemainingMinutes();
			lastRemainingSeconds = presenter->getRemainingSeconds();
			Unicode::snprintf(ta_timeBuffer, sizeof(ta_time), "%02d:%02d", lastRemainingMinutes, lastRemainingSeconds);
			ta_time.invalidate();
		}
		if(presenter->getCurrentOperatingPressure() != lastOperatingPressure)
		{
			lastOperatingPressure = presenter->getCurrentOperatingPressure();
			Unicode::snprintf(ta_pressureBuffer, sizeof(ta_pressure), "%d mbar", lastOperatingPressure);
			ta_pressure.invalidate();
		}

		updatePressureBar();

		cp_progress.setValue(presenter->getConfiguredTime() - ((presenter->getRemainingMinutes() * 60) + presenter->getRemainingSeconds()));

		presenter->clearGenerationInfoFlag();
	}

	// update right side button
	if(bt_OK.isVisible())
	{
		if( presenter->isPauseAvailable())
		{
			if( !presenter->isPaused() && (bt_OK.getCurrentlyDisplayedBitmap() != Bitmap(BITMAP_PAUSEBUTTON_00_A_ID)) )
			{
				bt_OK.setBitmaps(Bitmap(BITMAP_PAUSEBUTTON_00_A_ID), Bitmap(BITMAP_PAUSEBUTTON_00_B_ID));
				bt_OK.invalidate();
			}

			if( presenter->isPaused() && (bt_OK.getCurrentlyDisplayedBitmap() != Bitmap(BITMAP_PLAYBUTTON_00_A_ID)) )
			{
				bt_OK.setBitmaps(Bitmap(BITMAP_PLAYBUTTON_00_A_ID), Bitmap(BITMAP_PLAYBUTTON_00_B_ID));
				bt_OK.invalidate();
			}
		}
		else
		{
			if( (bt_OK.getCurrentlyDisplayedBitmap() != Bitmap(BITMAP_PLAYBUTTON_00_A_ID)) )
			{
				bt_OK.setBitmaps(Bitmap(BITMAP_PLAYBUTTON_00_A_ID), Bitmap(BITMAP_PLAYBUTTON_00_B_ID));
				bt_OK.invalidate();
			}
		}
	}

	// manage big pause button visibility
	if(bt_bigPause.isVisible() && (bt_bigPause.getAlpha() == 0))
	{
//		printf("Big pause visible to false...\n");
		bt_bigPause.setVisible(false);
	}
#endif
}

void RunningView::initTherapyContext(THERAPY_CTX *ctx)
{
//	printf("RunningView, Init therapy context...\n");
	therapyCtx = ctx;
}

uint16_t RunningView::getTherapyTargetValue(uint8_t valueID)
{
	return presenter->getTherapyTargetValue(valueID);
}

void RunningView::initTherapyDataInfo()
{
	uint8_t i;
	uint16_t initWidth = 0;
	uint16_t offset = 0;
	Unicode::UnicodeChar text[20];

	initWidth = si_modeIcon.getWidth();

	//  update therapy data info
    Unicode::strncpy(ta_therapyDataBuffer, "", sizeof(ta_therapyDataBuffer));
    for(i = 0; i < therapyCtx->stepsNum; i++)
    {
    	Unicode::itoa(getTherapyTargetValue(i), text, sizeof(text), 10);
    	Unicode::strncpy(ta_therapyDataBuffer + offset, text, Unicode::strlen(text));
    	offset += Unicode::strlen(text);

      	Unicode::strncpy(ta_therapyDataBuffer + offset, therapyCtx->units[i], Unicode::strlen(therapyCtx->units[i]));
    	offset += Unicode::strlen(therapyCtx->units[i]);
        Unicode::strncpy(ta_therapyDataBuffer + offset, " ", 1);
    	offset ++;
    }
    Unicode::strncpy(ta_therapyDataBuffer + offset, "\0", 1);
    ta_therapyData.resizeToCurrentText();

    ta_therapyData.setX((getScreenWidth() + initWidth + (ICON_TEXT_SPACE / 2) - ta_therapyData.getTextWidth()) / 2);
    ta_therapyData.invalidate();

	//  update mode bitmap
	si_modeIcon.setBitmap(touchgfx::Bitmap(therapyCtx->mainIcon));
	si_modeIcon.setWidthHeight(initWidth, initWidth);
	si_modeIcon.setX(ta_therapyData.getX() - initWidth - ICON_TEXT_SPACE);
	si_modeIcon.invalidate();
}

void RunningView::updatePressureBar()
{
	uint16_t pMax = 1700;          // mbar
	uint16_t pMin = 600;           // mbar
	uint16_t pRange = pMax - pMin; // mbar
	uint16_t pRelative = presenter->getCurrentOperatingPressure() - pMin;  // mbar

	uint16_t xRange = si_pressure.getWidth();            // pixels
	float xRelative = (float) ((pRelative * xRange) / pRange);  // pixels

	xRelative -= si_pressPointer.getWidth() / 2;

	si_pressPointer.moveTo(si_pressure.getX() + (int16_t) xRelative, si_pressPointer.getY());
	si_pressPointer.invalidate();
}

void RunningView::InitRunningContext()
{
	bt_OK.setVisible(therapyCtx->okButtonVisible);
	bt_bigPauseView.setVisible(therapyCtx->bigPauseButtonVisible);
}

void RunningView::InitProgressInfo()
{
	Unicode::snprintf(ta_doseBuffer, sizeof(ta_dose), "%u ug", 0);
	ta_dose.invalidate();

	Unicode::snprintf(ta_volumeBuffer, sizeof(ta_volume), "%u ml", 0);
	ta_volume.invalidate();
}

void RunningView::InitProgressGraphic(int min, int max)
{
	cp_progress.setRange(min, max);
}

void RunningView::manageBigPauseView()
{
	if(bt_bigPause.isVisible())
	{
//		printf("Stating fade out...\n");
		bt_bigPause.startFadeAnimation(0, 10,  touchgfx::EasingEquations::linearEaseOut);
		bt_bigPauseView.setBitmaps(Bitmap(BITMAP_EXPAND_01_ID), Bitmap(BITMAP_EXPAND_01_ID));
		bt_bigPauseView.invalidate();
	}
	else
	{
//		printf("Stating fade in...\n");
//		printf("Big pause visible to true...\n");
		bt_bigPause.setVisible(true);
		bt_bigPause.startFadeAnimation(100, 10,  touchgfx::EasingEquations::linearEaseOut);
		bt_bigPauseView.setBitmaps(Bitmap(BITMAP_RETRACT_01_ID), Bitmap(BITMAP_RETRACT_01_ID));
		bt_bigPauseView.invalidate();
	}
}
