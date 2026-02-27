#include <gui/switchselect_screen/SwitchSelectView.hpp>

SwitchSelectView::SwitchSelectView()
{

}

void SwitchSelectView::setupScreen()
{
    SwitchSelectViewBase::setupScreen();
    tb_generationMode.forceState(presenter->getGenerationMode());
    tb_generationMode.invalidate();
    generationModeOnEntry = presenter->getGenerationMode();
}

void SwitchSelectView::tearDownScreen()
{
    SwitchSelectViewBase::tearDownScreen();
}

void SwitchSelectView::cancelClicked()
{
	printf("SwitchSelectView::cancelClicked()....\n");

	presenter->generationModeClicked(generationModeOnEntry);
	presenter->setGuiTherapy(NO_MODE);
	application().gotoCalibrationScreenNoTransition();
}

void SwitchSelectView::okClicked()
{
	printf("SwitchSelectView::okClicked()....\n");
	presenter->setGuiTherapy(NO_MODE);
	application().gotoCalibrationScreenNoTransition();
}

void SwitchSelectView::generationModeClicked()
{
    bool state = tb_generationMode.getState();

    printf("SwitchSelectView::generationModeClicked()....state: %d\n", state);
	presenter->generationModeClicked(state);
}
