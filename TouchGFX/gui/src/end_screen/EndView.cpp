#include <gui/end_screen/EndView.hpp>
#include <BitmapDatabase.hpp>

EndView::EndView()
{

}

void EndView::setupScreen()
{
    EndViewBase::setupScreen();
    switch (presenter->getFsmState())
    {
    case STATE_COMPLETED:
    	si_result.setBitmap(Bitmap(BITMAP_CHECK_CIRCLE_GREEN_200_ID));
    	break;
    case STATE_USER_CANCELLED:
    	si_result.setBitmap(Bitmap(BITMAP_CANCEL_CIRCLE_RED_200_ID));
    	break;
    case STATE_OVERPRESSURE:
    	si_result.setBitmap(Bitmap(BITMAP_OVERPRESSURE_CIRCLE_YELLOW_200_ID));
    	break;
    }

    two_items_top_text_y = (((ta_time.getY() - ta_dose.getY()) + ta_dose.getHeight()) / 3) - (ta_dose.getHeight() / 2) + ta_dose.getY();
    two_items_down_text_y = two_items_top_text_y + ta_volume.getY() - ta_dose.getY();
}

void EndView::tearDownScreen()
{
    EndViewBase::tearDownScreen();
}

void EndView::initTherapyContext(THERAPY_CTX *ctx)
{
	therapyCtx = ctx;
}

uint16_t EndView::getTherapyTargetValue(uint8_t valueID)
{
	return presenter->getTherapyTargetValue(valueID);
}

void EndView::initVisibleItems()
{
    uint8_t visibleItems = 0;

    // calculate visible items number depending on therapy context
    if(therapyCtx->endTotalDoseVisible)	  { visibleItems++; }
    if(therapyCtx->endTotalVolumeVisible) { visibleItems++; }
    if(therapyCtx->endTotalTimeVisible)   { visibleItems++; }

    // position text lines depending on visible items
    if(visibleItems == 1)
	{
		if(therapyCtx->endTotalDoseVisible)
		{
			ta_dose.setY(ta_volume.getY());
		}
		else if(therapyCtx->endTotalTimeVisible)
		{
			ta_time.setY(ta_volume.getY());
		}
	}
    else if(visibleItems == 2)
	{
        if(therapyCtx->endTotalDoseVisible)
        {
        	ta_dose.setY(two_items_top_text_y);

        	if(therapyCtx->endTotalVolumeVisible)
        	{
        		ta_volume.setY(two_items_down_text_y);
        	}
        	else if(therapyCtx->endTotalTimeVisible)
			{
				ta_time.setY(two_items_down_text_y);
			}
        }
        else
        {
    		ta_volume.setY(two_items_top_text_y);
    		ta_time.setY(two_items_down_text_y);
        }
	}
    // position icons depending on visible items (centered with text)
    if(visibleItems != 3)
    {
    	si_dose.setY(ta_dose.getY() + ta_dose.getHeight() / 2 - si_dose.getHeight() / 2);
    	si_volume.setY(ta_volume.getY() + ta_volume.getHeight() / 2 - si_volume.getHeight() / 2);
    	si_time.setY(ta_time.getY() + ta_time.getHeight() / 2 - si_time.getHeight() / 2);
    }
    // show items depending on therapy context
    si_dose.setVisible(therapyCtx->endTotalDoseVisible);
    si_volume.setVisible(therapyCtx->endTotalVolumeVisible);
    si_time.setVisible(therapyCtx->endTotalTimeVisible);

    ta_dose.setVisible(therapyCtx->endTotalDoseVisible);
    ta_volume.setVisible(therapyCtx->endTotalVolumeVisible);
    ta_time.setVisible(therapyCtx->endTotalTimeVisible);

}

void EndView::initTherapyDataInfo()
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

void EndView::initTherapyEndInfo()
{
	uint32_t elpasedSeconds;
	uint32_t elpasedMinutes;

	Unicode::snprintf(ta_doseBuffer, sizeof(ta_dose), "%u ug", presenter->getCurrentTotalDose());
	ta_dose.invalidate();

	Unicode::snprintf(ta_volumeBuffer, sizeof(ta_volume), "%u ml", presenter->getCurrentOutputVolume());
	ta_volume.invalidate();

	elpasedSeconds = presenter->getConfiguredTime() - (presenter->getRemainingMinutes() * 60) - presenter->getRemainingSeconds();
	elpasedMinutes = elpasedSeconds / 60;
	elpasedSeconds = elpasedSeconds - (elpasedMinutes * 60);

	Unicode::snprintf(ta_timeBuffer, sizeof(ta_time), "%02d:%02d", elpasedMinutes, elpasedSeconds);
	ta_time.invalidate();
}

void EndView::okClicked()
{
	presenter->StartGeneration();
}

void EndView::cnlClicked()
{
	presenter->gotoRepose();
//	application().gotoMainMenuScreenNoTransition();
}
