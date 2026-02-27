#include <gui/mainmenu_screen/MainMenuView.hpp>
#include <gui/mainmenu_screen/MainMenuPresenter.hpp>

MainMenuPresenter::MainMenuPresenter(MainMenuView& v)
    : view(v)
{

}

void MainMenuPresenter::activate()
{
}

void MainMenuPresenter::deactivate()
{

}

int8_t * MainMenuPresenter::getDeviceConfig()
{
	return model->getDeviceConfig();
}

void MainMenuPresenter::setGuiTherapy(int8_t value)
{
	model->setGuiTherapy(value);

}

bool MainMenuPresenter::isCalibrationErrorDuringStartUp()
{
	return model->isCalibrationErrorDuringStartUp();

}

bool MainMenuPresenter::isVaccumAvailable()
{
	return model->isVaccumAvailable();

}

uint16_t MainMenuPresenter::getVisibleScreen()
{
	return SID_MAIN_MENU;
}
