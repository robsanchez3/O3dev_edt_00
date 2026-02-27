#include <gui/information_screen/InformationView.hpp>

InformationView::InformationView()
{

}

void InformationView::setupScreen()
{
    InformationViewBase::setupScreen();
    setupInfoScreen();
}

void InformationView::tearDownScreen()
{
    InformationViewBase::tearDownScreen();
}

void InformationView::cancelClicked()
{
	printf("InformationView::cancelClicked()....\n");
	presenter->setGuiTherapy(NO_MODE);
	application().gotoCalibrationScreenNoTransition();
}

Unicode::UnicodeChar * InformationView::getSoftwareVersion()
{
//	Unicode::UnicodeChar version[20];
//	Unicode::snprintf(version, 20, "v%s", SOFTWARE_VERSION);
	printf("InformationView::getSoftwareVersion()....%s\n", (char *)presenter->getSoftwareVersion());
	return presenter->getSoftwareVersion();
}

void InformationView::setupVersionsInfo()
{
	char swBuff[30];
	char ctrBuff[30];

	sprintf((char *)swBuff,  "Interface: %s", (char *)presenter->getSoftwareVersion());
	sprintf((char *)ctrBuff, "Control:    %s", (char *)presenter->getControlSoftwareVersion());

	int swLen = Unicode::strlen(swBuff) - 2; // remove last to characters (non official version chars)
	int ctrLen = Unicode::strlen(ctrBuff);

	memset(ta_infoBuffer, 0, sizeof(ta_infoBuffer));
	touchgfx::Unicode::fromUTF8((const uint8_t*)swBuff, ta_infoBuffer, swLen);
	ta_infoBuffer[swLen] = '\n';
	touchgfx::Unicode::fromUTF8((const uint8_t*)ctrBuff, &(ta_infoBuffer[swLen + 1]), ctrLen);
	ta_info.invalidate();
}

void InformationView::setupPressTempInfo()
{
	char pBuff[30];
	int16_t pressure = presenter->getPressure();
	int16_t temp = presenter->getTemperature();
	int16_t len;

	sprintf((char *)pBuff, "Pressure: %d mbar", pressure);
	len =  Unicode::strlen(pBuff);

	memset(ta_infoBuffer, 0, sizeof(ta_infoBuffer));
	touchgfx::Unicode::fromUTF8((const uint8_t*)pBuff, ta_infoBuffer,len);
	ta_infoBuffer[len] = '\n';

	sprintf((char *)pBuff, "Temperature: %d °C", temp);
	touchgfx::Unicode::fromUTF8((const uint8_t*)pBuff, &(ta_infoBuffer[len + 1]), Unicode::strlen(pBuff));
	ta_info.invalidate();
}

void InformationView::setupParametersInfo()
{
	char pBuff[300];

	sprintf((char *)pBuff,  "%s", (char *)presenter->getParameters());
	memset(ta_infoBuffer, 0, sizeof(ta_infoBuffer));
	touchgfx::Unicode::fromUTF8((const uint8_t*)pBuff, ta_infoBuffer,Unicode::strlen(pBuff));
	ta_info.invalidate();
}

void InformationView::setupStartupInfo()
{
	char pBuff[300];

	sprintf((char *)pBuff,  "%s", (char *)presenter->getStartupInfo());
	memset(ta_infoBuffer, 0, sizeof(ta_infoBuffer));
	touchgfx::Unicode::fromUTF8((const uint8_t*)pBuff, ta_infoBuffer,Unicode::strlen(pBuff));
	ta_info.invalidate();
}

void InformationView::setupInfoScreen()
{
	switch(presenter->getGuiTherapy())
	{
		case SW_VERSION_MODE:    setupVersionsInfo();
								    break;
		case PRESSURE_TEMP_MODE: setupPressTempInfo();
			                        break;
		case SHOW_PARAMS_MODE:   setupParametersInfo();
			                        break;
		case SHOW_STARTUP_MODE:  setupStartupInfo();
			                        break;
		default:                    memset(ta_infoBuffer, 0, sizeof(ta_infoBuffer));
			                        ta_info.invalidate();
			                        break;
	}
}
