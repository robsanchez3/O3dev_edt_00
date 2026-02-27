#include <gui/error_screen/ErrorView.hpp>

ErrorView::ErrorView()
{

}

void ErrorView::setupScreen()
{
    ErrorViewBase::setupScreen();
}

void ErrorView::tearDownScreen()
{
    ErrorViewBase::tearDownScreen();
}

void ErrorView::initTherapyContext(THERAPY_CTX *ctx)
{
	therapyCtx = ctx;
}

uint16_t ErrorView::getTherapyTargetValue(uint8_t valueID)
{
	return presenter->getTherapyTargetValue(valueID);
}

void ErrorView::initTherapyDataInfo()
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

void ErrorView::initErrorCode()
{
	Unicode::UnicodeChar text[10];

	Unicode::itoa(presenter->getErrorState(), text, sizeof(text), 10);
	Unicode::strncpy(ta_errorCodeBuffer, text, Unicode::strlen(text));

    ta_errorCode.invalidate();
}

void ErrorView::cnlClicked()
{
//	printf("Error cancelled from ErrorView::cnlClicked...\n");
	presenter->exitError();
	presenter->isStarting() ? application().gotoStartingScreenNoTransition() : application().gotoMainMenuScreenNoTransition();
}
