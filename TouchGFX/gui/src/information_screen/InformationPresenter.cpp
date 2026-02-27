#include <gui/information_screen/InformationView.hpp>
#include <gui/information_screen/InformationPresenter.hpp>

InformationPresenter::InformationPresenter(InformationView& v)
    : view(v)
{

}

void InformationPresenter::activate()
{

}

void InformationPresenter::deactivate()
{

}
void InformationPresenter::setGuiTherapy(int8_t value)
{
	model->setGuiTherapy(value);
}

int8_t InformationPresenter::getGuiTherapy()
{
	return model->getGuiTherapy();
}

Unicode::UnicodeChar * InformationPresenter::getSoftwareVersion()
{
	return (Unicode::UnicodeChar *) model->getSoftwareVersion();
}

Unicode::UnicodeChar * InformationPresenter::getControlSoftwareVersion()
{
	return (Unicode::UnicodeChar *) model->getControlSoftwareVersion();
}

int16_t InformationPresenter::getPressure()
{
	return model->getPressure();
}
int16_t InformationPresenter::getTemperature()
{
	return model->getTemperature();
}

Unicode::UnicodeChar * InformationPresenter::getParameters()
{
	return (Unicode::UnicodeChar *) model->getParameters();
}

Unicode::UnicodeChar * InformationPresenter::getStartupInfo()
{
	return (Unicode::UnicodeChar *) model->getStartupInfo();
}

