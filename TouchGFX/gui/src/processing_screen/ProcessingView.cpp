#include <gui/processing_screen/ProcessingView.hpp>

ProcessingView::ProcessingView()
{

}

void ProcessingView::setupScreen()
{
    ProcessingViewBase::setupScreen();
    setupProcessingScreen();
}

void ProcessingView::tearDownScreen()
{
    ProcessingViewBase::tearDownScreen();
}

void ProcessingView::cancelClicked()
{
	printf("ProcessingView::cancelClicked()....\n");
	presenter->userCancelled();
//	presenter->setGuiTherapy(TT_NO_MODE);
//	application().gotoCalibrationScreenNoTransition();
}

void ProcessingView::okClicked()
{
	presenter->userOk();
}

void ProcessingView::initTherapyContext(THERAPY_CTX *ctx)
{
//	printf("ProcessingView::initTherapyContext....\n");
	therapyCtx = ctx;
}

void ProcessingView::handleTickEvent()
{
	uint8_t percent;
	int32_t elapsedMs;

	if(presenter->getFsmState() != presenter->getCalibrationSucessState())
	{
		tickCount++;
		// 16.66 ms per tic
		elapsedMs = tickCount * 16;

		percent = (elapsedMs * 100) / therapyCtx->delayIndicatorTime[0]; // Calculate percentage of x seconds timeout

		cp_progress.setValue(percent);
		if (elapsedMs >= therapyCtx->delayIndicatorTime[0])
		{
//			tickCount = 0;
//			cp_progress.setVisible(false);
//			cp_progress.invalidate();
		}
	}
	else
	{
		setupProcessEnd();
	}
}

void ProcessingView::setupCalibratePeriod()
{
	char buff[50];

	sprintf((char *)buff, "Period calibration. Please wait...");
	memset(ta_infoBuffer, 0, sizeof(ta_infoBuffer));
	touchgfx::Unicode::fromUTF8((const uint8_t*)buff, ta_infoBuffer,Unicode::strlen(buff));
	ta_info.invalidate();
}

void ProcessingView::setupLoadParameters()
{
	char buff[50];

	sprintf((char *)buff, "Loading parameters. Please wait...");
	memset(ta_infoBuffer, 0, sizeof(ta_infoBuffer));
	touchgfx::Unicode::fromUTF8((const uint8_t*)buff, ta_infoBuffer,Unicode::strlen(buff));
	ta_info.invalidate();
}
void ProcessingView::setupSaveParameters()
{
	char buff[50];

	sprintf((char *)buff, "Saving parameters. Please wait...");
	memset(ta_infoBuffer, 0, sizeof(ta_infoBuffer));
	touchgfx::Unicode::fromUTF8((const uint8_t*)buff, ta_infoBuffer,Unicode::strlen(buff));
	ta_info.invalidate();
}

void ProcessingView::setupProcessingScreen()
{
	tickCount = 0;
	progressCount = 0;
	cp_progress.setRange(0, 100);
	cp_progress.setValue(0);
	bt_OK.setVisible(false);

	switch(presenter->getGuiTherapy())
	{
		case CAL_PERIOD_MODE:  setupCalibratePeriod();
							   presenter->periodCalibrationStart();
							   break;
		case LOAD_PARAMS_MODE: setupLoadParameters();
							   presenter->loadParameterStart();
			                    break;
		case SAVE_PARAMS_MODE: setupSaveParameters();
							   presenter->saveParameterStart();
			                   break;
		default:               memset(ta_infoBuffer, 0, sizeof(ta_infoBuffer));
			                   ta_info.invalidate();
			                   break;
	}
}

void ProcessingView::setupProcessEnd()
{
	char buff[50];

	bt_OK.setVisible(true);
	bt_OK.invalidate();
	bt_cancel.setVisible(false);
	bt_cancel.invalidate();

	switch(presenter->getGuiTherapy())
	{
		case CAL_PERIOD_MODE:  sprintf((char *)buff, "Period calibration. Finished.");
							   break;
		case LOAD_PARAMS_MODE: sprintf((char *)buff, "Loading parameters. Finished.");
			                   break;
		case SAVE_PARAMS_MODE: sprintf((char *)buff, "Saving parameters. Finished.");
			                   break;
		case CAL_O3_MODE:      sprintf((char *)buff, "Setting O3 calibration. Finished.");
                               break;
		default:               sprintf((char *)buff, "Unexpected error...");
			                   break;
	}
	memset(ta_infoBuffer, 0, sizeof(ta_infoBuffer));
	touchgfx::Unicode::fromUTF8((const uint8_t*)buff, ta_infoBuffer,Unicode::strlen(buff));
	ta_info.invalidate();

}
