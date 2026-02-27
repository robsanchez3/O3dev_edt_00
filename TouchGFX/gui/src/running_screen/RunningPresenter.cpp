#include <gui/running_screen/RunningView.hpp>
#include <gui/running_screen/RunningPresenter.hpp>

RunningPresenter::RunningPresenter(RunningView& v)
    : view(v)
{

}

void RunningPresenter::activate()
{
	view.initTherapyContext(model->getTherapyCtx());
	view.initTherapyDataInfo();
	view.InitRunningContext();
	view.InitProgressInfo();
}

void RunningPresenter::deactivate()
{

}

uint16_t RunningPresenter::getTherapyTargetValue(uint8_t valueID)
{
	return model->getTherapyTargetValue(valueID);
}

uint8_t RunningPresenter::getRemainingMinutes()
{
	return model->getRemainingMinutes();
}

uint8_t RunningPresenter::getRemainingSeconds()
{
	return model->getRemainingSeconds();
}

int16_t RunningPresenter::getCurrentOperatingPressure()
{
	return model->getCurrentOperatingPressure();
}

uint32_t RunningPresenter::getCurrentTotalDose()
{
	return model->getCurrentTotalDose();
}

uint32_t RunningPresenter::getCurrentOutputVolume()
{
	return model->getCurrentOutputVolume();
}

bool RunningPresenter::refreshGenerationInfo()
{
	return model->refreshGenerationInfo();
}

void RunningPresenter::clearGenerationInfoFlag()
{
	return model->clearGenerationInfoFlag();
}

void RunningPresenter::userCancelled()
{
	return model->userCancelled();
}

void RunningPresenter::userOk()
{
	return model->userOk();
}

void RunningPresenter::userOkRelease()
{
	return model->userOkRelease();
}

uint32_t RunningPresenter::getConfiguredTime()
{
	return model->getConfiguredTime();
}

uint16_t RunningPresenter::getVisibleScreen()
{
	return SID_RUNNING;
}

void RunningPresenter::updateProgressRange(int min, int max)
{
	view.InitProgressGraphic(min, max);
}

bool RunningPresenter::isPaused()
{
	return model->isPaused();
}

bool RunningPresenter::isPauseAvailable()
{
	return model->isPauseAvailable();
}




