#include <gui/syringefill_screen/SyringeFillView.hpp>
#include <BitmapDatabase.hpp>

#define ANIMATION_DELAY 50
#define ANIMATION_STEPS 125

int16_t delay;
int16_t syringeContent_yIni;
int16_t syringePlunger_yIni;

SyringeFillView::SyringeFillView()
{

}

void SyringeFillView::setupScreen()
{
    SyringeFillViewBase::setupScreen();
/*
    syringeContent_yIni = im_syringeContent.getY();
    syringePlunger_yIni = im_syringePlunger.getY();
    im_depressurize.setVisible(false);
    im_depressurize.setAlpha(255);

	startAnimation();

	printf("SyringeFillView setup...\n");
	*/
}

void SyringeFillView::cnlClicked()
{
	presenter->userCancelled(); // not used because handleClickEvent intercepts the event (needed to implement the release event)
}

void SyringeFillView::handleClickEvent(const ClickEvent& evt)
{
    if (evt.getType() == ClickEvent::PRESSED)
    {
        // Check if the event comes from bt_OK
        if (bt_OK.getRect().intersect(evt.getX(), evt.getY()))
        {
        	if(bt_OK.isVisible())
        	{
				printf("User Ok (handle click viewer)\n");
				presenter->userOk();
        	}
        }
        else if (bt_cancel.getRect().intersect(evt.getX(), evt.getY()))
        {
        	if(bt_cancel.isVisible())
        	{
				printf("User cancelled (handle click viewer)\n");
				presenter->userCancelled();
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
				printf("User Ok release (handle click viewer)\n");
				presenter->userOkRelease();
        	}
        }
    }
}

void SyringeFillView::test_quitar()
{
	presenter->userCancelled();
}

void SyringeFillView::tearDownScreen()
{
    SyringeFillViewBase::tearDownScreen();

    im_syringeContent.setY(syringeContent_yIni);
    im_syringePlunger.setY(syringePlunger_yIni);
}

int16_t SyringeFillView::computeAnimationSteps()
{
//	syringePlunger_yIni  ------------> ANIMATION_STEPS
//	im_syringePlunger.getY() --------> X

	return( (im_syringePlunger.getY() * ANIMATION_STEPS) / syringePlunger_yIni );
}


void SyringeFillView::handleTickEvent()
{
#ifndef SIMULATOR
	// manage OK button icon
	if(bt_OK.isVisible())  //TODO: evaluate if its necessary to update the button icon
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
	}
	// manage pause
	if(presenter->isPaused())
	{
		if(im_syringePlunger.isMoveAnimationRunning() && im_syringeContent.isMoveAnimationRunning() )//&& !im_depressurize.isVisible())
		{
			im_syringePlunger.cancelMoveAnimation();
			im_syringeContent.cancelMoveAnimation();
		}
	}
	else
	{
		if(!im_syringePlunger.isMoveAnimationRunning() && !im_syringeContent.isMoveAnimationRunning() )//&& !im_depressurize.isVisible())
		{
			if( im_syringeContent.getY() <= (syringeContent_yIni - im_syringeContent.getHeight()) )
			{
				if(!(--delay))
				{
					// reset position
					delay = ANIMATION_DELAY;
					im_syringePlunger.startMoveAnimation(im_syringePlunger.getX(), syringePlunger_yIni, 0, EasingEquations::linearEaseNone, EasingEquations::linearEaseNone);
					im_syringeContent.startMoveAnimation(im_syringeContent.getX(), syringeContent_yIni, 0, EasingEquations::linearEaseNone, EasingEquations::linearEaseNone);
				}
			}else
			{
				im_syringePlunger.startMoveAnimation(im_syringePlunger.getX(), syringePlunger_yIni - im_syringeContent.getHeight(), computeAnimationSteps(), EasingEquations::linearEaseNone, EasingEquations::linearEaseNone);
				im_syringeContent.startMoveAnimation(im_syringeContent.getX(), syringeContent_yIni - im_syringeContent.getHeight(), computeAnimationSteps(), EasingEquations::linearEaseNone, EasingEquations::linearEaseNone);
			}
		}
	}
	// manage depressurize icon
	if( presenter->isCompensatingPressure() )
	{
		if(!im_depressurize.getAlpha())
		{
			im_depressurize.setAlpha(255);
			im_depressurize.startFadeAnimation(0, 50, touchgfx::EasingEquations::linearEaseOut);
		}
	}

#endif
}

void SyringeFillView::startAnimation()
{
    delay = ANIMATION_DELAY;
	im_syringePlunger.startMoveAnimation(im_syringePlunger.getX(), syringePlunger_yIni - im_syringeContent.getHeight(), ANIMATION_STEPS, EasingEquations::linearEaseNone, EasingEquations::linearEaseNone);
	im_syringeContent.startMoveAnimation(im_syringeContent.getX(), syringeContent_yIni - im_syringeContent.getHeight(), ANIMATION_STEPS, EasingEquations::linearEaseNone, EasingEquations::linearEaseNone);
}


void SyringeFillView::initTherapyContext(THERAPY_CTX *ctx)
{
	therapyCtx = ctx;
}

uint16_t SyringeFillView::getTherapyTargetValue(uint8_t valueID)
{
	return presenter->getTherapyTargetValue(valueID);
}

void SyringeFillView::initTherapyDataInfo()
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

void SyringeFillView::initVisualElemnets()
{
	delay = ANIMATION_DELAY;
    syringeContent_yIni = im_syringeContent.getY();
    syringePlunger_yIni = im_syringePlunger.getY();
    im_depressurize.cancelFadeAnimation();
    im_depressurize.setVisible(true);
    im_depressurize.setAlpha(0);
	bt_OK.setVisible(therapyCtx->okButtonVisible);
}
