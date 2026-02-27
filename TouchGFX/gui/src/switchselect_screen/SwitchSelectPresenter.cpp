#include <gui/switchselect_screen/SwitchSelectView.hpp>
#include <gui/switchselect_screen/SwitchSelectPresenter.hpp>

SwitchSelectPresenter::SwitchSelectPresenter(SwitchSelectView& v)
    : view(v)
{

}

void SwitchSelectPresenter::activate()
{

}

void SwitchSelectPresenter::deactivate()
{

}

void SwitchSelectPresenter::setGuiTherapy(int8_t value)
{
	model->setGuiTherapy(value);
}

void SwitchSelectPresenter::generationModeClicked(bool state)
{
	model->setGenerationMode(state);
}

bool SwitchSelectPresenter::getGenerationMode()
{
	return model->getGenerationMode();
}

