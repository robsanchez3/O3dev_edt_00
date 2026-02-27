#include <gui/end_screen/EndView.hpp>
#include <gui/end_screen/EndPresenter.hpp>

EndPresenter::EndPresenter(EndView& v)
    : view(v)
{

}

void EndPresenter::activate()
{
	view.initTherapyContext(model->getTherapyCtx());
	view.initTherapyDataInfo();
	view.initTherapyEndInfo();
	view.initVisibleItems();
}

void EndPresenter::deactivate()
{

}

void EndPresenter::StartGeneration(void)
{
	model->StartGeneration();
}

uint16_t EndPresenter::getTherapyTargetValue(uint8_t valueID)
{
	return model->getTherapyTargetValue(valueID);
}

uint8_t EndPresenter::getFsmState()
{
	return model->getFsmState();
}

uint8_t EndPresenter::getRemainingMinutes()
{
	return model->getRemainingMinutes();
}

uint8_t EndPresenter::getRemainingSeconds()
{
	return model->getRemainingSeconds();
}

uint32_t EndPresenter::getCurrentTotalDose()
{
	return model->getCurrentTotalDose();
}

uint32_t EndPresenter::getCurrentOutputVolume()
{
	return model->getCurrentOutputVolume();
}

uint32_t EndPresenter::getConfiguredTime()
{
	return model->getConfiguredTime();
}

uint16_t EndPresenter::getVisibleScreen()
{
	return SID_END;
}

void EndPresenter::gotoRepose()
{
	model->gotoRepose();
}


