#include <gui/adjusting_screen/AdjustingView.hpp>
#include <gui/adjusting_screen/AdjustingPresenter.hpp>

#include <stdio.h>

AdjustingPresenter::AdjustingPresenter(AdjustingView& v)
    : view(v)
{

}

void AdjustingPresenter::activate()
{
	view.initTherapyContext(model->getTherapyCtx());
	view.initTherapyDataInfo();
}

void AdjustingPresenter::deactivate()
{
}

uint16_t AdjustingPresenter::getTherapyTargetValue(uint8_t valueID)
{
	return model->getTherapyTargetValue(valueID);
}

uint8_t AdjustingPresenter::getFsmState()
{
	return model->getFsmState();
}

void AdjustingPresenter::StopGeneration()
{
	model->StopGeneration();
}

uint16_t AdjustingPresenter::getVisibleScreen()
{
	return SID_ADJUSTING;
}


