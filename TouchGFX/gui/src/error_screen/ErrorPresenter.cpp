#include <gui/error_screen/ErrorView.hpp>
#include <gui/error_screen/ErrorPresenter.hpp>

ErrorPresenter::ErrorPresenter(ErrorView& v)
    : view(v)
{

}

void ErrorPresenter::activate()
{
	if(model->getGuiTherapy() >= 0)
	{
//      therapy already selected (else starting)
		view.initTherapyContext(model->getTherapyCtx());
		view.initTherapyDataInfo();
	}
	view.initErrorCode();
}

void ErrorPresenter::deactivate()
{
}

uint16_t ErrorPresenter::getTherapyTargetValue(uint8_t valueID)
{
	return model->getTherapyTargetValue(valueID);
}

bool ErrorPresenter::isStarting()
{
	return model->isStarting();
}

uint8_t ErrorPresenter::getErrorState()
{
	return model->getErrorState();
}

void ErrorPresenter::exitError()
{
//	printf("Error exited from ErrorPresenter::exitError...\n");
	model->exitError();
}

uint16_t ErrorPresenter::getVisibleScreen()
{
	return SID_ERROR;
}


