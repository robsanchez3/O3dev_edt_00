#include <gui/washing_screen/WashingView.hpp>
#include <gui/washing_screen/WashingPresenter.hpp>

WashingPresenter::WashingPresenter(WashingView& v)
    : view(v)
{

}

void WashingPresenter::activate()
{
	view.initTherapyContext(model->getTherapyCtx());
	view.initTherapyDataInfo();
}

void WashingPresenter::deactivate()
{

}

uint16_t WashingPresenter::getVisibleScreen()
{
	return SID_WASHING;
}

uint16_t WashingPresenter::getTherapyTargetValue(uint8_t valueID)
{
	return model->getTherapyTargetValue(valueID);
}

bool WashingPresenter::refreshGenerationInfo()
{
	return model->refreshGenerationInfo();
}

void WashingPresenter::clearGenerationInfoFlag()
{
	return model->clearGenerationInfoFlag();
}

uint8_t WashingPresenter::getWashingSeconds()
{
	return model->getWashingSeconds();
}

void WashingPresenter::cancelWashing()
{
	return model->cancelWashing();
}

void WashingPresenter::washingExtesion()
{
	return model->washingExtesion();
}


