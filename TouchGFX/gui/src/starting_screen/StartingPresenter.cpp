#include <gui/starting_screen/StartingView.hpp>
#include <gui/starting_screen/StartingPresenter.hpp>

StartingPresenter::StartingPresenter(StartingView& v)
    : view(v)
{

}

void StartingPresenter::activate()
{

}

void StartingPresenter::deactivate()
{

}

uint16_t StartingPresenter::getVisibleScreen()
{
	return SID_STARTING;
}

void StartingPresenter::settingsClicked()
{
	model->userOk();
	model->blockConfigOptions();
}
