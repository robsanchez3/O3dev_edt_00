#include <gui/calibration_screen/CalibrationView.hpp>
#include <gui/calibration_screen/CalibrationPresenter.hpp>

CalibrationPresenter::CalibrationPresenter(CalibrationView& v)
    : view(v)
{

}

void CalibrationPresenter::activate()
{

}

void CalibrationPresenter::deactivate()
{

}
void CalibrationPresenter::setGuiTherapy(int8_t value)
{
	model->setGuiTherapy(value);
}

uint16_t CalibrationPresenter::getVisibleScreen()
{
	return SID_CALIBRATION;
}

void CalibrationPresenter::validatePin(const Unicode::UnicodeChar* pin)
{
    // convert Unicode to ASCII
    char asciiPin[5];
    for (int i = 0; i < 4; i++)
        asciiPin[i] = static_cast<char>(pin[i]);
    asciiPin[4] = '\0';

    // send to model for validation
    model->checkPin(asciiPin);
}
void CalibrationPresenter::onPinValidated(bool success)
{
    view.showPinValidationResult(success);
    if (success)
	{
		model->unblockConfigOptions();
	}
}

void CalibrationPresenter::cancelClicked()
{
	model->blockConfigOptions();
}

bool CalibrationPresenter::configOptionsAllowed()
{
	return model->configOptionsAllowed();
}

void CalibrationPresenter::flowCalibrationStart()
{
	return model->flowCalibrationStart();
}

void CalibrationPresenter::o3CalibrationStart()
{
	return model->o3CalibrationStart();
}

bool CalibrationPresenter::isGenerationModeAvailable()
{
	return model->isGenerationModeAvailable();
}

