#include <gui/adjusting_screen/AdjustingView.hpp>

#include "BitmapDatabase.hpp"

AdjustingView::AdjustingView():
	xAngle3D(0.0f),
	yAngle3D(0.0f),
	zAngle3D(0.0f),
	deltaXangle3D(0.032f),
	deltaYangle3D(0.029f),
	deltaZangle3D(0.032f),
	step(0),
	delay(60),
	stop(false)
{

}

void AdjustingView::setupScreen()
{
    AdjustingViewBase::setupScreen();
}

void AdjustingView::tearDownScreen()
{
    AdjustingViewBase::tearDownScreen();
}

void AdjustingView::handleTickEvent()
{
#ifndef SIMULATOR
//	Animation management
	if(!delay) // 16.66 ms per delay unit
	{
		step++;
		if(!stop)
		{
			xAngle3D += deltaXangle3D;
			yAngle3D += deltaYangle3D;
			zAngle3D += deltaZangle3D;
			tm_molecule.updateAngles(0.0F, yAngle3D, 0.0F);
		}

		switch (step)
		{
		case 53:  // quarter of turn
			tm_molecule.setBitmap(Bitmap(BITMAP_MOLECULAO3_2_ID));
			break;
		case 212: // whole turn
			delay = 100;
			stop = true;
			xAngle3D = 0;
			yAngle3D = 0;
			zAngle3D = 0;
			tm_molecule.updateAngles(0.0F, yAngle3D, 0.0F);
			break;
		case 213:
			delay = 10;
			tm_molecule.clearFadeAnimationEndedAction();
			tm_molecule.startFadeAnimation(0, delay, touchgfx::EasingEquations::linearEaseOut);
			si_molecule.clearFadeAnimationEndedAction();
			si_molecule.startFadeAnimation(255, delay, touchgfx::EasingEquations::linearEaseOut);
			break;
		case 214:
			delay = 60;
			step = 0;
			stop = false;
			tm_molecule.setBitmap(Bitmap(BITMAP_MOLECULAO2_ID));
			tm_molecule.setAlpha(255);
			si_molecule.setAlpha(0);
			break;
		}
	}
	else
	{
		delay--;
	}

#endif
}

void AdjustingView::okClicked()
{
}

void AdjustingView::cnlClicked()
{
	presenter->StopGeneration();
}

void AdjustingView::initTherapyContext(THERAPY_CTX *ctx)
{
	therapyCtx = ctx;
}

uint16_t AdjustingView::getTherapyTargetValue(uint8_t valueID)
{
	return presenter->getTherapyTargetValue(valueID);
}

void AdjustingView::initTherapyDataInfo()
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
