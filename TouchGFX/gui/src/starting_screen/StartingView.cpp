#include <gui/starting_screen/StartingView.hpp>
#include <gui_generated/starting_screen/StartingViewBase.hpp>

#include "BitmapDatabase.hpp"

#define TOUT_SETTINGS_MS 2500
#define VIEW_SETTINGS_DELAY_MS 1000

StartingView::StartingView():
	xAngle3D(0.0f),
	yAngle3D(0.0f),
	zAngle3D(0.0f),
	deltaXangle3D(0.032f),
	deltaYangle3D(0.029f),
	deltaZangle3D(0.032f),
	step(0),
	delay(60),
	stop(false),
    timeoutCounter(0),
    timeoutActive(true)
{

}

void StartingView::setupScreen()
{
    StartingViewBase::setupScreen();
    lp_tout.setVisible(false);
    bt_settings.setVisible(false);
    timeoutCounter = 0;
}

void StartingView::tearDownScreen()
{
    StartingViewBase::tearDownScreen();
}

void StartingView::handleTickEvent()
{
	uint8_t percent;
	uint32_t elapsedMs;

//  Progress bar management
	if (timeoutActive)
	{
		timeoutCounter++;
		// 16.66 ms per tic
		elapsedMs = timeoutCounter * 16;

		if(bt_settings.isVisible() == false)
		{
			if (elapsedMs >= VIEW_SETTINGS_DELAY_MS)
			{
			//	printf("StartingView::handleTickEvent() - Showing settings button after %ld ms\n", elapsedMs);
				timeoutCounter = 0;
				lp_tout.setVisible(true);
				bt_settings.setVisible(true);
				lp_tout.invalidate();
				bt_settings.invalidate();
			}
		}
		else
		{
			percent = (elapsedMs * 100) / TOUT_SETTINGS_MS; // Calculate percentage of 3 seconds timeout

			lp_tout.setValue(percent);

			if (elapsedMs >= TOUT_SETTINGS_MS)
			{
			//	printf("StartingView::handleTickEvent() - Hiding settings button after %ld ms\n", elapsedMs);
				timeoutActive = false;
				timeoutCounter = 0;
				lp_tout.setVisible(false);
				bt_settings.setVisible(false);
				lp_tout.invalidate();
				bt_settings.invalidate();
			}
		}
	}

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
}

void StartingView::settingsClicked()
{
	printf("StartingView::settingsClicked()...\n");
	presenter->settingsClicked();
}
