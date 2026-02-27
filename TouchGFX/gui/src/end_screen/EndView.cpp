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
//	TODO: make position dinamic dependig on visible items (hide icons)
//  TODO: do the same for running view
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
