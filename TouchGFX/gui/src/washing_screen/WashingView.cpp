#include <gui/washing_screen/WashingView.hpp>

WashingView::WashingView():
	xAngle3D(0.0f),
	yAngle3D(0.0f),
	zAngle3D(0.0f),
	deltaXangle3D(0.032f),
	deltaYangle3D(0.029f),
	deltaZangle3D(0.032f),
	step(0),
	delay(30),
	stop(false)
{

}

void WashingView::setupScreen()
{
    WashingViewBase::setupScreen();
	Unicode::snprintf(ta_timeBuffer, sizeof(ta_time), "", presenter->getWashingSeconds());
	ta_time.invalidate();
}

void WashingView::tearDownScreen()
{
    WashingViewBase::tearDownScreen();
}


void WashingView::handleTickEvent()
{
#ifndef SIMULATOR

	if(presenter->refreshGenerationInfo())
	{
		Unicode::snprintf(ta_timeBuffer, sizeof(ta_time), "00:%02d", presenter->getWashingSeconds());
		ta_time.invalidate();

		presenter->clearGenerationInfoFlag();
	}
#endif

	//	Animation management

	if(!delay) // 16.66 ms per delay unit
	{
		step++;
		xAngle3D += deltaXangle3D;
		yAngle3D += deltaYangle3D;
		zAngle3D += deltaZangle3D;
		tm_washing.updateAngles(0.0F, 0.0F, zAngle3D);

		if (step == 197)
		{
			xAngle3D = 0;
			yAngle3D = 0;
			zAngle3D = 0;
		}
	}
	else
	{
		delay--;
	}
}

void WashingView::cnlClicked()
{
	presenter->cancelWashing();
}

void WashingView::okClicked()
{
	presenter->washingExtesion();
}

void WashingView::initTherapyContext(THERAPY_CTX *ctx)
{
	therapyCtx = ctx;
}

uint16_t WashingView::getTherapyTargetValue(uint8_t valueID)
{
	return presenter->getTherapyTargetValue(valueID);
}

void WashingView::initTherapyDataInfo()
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
