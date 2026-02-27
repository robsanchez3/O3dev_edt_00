#include <gui/processing_screen/ProcessingView.hpp>
#include <gui/processing_screen/ProcessingPresenter.hpp>

ProcessingPresenter::ProcessingPresenter(ProcessingView& v)
    : view(v)
{

}

void ProcessingPresenter::activate()
{
	view.initTherapyContext(model->getTherapyCtx());
}

void ProcessingPresenter::deactivate()
{

}

void ProcessingPresenter::setGuiTherapy(int8_t value)
{
	model->setGuiTherapy(value);
}

int8_t ProcessingPresenter::getGuiTherapy()
{
	return model->getGuiTherapy();
}

void ProcessingPresenter::periodCalibrationStart()
{
	return model->periodCalibrationStart();
}

void ProcessingPresenter::loadParameterStart()
{
	return model->loadParameterStart();
}

void ProcessingPresenter::saveParameterStart()
{
	return model->saveParameterStart();
}

void ProcessingPresenter::userCancelled()
{
	return model->userCancelled();
}
void ProcessingPresenter::userOk()
{
	return model->userOk();
}
uint8_t ProcessingPresenter::getFsmState()
{
	return model->getFsmState();
}

uint8_t ProcessingPresenter::getCalibrationSucessState()
{
	return model->getCalibrationSucessState();
}

