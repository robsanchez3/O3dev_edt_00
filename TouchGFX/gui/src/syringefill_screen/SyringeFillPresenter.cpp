#include <gui/syringefill_screen/SyringeFillView.hpp>
#include <gui/syringefill_screen/SyringeFillPresenter.hpp>

SyringeFillPresenter::SyringeFillPresenter(SyringeFillView& v)
    : view(v)
{

}

void SyringeFillPresenter::activate()
{
	view.initTherapyContext(model->getTherapyCtx());
	view.initTherapyDataInfo();
	view.initVisualElemnets();
}

void SyringeFillPresenter::deactivate()
{

}

uint16_t SyringeFillPresenter::getVisibleScreen()
{
	return SID_SYRINGE_FILL;
}

uint16_t SyringeFillPresenter::getTherapyTargetValue(uint8_t valueID)
{
	return model->getTherapyTargetValue(valueID);
}

uint8_t SyringeFillPresenter::getFsmState()
{
	return model->getFsmState();
}

void SyringeFillPresenter::userCancelled()
{
	return model->userCancelled();
}

void SyringeFillPresenter::userOk()
{
	return model->userOk();
}

void SyringeFillPresenter::userOkRelease()
{
	return model->userOkRelease();
}

bool SyringeFillPresenter::isCompensatingPressure()
{
	return model->isCompensatingPressure();
}

bool SyringeFillPresenter::isPauseAvailable()
{
	return model->isPauseAvailable();
}

bool SyringeFillPresenter::isPaused()
{
	return model->isPaused();
}

