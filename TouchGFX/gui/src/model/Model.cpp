#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <gui/common/FrontendApplication.hpp>
#include <gui/model/parseappconfig.hpp>
#include <gui/model/parseUpdates.hpp>

#ifndef SIMULATOR

////////////////#include <gui/widget/edt_f7xxh7xx_TestAPI.h>  TODO ¿quitar widget?
#include <gui/model/TherapyTemplates.hpp>
#include <gui/model/LogManage.hpp>
#include "usbd_cdc_if.h"  // TODO analyze if still needed
#include "BitmapDatabase.hpp"
#include "cmsis_os2.h"
#include "../../../../Drivers/O3/Fsm_o3/fsm_o3_api.h"
extern "C" osSemaphoreId_t hSysConfigReady;  /* signals defaultTask that sys config is ready */
////extern "C" uint8_t usb_fatfs_initialized;

// ojoooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// ojoooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
uint8_t usb_fatfs_initialized = 0; // TODO: remove when SD option tested
//TODO look for USB references and remove if not needed
// ojoooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// ojoooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
// ojoooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
#endif

#define PIN "1234"  // hard coded PIN




Model* modelInstance = nullptr;

Model::Model() : modelListener(0)
{
    modelInstance = this;

	initTherapyTargetValues();
	initStorageDelegates();

#ifndef SIMULATOR

#if 0 //  legacy code from original demonstration application (start)
	TestAPI_ThreadInit();
	EDT_GPIO_Mode(GPIO4_GPIO_PORT,GPIO4_PIN,GPIO_OUT);
	EDT_GPIO_Mode(GPIO5_GPIO_PORT,GPIO5_PIN,GPIO_OUT);
#endif //  legacy code from original demonstration application (start)

#endif
}

void Model::tick()
{
	static char debug_done = 0;
//  Update screen according to current FSM state

//  REMEMBER: The Model has a pointer to your currently active Presenter by means of ModelListener (where getVisibleScreen() has been defined as virtual)


	// TODO: analyze if it is possible change screen in "controlled way"
	// TODO: analyze if it is possible change screen in "controlled way"
	switch(getFsmState())
	{
	case STATE_ERROR:
		if(modelListener->getVisibleScreen() != SID_ERROR)
		{
			static_cast<FrontendApplication*>(Application::getInstance())->gotoErrorScreenNoTransition();
		}
		break;
	case STATE_WAITING_FOR_PROTOCOL:
		// just debug
		if(!debug_done)
		{
			printf("Menu config debug: deviceConfig[0]= %d, ", deviceConfig[0]);
			for(uint8_t i = 1; i < MAX_DEV_THERAPIES; i++)
			{
				printf("[%d]= %d, ", i, deviceConfig[i]);
			}
			printf("\n");
			debug_done = 1;
		}
		// end just debug

		if(modelListener->getVisibleScreen() == SID_STARTING)  // End of initial starting process
		{
			static_cast<FrontendApplication*>(Application::getInstance())->gotoMainMenuScreenNoTransition();
		}
		if(modelListener->getVisibleScreen() == SID_END) // End of therapy
		{
			static_cast<FrontendApplication*>(Application::getInstance())->gotoMainMenuScreenNoTransition();
		}
		break;
	case STATE_O3_GENERATING:
	case STATE_VACUUM_GENERATING:
		switch(guiTherapy)
		{
		case SYRINGE_MODE:
		case SYRINGE_AUTO_MODE:
		case SYRINGE_MANUAL_MODE: if(modelListener->getVisibleScreen() != SID_SYRINGE_FILL )  static_cast<FrontendApplication*>(Application::getInstance())->gotoSyringeFillScreenNoTransition();
		break;
		case CLOSED_BAG_MODE:     modelListener->updateProgressRange(0, fsm_o3_getTime());  // TODO: try to update just once
		//	break;                intentionally no break to force default action
		default:                  if(modelListener->getVisibleScreen() != SID_RUNNING) static_cast<FrontendApplication*>(Application::getInstance())->gotoRunningScreenNoTransition();
		}

	break;
	case STATE_USER_CANCELLED:
	case STATE_COMPLETED:
	case STATE_OVERPRESSURE:
		if(modelListener->getVisibleScreen() != SID_END)
		{
			static_cast<FrontendApplication*>(Application::getInstance())->gotoEndScreenNoTransition();
		}
		break;
	case STATE_WASHING:
		if(modelListener->getVisibleScreen() != SID_WASHING)
		{
			static_cast<FrontendApplication*>(Application::getInstance())->gotoWashingScreenNoTransition();
		}
		break;
	case STATE_WAITING_THERAPY_TIME:
		modelListener->updateProgressRange(0, fsm_o3_getTime()); // TODO: try to update just once
		break;
	case STATE_WAITING_EXTERNAL_STUFF:  //TODO should it be STATE_WAITING_EXTERNAL_STUFF_CONNECTION?
		if(modelListener->getVisibleScreen() != SID_WASHING)
		{
			static_cast<FrontendApplication*>(Application::getInstance())->gotoWashingScreenNoTransition();
		}
		break;
	case STATE_ADJUSTING:
	case STATE_TUNING_O3_SENSOR:
		if(modelListener->getVisibleScreen() != SID_ADJUSTING)
		{
			static_cast<FrontendApplication*>(Application::getInstance())->gotoAdjustingScreenNoTransition();
		}
		break;
#if 1
	case STATE_WAITING_FOR_SERVICE:
//  	if( (modelListener->getVisibleScreen() != SID_CALIBRATION) && (fsm_o3_getOption() == NO_MODE) )
		if( (modelListener->getVisibleScreen() != SID_CALIBRATION) && (fsm_o3_getOption() == SERVICE_MODE) )
		{
			static_cast<FrontendApplication*>(Application::getInstance())->gotoCalibrationScreenNoTransition();
		//	ChangeCurrentState(STATE_WAITING_FOR_SERVICE);
		}
			break;
#endif
	default:
		debug_done = 0;
		break;
	}

#if 0
//
//  Configure hardware description according to menu configuration file on disk FATFS (if available)
//
	if(!hw_configured)
	{
		//  wait for disk FATFS initialization
//		if(usb_fatfs_initialized) // TODO: remove when SD option tested
		if(0)//if(is_SD() && is_fs_mounted() && hw_configuration_tout > 10)
		{
			hw_configured = 1;
			if( loadHardwareConfig(fsm_o3_getHwConfig()) == 0)
			{
				printf("Hardware configuration loaded from disk.\n");
			}
			else
			{
				printf("Hardware configuration not loaded from disk, using default.\n");
			}
		}
		else if(++hw_configuration_tout > 100)   // configure by default after some time
		{
			hw_configured = 1;
			printf("Hardware configuration not loaded, using default.\n");
		}
	}
//
//  Configure syringe stop pattern according to menu configuration file on disk FATFS (if available)
//
	if(!syr_configured)
	{
		//  wait for disk FATFS initialization
//		if(usb_fatfs_initialized)
		if(0)//if(is_SD() && is_fs_mounted())
		{
			syr_configured = 1;
			if( loadSyringeConfig(fsm_o3_getSyringePattern()) == 0)
			{
				printf("Syringe stop pattern loaded from disk.\n");
			}
			else
			{
				printf("Syringe stop pattern not loaded from disk, using default.\n");
			}
		}
		else if(++syr_configuration_tout > 100)   // configure by default after some time
		{
			syr_configured = 1;
			printf("Syringe stop pattern not loaded, using default.\n");
		}
	}
//
//  Configure software description according to menu configuration file on disk FATFS (if available)
//
	if(!usr_configured)
	{
		//  wait for disk FATFS initialization
//		if(usb_fatfs_initialized)
		if(0)//if(is_SD() && is_fs_mounted())
		{
			usr_configured = 1;
			if( loadUserConfig((USR_CONFIG_T*)fsm_o3_getUsrConfig()) == 0)
			{
				printf("User configuration loaded from disk.\n");
			}
			else
			{
				printf("User configuration not loaded from disk, using default.\n");
			}
		}
		else if(++usr_configuration_tout > 100)   // configure by default after some time
		{
			usr_configured = 1;
			printf("User configuration not loaded, using default.\n");
		}
	}
//
//  Configure main menu according to menu configuration file on disk FATFS (if available)
//
	if(!main_menu_configured)
	{
		//  wait for disk FATFS initialization
//		if(usb_fatfs_initialized)
		if(0)//if(is_SD() && is_fs_mounted())
		{
			main_menu_configured = 1;
			configureMainMenu();
//			printf("Main menu configured after %lu ticks.\n", main_menu_configuration_tout);
		}

		if(main_menu_configuration_tout > 25)   // configure by default after some time
		{
			if(is_SD() && is_SD_mounted())
			{
				configureMainMenu();
				main_menu_configured = 1;
			}
		}
    	else if(++main_menu_configuration_tout > 100)   // configure by default after some time
		{
			setDefaultMainMenu();
			main_menu_configured = 1;
		}
	}
#endif
//
//  Load all system configuration from SD card (if available)
//
	/*
	if(!sys_configured)
	{
		if(++sys_configuration_tout > 25)   // wait until OS started and SD/FS initialized
		{
			if(is_SD() && is_SD_mounted())
			{
				sys_configured = 1;
				if( loadHardwareConfig(fsm_o3_getHwConfig()) == 0 )
					printf("Hardware configuration loaded from disk.\n");
				else
					printf("Hardware configuration not loaded from disk, using default.\n");

				if( loadSyringeConfig(fsm_o3_getSyringePattern()) == 0 )
					printf("Syringe stop pattern loaded from disk.\n");
				else
					printf("Syringe stop pattern not loaded from disk, using default.\n");

				if( loadUserConfig((USR_CONFIG_T*)fsm_o3_getUsrConfig()) == 0 )
					printf("User configuration loaded from disk.\n");
				else
					printf("User configuration not loaded from disk, using default.\n");

				if( loadMainMenu(deviceConfig, MAX_DEV_THERAPIES) == 0 )
					printf("main menu loaded from disk.\n");
				else
				{
					setDefaultMainMenu();
					printf("Main menu not loaded, using default.\n");
				}
			}
			else if(sys_configuration_tout > 100)   // configure by default after some time
			{
				sys_configured = 1;
				setDefaultMainMenu();
				printf("System configuration not loaded, using defaults.\n");
			}
		}
	}

*/
	// USB detection and system config loading handled by ConfigLoaderTask

#if 0 //  legacy code from original demonstration application (start)

	tickcount++;

	if (tickcount > 2)
	{
		tickcount=0;
#ifndef SIMULATOR
		GPIO_0_State = EDT_GPIO_Read(GPIO0_GPIO_PORT, GPIO0_PIN);  // read GPIO 0
		if (GPIO_0_State != old_GPIO_0_State)
		{
			old_GPIO_0_State = GPIO_0_State;
			if (GPIO_0_State == 0)
			{
				modelListener->ShowGPIO_0_State();
			}
		}
		GPIO_1_State = EDT_GPIO_Read(GPIO1_GPIO_PORT, GPIO1_PIN);   // Read GPIO 1
		if (GPIO_1_State != old_GPIO_1_State)
		{
			if (GPIO_1_State == 0)
			{
				modelListener->PopUpWindowGPIO1();
			}
			old_GPIO_1_State = GPIO_1_State;
		}
#endif  
	}

	if (pulsecount <= 100)
	{
		pulsecount++;
		if(pulsecount == 100)
		{
			touchgfx_printf("\rGPIO4 Set HIGH\n\r");
#ifndef SIMULATOR    
			EDT_GPIO_Write(GPIO4_GPIO_PORT,GPIO4_PIN, GPIO_PIN_SET);   // Turn OFF LED on Testboard
#endif
		}
	}
	// Check if any character are received on RS232
	// Characters are received under interrupt control and transferred via:
	// void rs232Rx_task(void *pvParameters) in file edt_F750_USARTAPI.c
	//
	// It is up to the user to implement any Protocol to his own Choise
#ifndef SIMULATOR     
/*  if(_RS232RevSt.RevF==true)
    {
    	_RS232RevSt.RevF=false;
    	modelListener->RS232CharReceived(_RS232RevSt.pdata,_RS232RevSt.size); //
    	memset(_RS232RevSt.pdata,0,sizeof(_RS232RevSt.pdata));
    	_RS232RevSt.size=0;
 	 }*/
	extern uint8_t CRxData[];
	if(CanHandleSt.CAN_HandleDataEn == true)
	{
		CanHandleSt.CAN_HandleDataEn = false;
		modelListener->CANReceived(CRxData);
	}
#endif // SIMULATOR

#endif //  legacy code from original demonstration application (end)


} // Model::tick()

void Model::checkPin(const char* pin)
{
	const char correctPin[] = PIN;  // valid PIN

	if (strcmp(pin, correctPin) == 0)
	{
		modelListener->onPinValidated(true); // notify presenter right PIN
	}
	else
	{
		modelListener->onPinValidated(false); // notify presenter wrong PIN
	}
}

void Model::blockConfigOptions()
{
	configBlocked = true;
}

void Model::unblockConfigOptions()
{
	configBlocked = false;
}

bool Model::configOptionsAllowed()
{
	return !configBlocked;
}

void Model::setGuiTherapy(int8_t value)
{
	guiTherapy = value;
	setupTherapyContext(value);   //TODO: consider to move this line after the next one

	if(value == CAL_PRESS_MODE)
	{
		printf("Resetting current pressure calibration values...\n");
		fsm_o3_calibratePressureInit();
	}
#ifndef SIMULATOR
	fsm_o3_setOption(fsm_o3_convertTherapyOption((OPERATION_MODE_E) value));
	fsm_o3_setTemperatureMonitoring(1);
//	printf("Model setGuiTherapy: value: %d - fsm_o3_getOption(): %d\n", value, fsm_o3_getOption());
#endif
}

int8_t Model::getGuiTherapy()
{
	return guiTherapy;
}

void Model::initTherapyTargetValues()
{
	printf("Init therapyTargetValues... (now handled by fsm_o3_api)\n");
}

void Model::initStorageDelegates(void)
{
	fsm_o3_registerStorage(
		(int8 (*)())openParamFileWrite,
		(int8 (*)())openParamFileRead,
		(int8 (*)(uint16, int32))writeLogLine,
		(int8 (*)(uint16, int32 *))readLogLine,
		closeParamFile
	);
}

void Model::setTherapyTargetValue(uint8_t valueID, uint16_t value)
{
//	printf("Model setTherapyTargetValue. valueID: %d, value: %d\n", valueID, value);
#ifndef SIMULATOR
	fsm_o3_setTherapyParam(valueID, value);
#endif
}

uint16_t Model::getTherapyTargetValue(uint8_t valueID)
{
//	printf("Model getTherapyTargetValue. valueID: %d, value: %d\n", valueID, fsm_o3_getTherapyParam(guiTherapyCtx.therapyTargetValue[valueID]));
#ifndef SIMULATOR
	return (uint16_t)fsm_o3_getTherapyParam(guiTherapyCtx.therapyTargetValue[valueID]);
#else
	return 0;
#endif
}

void Model::setGenerationMode(bool state)
{
//	GLB_fsm_o3.GenerationMode = state ? O3_GENERATION_BASED_ON_O3_PHOTOSENSOR : O3_GENERATION_BASED_ON_TUBE_CALIBRATION;
//	printf("GenerationModeon on Model setGenerationMode: %d\n", GLB_fsm_o3.GenerationMode);
	fsm_o3_setGenerationMode(state ? O3_GENERATION_BASED_ON_O3_PHOTOSENSOR : O3_GENERATION_BASED_ON_TUBE_CALIBRATION);
	saveUserConfig((USR_CONFIG_T*)fsm_o3_getUsrConfig());
	printf("GenerationModeon on Model setGenerationMode: %d\n", fsm_o3_getGenerationMode());
}

bool Model::getGenerationMode()
{
//	printf("GenerationMode on Model getGenerationMode: %d\n", GLB_fsm_o3.GenerationMode);
//	return ((bool)GLB_fsm_o3.GenerationMode);
	printf("GenerationMode on Model getGenerationMode: %d\n", fsm_o3_getGenerationMode());
	return ((bool)fsm_o3_getGenerationMode());
}

bool Model::isGenerationModeAvailable()
{
//	printf("GenerationMode availability: %d\n", GLB_fsm_o3.HwConfig.O3Sensor);
	return ((bool)fsm_o3_hasO3Sensor());
}

bool Model::isVaccumAvailable()
{
//	printf("Vacuum pump availability: %d\n", GLB_fsm_o3.HwConfig.VPump);
	return ((bool)fsm_o3_hasVacuumPump());
}

char * Model::getSoftwareVersion()
{
	return (char *)O3_LIB_VERSION;
}

char * Model::getControlSoftwareVersion()
{
	fsm_o3_refreshGeneratorVersion();
	return fsm_o3_getSharedBuffer();
}

int16_t Model::getPressure()
{
	return fsm_o3_getPressureSensor();
}

int16_t Model::getTemperature()
{
	return fsm_o3_getTemperatureSensor();
}

char * Model::getParameters()  //TOTO: improve naming
{
	fsm_o3_refreshParameters();
	return fsm_o3_getSharedBuffer();
}

char * Model::getStartupInfo()
{
	fsm_o3_refreshStartupInfo();
	return fsm_o3_getSharedBuffer();
}

int8_t * Model::getDeviceConfig()
{
	return deviceConfig;
}

void Model::periodCalibrationStart()
{
	fsm_o3_gotoCalibratePeriod();
}

void Model::loadParameterStart()
{
	fsm_o3_gotoLoadParameters();
}

void Model::saveParameterStart()
{
	fsm_o3_gotoSaveParameters();
}

void Model::flowCalibrationStart()
{
	fsm_o3_gotoCalibrateFlow();
}

void Model::o3CalibrationStart()
{
	fsm_o3_gotoCalibrateO3();
}

void Model::onSelectionAction(uint8_t selectionStep)
{
	guiTherapyCtx.okAction[selectionStep]();
}

void Model::onSliderAction(uint8_t step)
{
	guiTherapyCtx.sliderAction[step]();
}


THERAPY_CTX * Model::getTherapyCtx()
{
	return &guiTherapyCtx;
}

void Model::EndSelection(void)
{
//	printf("End of selection (model)...\n");
#ifndef SIMULATOR
//	Experimental_LaunchTherapy(); //TODO: Launch synchronously from state machine
	fsm_o3_sendEnter();

	if(getFsmState() >= STATE_WAITING_FOR_SERVICE)
	{
		static_cast<FrontendApplication*>(Application::getInstance())->gotoCalibrationScreenNoTransition();
	}
	else
	{
		switch(fsm_o3_getOption())
		{
			case VACUUM_MODE:
			case VACUUM_TIME_MODE:
			case VACUUM_PRESSURE_MODE:
				static_cast<FrontendApplication*>(Application::getInstance())->gotoRunningScreenNoTransition();
				break;
		default:
			static_cast<FrontendApplication*>(Application::getInstance())->gotoAdjustingScreenNoTransition();
		}

	}
#endif
}

void Model::StartGeneration(void)
{
	printf("Start generation (model)...\n");
#ifndef SIMULATOR
//	Experimental_LaunchTherapy(); //TODO: Launch synchronously from state machine
	fsm_o3_sendEnter();
	static_cast<FrontendApplication*>(Application::getInstance())->gotoAdjustingScreenNoTransition();
#endif
}

void Model::CancelSelection(void)
{
//	printf("Cancel selection (model)...\n");

	if(getFsmState() >= STATE_WAITING_FOR_SERVICE)
	{
		static_cast<FrontendApplication*>(Application::getInstance())->gotoCalibrationScreenNoTransition();
	}
	else
	{
		static_cast<FrontendApplication*>(Application::getInstance())->gotoMainMenuScreenNoTransition();
	}
}

void Model::StopGeneration(void)
{
	printf("Stop generation (model)...\n");
#ifndef SIMULATOR
//	Experimental_StopTherapy(); //TODO: Launch synchronously from state machine
	fsm_o3_sendCancel();
#endif
}

void Model::exitError(void)
{
	printf("Exit error (model)...\n");
#ifndef SIMULATOR
//	ExitError(); //TODO: Launch synchronously from state machine
	fsm_o3_sendCancel();
#endif
}

void Model::userCancelled(void)
{
//	printf("User cancelled (model)...\n");
#ifndef SIMULATOR
//	GotoUserCanceled(); //TODO: Launch synchronously from state machine
	fsm_o3_sendCancel();
	fsm_o3_setTemperatureMonitoring(0);
#endif
}

void Model::userOk(void)
{
//	printf("User OK (model)...\n");
#ifndef SIMULATOR
//	KeyOnPush();//TODO: Launch synchronously from state machine
	fsm_o3_sendEnter();
#endif
}

void Model::userOkRelease(void)
{
//	printf("User OK released (model)...\n");
#ifndef SIMULATOR
//	KeyOnRelease();//TODO: Launch synchronously from state machine
	fsm_o3_sendGeneric(); // to handle OK release event
#endif
}

void Model::cancelWashing(void)
{
	printf("Washing cancelled (model)...\n");
#ifndef SIMULATOR
//	CancelWashing(); //TODO: Launch synchronously from state machine
	fsm_o3_sendCancel();
#endif
}

void Model::washingExtesion(void)
{
	printf("Washing extension (model)...\n");
#ifndef SIMULATOR
//	WashingExtesion(); //TODO: Launch synchronously from state machine
	fsm_o3_sendEnter();
#endif
}

void Model::gotoRepose(void)
{
//	printf("Go to repose (model)...\n");
#ifndef SIMULATOR
//	GotoMainMenu(); //TODO: Launch synchronously from state machine
	fsm_o3_sendCancel();
#endif
}

#define IS_STARTING()  ( (fsm_o3_getStateId() == STATE_INIT) || (fsm_o3_getStateId() == STATE_INIT_CHECK_1) || (fsm_o3_getStateId() == STATE_INIT_CHECK_2) || (fsm_o3_getStateId() == STATE_INIT_CHECK_3) )

bool Model::isStarting()
{
#ifndef SIMULATOR
//	return (bool) GLB_fsm_o3.Starting;
	/* Is starting */
	return (bool) ( (fsm_o3_getStateId() == STATE_INIT) ||
                    (fsm_o3_getStateId() == STATE_INIT_CHECK_1) ||
	                (fsm_o3_getStateId() == STATE_INIT_CHECK_2) ||
	                (fsm_o3_getStateId() == STATE_INIT_CHECK_3) );
#else
	return 0;
#endif
}

bool Model::isCalibrationErrorDuringStartUp()
{
#ifndef SIMULATOR
	return (bool) fsm_o3_hasCalibrationErrorDuringStartUp();
#else
	return 0;
#endif
}

bool Model::isCompensatingPressure()
{
#ifndef SIMULATOR
	return (bool) fsm_o3_getDepressureSeconds();
#else
	return 0;
#endif
}

bool Model::isPaused()
{
#ifndef SIMULATOR
	if( (fsm_o3_getOption() == INSUFFLATION_R_MODE) || (fsm_o3_getOption() == INSUFFLATION_V_MODE) )
	{
		return !((bool) fsm_o3_getInsufflationState());
	}
	else if( (fsm_o3_getOption() == SYRINGE_MANUAL_MODE) )
	{
		return !((bool) fsm_o3_getSyringeManualState());
	}
	else if(fsm_o3_getOption() == VACUUM_TIME_MODE)
	{
		return ( (fsm_o3_getVacuumStatus() == VACUUM_STATE_RUNNING) ? FALSE : TRUE );   //TODO ¿resolve as other cases and remove VACUUM_LOGICAL_STATE_E from fsm_o3_types.h?
	}
	else if(fsm_o3_getOption() == MANUAL_MODE)
	{
		return (FALSE);
	}
	else
	{
		return (FALSE);
	}
#else
	return 0;
#endif
}

bool Model::isPauseAvailable()
{
#ifndef SIMULATOR
	if( (fsm_o3_getOption() == SYRINGE_MANUAL_MODE) || (fsm_o3_getOption() == INSUFFLATION_R_MODE) || (fsm_o3_getOption() == INSUFFLATION_V_MODE) || (fsm_o3_getOption() == VACUUM_TIME_MODE) )
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
#else
	return 0;
#endif
}

uint8_t Model::getFsmState()
{
#ifndef SIMULATOR
	return fsm_o3_getStateId();
#else
	return 0;
#endif
}

uint8_t Model::getErrorState()
{
#ifndef SIMULATOR
	return fsm_o3_getErrorState();
#else
	return 0;
#endif
}

uint8_t Model::getRemainingMinutes()
{
#ifndef SIMULATOR
	return fsm_o3_getRemainingMinutes();
#else
	return 0;
#endif
}

uint8_t Model::getRemainingSeconds()
{
#ifndef SIMULATOR
	return fsm_o3_getRemainingSeconds();
#else
	return 0;
#endif
}

uint8_t Model::getWashingSeconds()
{
#ifndef SIMULATOR
	return fsm_o3_getWashingSeconds();
#else
	return 0;
#endif
}


int16_t Model::getCurrentOperatingPressure()
{
#ifndef SIMULATOR
	return fsm_o3_getOperatingPressure();
#else
	return 0;
#endif
}

uint32_t Model::getCurrentTotalDose()
{
#ifndef SIMULATOR
	return fsm_o3_getTotalDose();
#else
	return 0;
#endif
}

uint32_t Model::getCurrentOutputVolume()
{
#ifndef SIMULATOR
	return fsm_o3_getOutputVolume();
#else
	return 0;
#endif
}

uint32_t Model::getConfiguredTime()
{
#ifndef SIMULATOR
	return fsm_o3_getTime();
#else
	return 60;
#endif
}
uint32_t Model::getConfiguredVolume()
{
#ifndef SIMULATOR
	return fsm_o3_getTherapyParam(TTV_VOLUME);
#else
	return 1000;
#endif
}

bool Model::refreshGenerationInfo()
{
#ifndef SIMULATOR
	return (bool) fsm_o3_getRefreshScreen();
#else
	return 0;
#endif
}

void Model::clearGenerationInfoFlag()
{
#ifndef SIMULATOR
	fsm_o3_setRefreshScreen(0);
#endif
}

uint8_t Model::getCalibrationSucessState()
{
#ifndef SIMULATOR
	return STATE_CALIBRATION_END;
#else
	return 0;
#endif
}

/*
 * Therapy context initialization
 */
void Model::setupDefaultTherapyContext(int8_t therapyID)  // TODO: refactor name to tHerapy....
{
    const THERAPY_CTX *tpl = &therapyTemplates[therapyID];

    guiTherapyCtx.stepsNum = tpl->stepsNum;
    guiTherapyCtx.mainIcon = tpl->mainIcon;
    guiTherapyCtx.endTotalDoseVisible = tpl->endTotalDoseVisible;
    guiTherapyCtx.endTotalVolumeVisible = tpl->endTotalVolumeVisible;
    guiTherapyCtx.endTotalTimeVisible = tpl->endTotalTimeVisible;
    guiTherapyCtx.okButtonVisible = tpl->okButtonVisible;
    guiTherapyCtx.bigPauseButtonVisible = tpl->bigPauseButtonVisible;
    guiTherapyCtx.maxPressure = tpl->maxPressure;

//  for (int i = 0; i < tpl->stepsNum; i++)
    for (int i = 0; i < MAX_THERAPY_STEPS; i++)
    {
        guiTherapyCtx.icon[i] = tpl->icon[i];
        guiTherapyCtx.minValue[i] = tpl->minValue[i];
        guiTherapyCtx.defValue[i] = tpl->defValue[i];
        guiTherapyCtx.maxValue[i] = tpl->maxValue[i];
        guiTherapyCtx.negativeValue[i] = tpl->negativeValue[i];
        guiTherapyCtx.step[i] = tpl->step[i];
        guiTherapyCtx.units[i] = tpl->units[i] ? tpl->units[i] : "";
        guiTherapyCtx.therapyTargetValue[i] = tpl->therapyTargetValue[i];
        guiTherapyCtx.secondSelectionVisible[i] = tpl->secondSelectionVisible[i];
        guiTherapyCtx.delayIndicatorTime[i] = tpl->delayIndicatorTime[i];
        guiTherapyCtx.selectAvailable[i] = tpl->selectAvailable[i];
        guiTherapyCtx.okAction[i] = tpl->okAction[i];
        guiTherapyCtx.sliderAction[i] = tpl->sliderAction[i];
    }
}

void Model::setupTherapyContext(int8_t therapyID)// TODO: refactor name to tHerapy....
{
	memset(&guiTherapyCtx, 0, sizeof(guiTherapyCtx));

	for(int8_t i = 0; i < MAX_THERAPY_STEPS; i++)
	{
		guiTherapyCtx.okAction[i] = fsm_o3_no_op;       // avoid null pointer dereference
		guiTherapyCtx.sliderAction[i] = fsm_o3_no_op;   // avoid null pointer dereference
		guiTherapyCtx.units[i] = "";                     // point to empty string literal
	}
	setupDefaultTherapyContext(therapyID);
	print_guiTherapyCtx(&guiTherapyCtx);

	if( !loadTherapyFromFile((OPERATION_MODE_E)therapyID, &guiTherapyCtx) )
	{
		printf("Failed to load therapy context from file for therapy code %d. Using default values.\n", therapyID);
		setupDefaultTherapyContext(therapyID);
	}

	/*
	switch(therapyID)
	{// TODO consider moving outside the graphic resources....
	case SYRINGE_MODE:
	case SYRINGE_AUTO_MODE:	  GLB_fsm_o3.ConfiguredTime = 1;  break;
	case SYRINGE_MANUAL_MODE: GLB_fsm_o3.ConfiguredTime = 10; break;
	case MANUAL_MODE:         GLB_fsm_o3.ConfiguredTime = 10; break;
	case DENTAL_MODE:         GLB_fsm_o3.ConfiguredTime = 1;  break;
	}
*/

	/* Conditionally modify TherapyTemplates.hpp defined values */
	if(therapyID == CAL_O3_MODE)
	{
		guiTherapyCtx.delayIndicatorTime[0] = fsm_o3_hasO3Sensor() ? 100000 : 20000;
	}

	print_guiTherapyCtx(&guiTherapyCtx); // debug

	/*  Initialize therapy specific default values */
	fsm_o3_setPressThreshold(guiTherapyCtx.maxPressure);

	/*
	 * First value in this loop is corrupted to 0 value.
	 * Anyway, all values are set again in the next lines by calling setTherapyTargetValue()
	 * with the default values before being used, so this should not cause any issue.
	 */
    for(int j = MAX_THERAPY_STEPS - 1; j >= 0; j--) //for(int j = 0; j < MAX_THERAPY_STEPS; j++)
	{
		setTherapyTargetValue(guiTherapyCtx.therapyTargetValue[j], guiTherapyCtx.defValue[j]);
	}

	printTherapyTargetValues(); // debug
}

/*
 * Main menu initialization
 */
void Model::setDefaultMainMenu(void)
{
	printf("Using default main menu configuration.\n");

	for (int i = 0; i < (MAX_DEV_THERAPIES + 1); i++) {
		deviceConfig[i] = deviceConfig_default[i];
	}
}

int16_t Model::configureMainMenu(void)
{
	int16_t ret = loadMainMenu(deviceConfig, MAX_DEV_THERAPIES);

/*  On error, use default configuration */
//	if( ret != 0)   // TODO:Consider remove...done at the beggining of this file in the tick function
//	{
//		setDefaultMainMenu();
//	}
	return ( (ret == 0) ? 1 : 0 );
}

void setMaxFlow(void)
{
	fsm_o3_setMaxAllowedFlow();

    if (modelInstance)
    	modelInstance->guiTherapyCtx.maxValue[ (fsm_o3_getOption() == SALINE_MODE) ? 2 : 1 ] = fsm_o3_getMaxAllowedFlow();
}

void setMaxTime(void)
{
	fsm_o3_setMaxAllowedTime();

    if (modelInstance)
    	modelInstance->guiTherapyCtx.maxValue[ (fsm_o3_getOption() == SALINE_MODE) ? 3 : 2 ] = fsm_o3_getMaxAllowedTime();
}

void Model::printTherapyTargetValues(void)
{
#if 1
	printf("[%lu ms] Therapy target values:\n", HAL_GetTick());
	printf("therapyTargetValues[TTV_CONCENTRATION]     (ConfiguredO3Concentration): %lu\n", fsm_o3_getTherapyParam(TTV_CONCENTRATION));
	printf("therapyTargetValues[TTV_FLOW]              (ConfiguredFlow): %lu\n", fsm_o3_getTherapyParam(TTV_FLOW));
	printf("therapyTargetValues[TTV_TIME]              (ConfiguredTime): %lu\n", fsm_o3_getTherapyParam(TTV_TIME));
	printf("therapyTargetValues[TTV_VOLUME]            (ConfiguredVolume): %lu\n", fsm_o3_getTherapyParam(TTV_VOLUME));
	printf("therapyTargetValues[TTV_DOSE] )            (ConfiguredDose): %lu\n", fsm_o3_getTherapyParam(TTV_DOSE));
	printf("therapyTargetValues[TTV_PRESSURE]          (ConfiguredPressure): %lu\n", fsm_o3_getTherapyParam(TTV_PRESSURE));
	printf("therapyTargetValues[TTV_VACUUM_TIME]       (ConfiguredVacuumTime): %lu\n", fsm_o3_getTherapyParam(TTV_VACUUM_TIME));
	printf("therapyTargetValues[TTV_VACUUM_PRESSURE]   (ConfiguredVacuumPressure): %lu\n", fsm_o3_getTherapyParam(TTV_VACUUM_PRESSURE));
	printf("therapyTargetValues[TTV_CALIBRATION_VAL_0] (CalibrationValue_0): %lu\n", fsm_o3_getTherapyParam(TTV_CALIBRATION_VAL_0));
	printf("therapyTargetValues[TTV_CALIBRATION_VAL_1] (CalibrationValue_1): %lu\n", fsm_o3_getTherapyParam(TTV_CALIBRATION_VAL_1));
	printf("therapyTargetValues[TTV_CALIBRATION_VAL_2] (CalibrationValue_2): %lu\n", fsm_o3_getTherapyParam(TTV_CALIBRATION_VAL_2));
	printf("                                           (PressThreshold): %d\n", fsm_o3_getPressThreshold());
#endif
}

/**
 * @brief Checks if the SD is present.
 * @retval 1 if present, 0 if not present
 */
uint8_t Model::is_SD(void)
{
	uint8_t status;

	if (HAL_GPIO_ReadPin(SD_DETECT_GPIO_Port, SD_DETECT_Pin) == GPIO_PIN_SET)
	{
		status = HAL_OK;
	}
	else
	{
		status = HAL_ERROR;
	}
	return status;
}

#ifdef USB_HOST_MODE
extern "C" uint8_t USBH_IsFlashReady(void);
#endif

/**
 * @brief Checks if a USB flash drive is connected and MSC class is active.
 * @retval 1 if ready, 0 if not
 */
uint8_t Model::is_USB_flash(void)
{
#ifdef USB_HOST_MODE
    return USBH_IsFlashReady();
#else
    return 0;
#endif
}

#include "fatfs.h"
#define NO_is_SD_mounted_verbose
/**
 * @brief Checks if the SD drive filesystem is mounted (drive "0:").
 * @retval 1 if mounted, 0 if not mounted
 */
uint8_t Model::is_SD_mounted(void)
{
    FATFS *fs;
    DWORD fre_clust;
#ifdef is_fs_mounted_verbose
    DWORD fre_sect, tot_sect;
#endif
    FRESULT res;

    // Try to get free space to check if the filesystem is mounted
    res = f_getfree("0:", &fre_clust, &fs);
    if (res == FR_OK)
    {
#ifdef is_fs_mounted_verbose
        printf("SD filesystem is mounted.\n");
        // Optionally, print some info about the filesystem
        tot_sect = (fs->n_fatent - 2) * fs->csize;
        fre_sect = fre_clust * fs->csize;
        printf("SD total sectors: %lu, Free sectors: %lu\n", tot_sect, fre_sect);
#endif
        return 1;
    }
    else
    {
#ifdef is_fs_mounted_verbose
        printf("SD filesystem is NOT mounted. f_getfree error: %d\n", res);
#endif
        return 0;
    }
}

#ifdef USB_HOST_MODE
#define NO_is_USB_flash_mounted_verbose
/**
 * @brief Checks if the USB flash drive filesystem is mounted (drive "1:").
 * @retval 1 if mounted, 0 if not mounted
 */
uint8_t Model::is_USB_flash_mounted(void)
{
    FATFS *fs;
    DWORD fre_clust;
#ifdef is_USB_flash_mounted_verbose
    DWORD fre_sect, tot_sect;
#endif
    FRESULT res;

    res = f_getfree("1:", &fre_clust, &fs);
    if (res == FR_OK)
    {
#ifdef is_USB_flash_mounted_verbose
        printf("USB filesystem is mounted.\n");
        tot_sect = (fs->n_fatent - 2) * fs->csize;
        fre_sect = fre_clust * fs->csize;
        printf("USB total sectors: %lu, Free sectors: %lu\n", tot_sect, fre_sect);
#endif
        return 1;
    }
    else
    {
#ifdef is_USB_flash_mounted_verbose
        printf("USB filesystem NOT mounted. f_getfree error: %d\n", res);
#endif
        return 0;
    }
}
#endif


//****************************************************************************************************************************************************************************************************

//
#if 0 //  functions from original demonstration application (start declaration)
//
#define STLM75ADR 0x90  // A0, A1 and A2 COnnected to VSS
bool CANHOST;

//  CAN Transmite with can2.0-8byte / fdcan-16byte a packet
void Model::SendCAN(uint8_t value)
{
	if (CANHOST)
	{
		touchgfx_printf("\rCAN Slidervalue to REMOTE =  %d\n\r",value);
	}
	else
	{
		touchgfx_printf("\rCAN Slidervalue to HOST =  %d\n\r",value);
	}

#ifndef SIMULATOR
	char TX_Buffer[30];
	sprintf((char *) TX_Buffer,"Val:%d",(int) value);
	n = (uint16_t) strlen (TX_Buffer);
	if (CANHOST)
	{
		EDT_CAN_Transmit(&CanHandle, CANBUS_REMOTEID, (uint8_t *)TX_Buffer, 8);
	}
	else
	{
		EDT_CAN_Transmit(&CanHandle, CANBUS_HOSTID, (uint8_t *)TX_Buffer, 8);
	}
#endif
}

void Model::SendRS232(uint8_t value)
{
	touchgfx_printf("\rRS232 Slidervalue = %d\n\r", value);

#ifndef SIMULATOR
	char TX_Buffer[30];
	sprintf((char *) TX_Buffer,"RS232 Slider Position = %d\n\r",(int) value);
	n = (uint16_t) strlen (TX_Buffer);
	touchgfx_printf("\r%s\n\r",TX_Buffer);

	EDT_UART_Transmit_IT(&hRs232, (uint8_t *)TX_Buffer, n);
#endif
}

void Model::SendRS485(uint8_t value)
{
	touchgfx_printf("\rRS485 Slidervalue = %d\n\r", value);

#ifndef SIMULATOR
	char TX_Buffer[30];
	EDT_LCD_BL_SetPwm(value);
	sprintf((char *) TX_Buffer,"Backlight PWM Value = %d\n\r",(int) value);
	n = (uint16_t) strlen (TX_Buffer);
	EDT_UART_Transmit_IT(&hRs485, (uint8_t *)TX_Buffer, n);
#endif
}

void Model::USBCDCSend(int value)
{
	touchgfx_printf("\rUSBCDC Slidervalue = %d\n\r", value);

#ifndef SIMULATOR
	sprintf((char *) USB_CDC_Tx_Buffer, "USB_CDC_TX: Slider Position = %d\n\r", value);
	n =  (uint16_t) strlen ((char *)USB_CDC_Tx_Buffer);
	CDC_Transmit_FS(USB_CDC_Tx_Buffer, n);
	//HAL_Delay(3);
#endif
}

float Model::ReadTemperatureSensor()
{
	touchgfx_printf("\rReading Temperature Sensor on I2C\n\r");
#ifndef SIMULATOR

	HAL_I2C_Master_Receive(&I2cHandle,STLM75ADR,Rx_Buffer,2,50); // Get Temperature

	raw = ((Rx_Buffer[0] << 8)  | Rx_Buffer[1]) >> 7;

	if (raw & 0x0100) {
		// Negative temperature
		intTemperature = -10 * (((~(uint8_t)(raw & 0x7E) + 1) & 0x7F) >> 1) - (raw & 0x01) * 5;
	} else {
		// Positive temperature
		intTemperature = ((raw & 0x7E) >> 1) * 10 + (raw & 0x01) * 5;
	}
	Temperature = (float) intTemperature;
	Temperature = Temperature / 10.0;
	/*
  sprintf((char *)Tx_Buffer, "\n\rPool Temperature = %.1f\n\r", Temperature);
  n =  (uint16_t) sizeof (Tx_Buffer);
  HAL_UART_Transmit_IT(&hRs232, Tx_Buffer, n);    // RS232 Non-Blocking
	 */
#endif

	modelListener->GetModelTemperature();
	return (Temperature);
}

void Model::SetGPIO4Active()
{
	touchgfx_printf("\rGPIO4 Set LOW\n\r");
#ifndef SIMULATOR
	EDT_GPIO_Write(GPIO4_GPIO_PORT,GPIO4_PIN, GPIO_PIN_RESET);   // Turn ON LED on testboard
#endif
	pulsecount = 0;
}

void Model::SetGPIO5(char value)
{
	GPIO5_Level = value;
	if (GPIO5_Level)
	{
		touchgfx_printf("\rGPIO5 Set HIGH\n\r");
	}
	else
	{
		touchgfx_printf("\rGPIO5 Set LOW\n\r");
	}

#ifndef SIMULATOR
	if (GPIO5_Level)
		EDT_GPIO_Write(GPIO5_GPIO_PORT,GPIO5_PIN, GPIO_PIN_SET);
	else
		EDT_GPIO_Write(GPIO5_GPIO_PORT,GPIO5_PIN, GPIO_PIN_RESET);
#endif
}

#endif //  functions from original demonstration application (end declaration)




#if 0

// TODO remove this commented code when config by means of therapy templates be well tested and validated
void Model::setupTherapyContextBack(int8_t therapyID)
{
//	printf("%s(). Init therapy context for therapy code %d\n", __func__, therapyID);

	for(int8_t i = 0; i < MAX_THERAPY_STEPS; i++)
	{

		// TODO: it is thre othre ponter to function  	REVIEW AND CORRECT ASAP

		guiTherapyCtx.okAction[i] = fsm_o3_no_op;       // to avoid null pointer dereference
		guiTherapyCtx.sliderAction[i] = fsm_o3_no_op;   // to avoid null pointer dereference
		guiTherapyCtx.selectAvailable[i] = true;         // default visible
		guiTherapyCtx.delayIndicatorTime[i] = 0;         // default invisible
		guiTherapyCtx.secondSelectionVisible[i] = false; // default invisible
	}

	switch(therapyID)
	{
	case SYRINGE_MODE:
	case SYRINGE_AUTO_MODE:
		guiTherapyCtx.stepsNum = 1;
		guiTherapyCtx.mainIcon = ( therapyID == SYRINGE_MODE ) ? BITMAP_JERINGA_01_ID : BITMAP_JERINGAAUTO_01_ID;
		guiTherapyCtx.icon[0] = BITMAP_O3_00_ID;
		guiTherapyCtx.minValue[0] = 0;
		guiTherapyCtx.defValue[0] = 1;
		guiTherapyCtx.maxValue[0] = 80;
		guiTherapyCtx.step[0] = 1;
		guiTherapyCtx.units[0] = "ug/Nml";
		guiTherapyCtx.therapyTargetValue[0] = TTV_CONCENTRATION;
		guiTherapyCtx.endTotalDoseVisible = true;
		guiTherapyCtx.endTotalVolumeVisible = true;
		guiTherapyCtx.endTotalTimeVisible = false;
		guiTherapyCtx.okButtonVisible = false;
		guiTherapyCtx.bigPauseButtonVisible = false;

		fsm_o3_setTherapyParam(TTV_TIME, 1); // default time value for this therapy TODO consider moving outside the graphic resources....
		break;
	case SYRINGE_MANUAL_MODE:
		guiTherapyCtx.stepsNum = 1;
		guiTherapyCtx.mainIcon = BITMAP_JERINGAMANUAL_01_ID;
		guiTherapyCtx.icon[0] = BITMAP_O3_00_ID;
		guiTherapyCtx.minValue[0] = 0;
		guiTherapyCtx.defValue[0] = 1;
		guiTherapyCtx.maxValue[0] = 80;
		guiTherapyCtx.step[0] = 1;
		guiTherapyCtx.units[0] = "ug/Nml";
		guiTherapyCtx.therapyTargetValue[0] = TTV_CONCENTRATION;
		guiTherapyCtx.endTotalDoseVisible = true;
		guiTherapyCtx.endTotalVolumeVisible = false;
		guiTherapyCtx.endTotalTimeVisible = false;
		guiTherapyCtx.okButtonVisible = true;
		guiTherapyCtx.bigPauseButtonVisible = false;

		fsm_o3_setTherapyParam(TTV_TIME, 10); // default time value for this therapy
		break;
	case CONTINUOUS_MODE:

		guiTherapyCtx.stepsNum = 3;
		guiTherapyCtx.mainIcon = BITMAP_CONTINUO_01_ID;
		guiTherapyCtx.icon[0] = BITMAP_O3_00_ID;
		guiTherapyCtx.icon[1] = BITMAP_FLOW_00_ID;
		guiTherapyCtx.icon[2] = BITMAP_TIME_00_ID;
		guiTherapyCtx.minValue[0] = 3;
		guiTherapyCtx.defValue[0] = 40;//160;
		guiTherapyCtx.maxValue[0] = 80;
		guiTherapyCtx.step[0] = 10;
		guiTherapyCtx.minValue[1] = 10;
		guiTherapyCtx.defValue[1] = 20;//3100;
		guiTherapyCtx.maxValue[1] = 50;
		guiTherapyCtx.step[1] = 1;
		guiTherapyCtx.minValue[2] = 1;
		guiTherapyCtx.defValue[2] = 2;//57000;
		guiTherapyCtx.maxValue[2] = 30;
		guiTherapyCtx.step[2] = 1;
		guiTherapyCtx.units[0] = "ug/Nml";
		guiTherapyCtx.units[1] = "l/h";
		guiTherapyCtx.units[2] = "min";
		guiTherapyCtx.therapyTargetValue[0] = TTV_CONCENTRATION;
		guiTherapyCtx.therapyTargetValue[1] = TTV_FLOW;
		guiTherapyCtx.therapyTargetValue[2] = TTV_TIME;
		guiTherapyCtx.endTotalDoseVisible = true;
		guiTherapyCtx.endTotalVolumeVisible = true;
		guiTherapyCtx.endTotalTimeVisible = true;
		guiTherapyCtx.okButtonVisible = false;
		guiTherapyCtx.bigPauseButtonVisible = false;
/*
		print_guiTherapyCtx(&guiTherapyCtx);
		loadTherapyFromFile(CONTINUOUS_MODE);
		*/

		print_guiTherapyCtx(&guiTherapyCtx);
		printf("nueva ini...............................................................\n");
		setupDefaultTherapyContext(therapyID);

		print_guiTherapyCtx(&guiTherapyCtx);
		break;
	case INSUFFLATION_MODE:
	case INSUFFLATION_R_MODE:
		guiTherapyCtx.stepsNum = 3;
		guiTherapyCtx.mainIcon = ( therapyID == INSUFFLATION_MODE ) ? BITMAP_INSUFLACION_01_ID : BITMAP_RECTALINSUFFLATION_01_ID;
		guiTherapyCtx.icon[0] = BITMAP_O3_00_ID;
		guiTherapyCtx.icon[1] = BITMAP_FLOW_00_ID;
		guiTherapyCtx.icon[2] = BITMAP_VOLUME_00_ID;
		guiTherapyCtx.minValue[0] = 1;
		guiTherapyCtx.defValue[0] = 2;
		guiTherapyCtx.maxValue[0] = 80;
		guiTherapyCtx.step[0] = 1;
		guiTherapyCtx.minValue[1] = 1;
		guiTherapyCtx.defValue[1] = 30;
		guiTherapyCtx.maxValue[1] = 50;
		guiTherapyCtx.step[1] = 1;
		guiTherapyCtx.minValue[2] = 100;
		guiTherapyCtx.defValue[2] = 1500;
		guiTherapyCtx.maxValue[2] = 2000;
		guiTherapyCtx.step[2] = 10;
		guiTherapyCtx.units[0] = "ug/Nml";
		guiTherapyCtx.units[1] = "l/h";
		guiTherapyCtx.units[2] = "ml";
		guiTherapyCtx.therapyTargetValue[0] = TTV_CONCENTRATION;
		guiTherapyCtx.therapyTargetValue[1] = TTV_FLOW;
		guiTherapyCtx.therapyTargetValue[2] = TTV_VOLUME;
		guiTherapyCtx.secondSelectionVisible[2] = true;
		guiTherapyCtx.endTotalDoseVisible = true;
		guiTherapyCtx.endTotalVolumeVisible = true;
		guiTherapyCtx.endTotalTimeVisible = true;
		guiTherapyCtx.okButtonVisible = true;
		guiTherapyCtx.bigPauseButtonVisible = true;
		break;
	case INSUFFLATION_V_MODE:
		guiTherapyCtx.stepsNum = 3;
		guiTherapyCtx.mainIcon = BITMAP_VAGINALINSUFFLATION_01_ID;
		guiTherapyCtx.icon[0] = BITMAP_O3_00_ID;
		guiTherapyCtx.icon[1] = BITMAP_FLOW_00_ID;
		guiTherapyCtx.icon[2] = BITMAP_TIME_00_ID;
		guiTherapyCtx.minValue[0] = 1;
		guiTherapyCtx.defValue[0] = 2;
		guiTherapyCtx.maxValue[0] = 80;
		guiTherapyCtx.step[0] = 1;
		guiTherapyCtx.minValue[1] = 1;
		guiTherapyCtx.defValue[1] = 30;
		guiTherapyCtx.maxValue[1] = 50;
		guiTherapyCtx.step[1] = 1;
		guiTherapyCtx.minValue[2] = 1;
		guiTherapyCtx.defValue[2] = 3;
		guiTherapyCtx.maxValue[2] = 30;
		guiTherapyCtx.step[2] = 10;
		guiTherapyCtx.units[0] = "ug/Nml";
		guiTherapyCtx.units[1] = "l/h";
		guiTherapyCtx.units[2] = "min";
		guiTherapyCtx.therapyTargetValue[0] = TTV_CONCENTRATION;
		guiTherapyCtx.therapyTargetValue[1] = TTV_FLOW;
		guiTherapyCtx.therapyTargetValue[2] = TTV_TIME;
		guiTherapyCtx.endTotalDoseVisible = true;
		guiTherapyCtx.endTotalVolumeVisible = true;
		guiTherapyCtx.endTotalTimeVisible = true;
		guiTherapyCtx.okButtonVisible = true;
		guiTherapyCtx.bigPauseButtonVisible = true;
		break;
	case MANUAL_MODE:
		guiTherapyCtx.stepsNum = 2;
		guiTherapyCtx.mainIcon = BITMAP_MANUAL_01_ID;
		guiTherapyCtx.icon[0] = BITMAP_O3_00_ID;
		guiTherapyCtx.icon[1] = BITMAP_FLOW_00_ID;
		guiTherapyCtx.icon[2] = BITMAP_TIME_00_ID;
		guiTherapyCtx.minValue[0] = 1;
		guiTherapyCtx.defValue[0] = 2;
		guiTherapyCtx.maxValue[0] = 80;
		guiTherapyCtx.step[0] = 1;
		guiTherapyCtx.minValue[1] = 1;
		guiTherapyCtx.defValue[1] = 31;
		guiTherapyCtx.maxValue[1] = 50;
		guiTherapyCtx.step[1] = 1;
		guiTherapyCtx.minValue[2] = 100;
		guiTherapyCtx.defValue[2] = 1500;
		guiTherapyCtx.maxValue[2] = 2000;
		guiTherapyCtx.step[2] = 10;
		guiTherapyCtx.units[0] = "ug/Nml";
		guiTherapyCtx.units[1] = "l/h";
		guiTherapyCtx.units[2] = "";
		guiTherapyCtx.therapyTargetValue[0] = TTV_CONCENTRATION;
		guiTherapyCtx.therapyTargetValue[1] = TTV_FLOW;
		guiTherapyCtx.therapyTargetValue[2] = TTV_VOLUME;
		guiTherapyCtx.endTotalDoseVisible = true;
		guiTherapyCtx.endTotalVolumeVisible = true;
		guiTherapyCtx.endTotalTimeVisible = true;
		guiTherapyCtx.okButtonVisible = true;
		guiTherapyCtx.bigPauseButtonVisible = false;

		fsm_o3_setTherapyParam(TTV_TIME, 10); // default time value for this therapy
		break;
	case DENTAL_MODE:
		guiTherapyCtx.stepsNum = 3;
		guiTherapyCtx.mainIcon = BITMAP_GUN_01_ID;
		guiTherapyCtx.icon[0] = BITMAP_O3_00_ID;
		guiTherapyCtx.icon[1] = BITMAP_FLOW_00_ID;
		guiTherapyCtx.icon[2] = BITMAP_OVERPRESSURE_00_ID;
		guiTherapyCtx.minValue[0] = 1;
		guiTherapyCtx.defValue[0] = 2;
		guiTherapyCtx.maxValue[0] = 80;
		guiTherapyCtx.step[0] = 1;
		guiTherapyCtx.minValue[1] = 1;
		guiTherapyCtx.defValue[1] = 31;
		guiTherapyCtx.maxValue[1] = 50;
		guiTherapyCtx.step[1] = 1;
		guiTherapyCtx.minValue[2] = 1;
		guiTherapyCtx.defValue[2] = 701;
		guiTherapyCtx.maxValue[2] = 800;
		guiTherapyCtx.step[2] = 1;
		guiTherapyCtx.units[0] = "ug/Nml";
		guiTherapyCtx.units[1] = "l/h";
		guiTherapyCtx.units[2] = "mbar";
		guiTherapyCtx.therapyTargetValue[0] = TTV_CONCENTRATION;
		guiTherapyCtx.therapyTargetValue[1] = TTV_FLOW;
		guiTherapyCtx.therapyTargetValue[2] = TTV_PRESSURE;
		guiTherapyCtx.endTotalDoseVisible = true;
		guiTherapyCtx.endTotalVolumeVisible = true;
		guiTherapyCtx.endTotalTimeVisible = true;
		guiTherapyCtx.okButtonVisible = false;
		guiTherapyCtx.bigPauseButtonVisible = false;

		fsm_o3_setTherapyParam(TTV_TIME, 1); // default time value for this therapy TODO consider moving outside the graphic resources....
		break;
	case DOSE_MODE:
		guiTherapyCtx.stepsNum = 2;
		guiTherapyCtx.mainIcon = BITMAP_DOSE_01_ID;
		guiTherapyCtx.icon[0] = BITMAP_O3_00_ID;
		guiTherapyCtx.icon[1] = BITMAP_DOSE_01_ID;
		guiTherapyCtx.icon[2] = BITMAP_TIME_00_ID;
		guiTherapyCtx.minValue[0] = 1;
		guiTherapyCtx.defValue[0] = 2;
		guiTherapyCtx.maxValue[0] = 80;
		guiTherapyCtx.step[0] = 1;
		guiTherapyCtx.minValue[1] = 1;
		guiTherapyCtx.defValue[1] = 2000;
		guiTherapyCtx.maxValue[1] = 5000;
		guiTherapyCtx.step[1] = 1;
		guiTherapyCtx.minValue[2] = 100;
		guiTherapyCtx.defValue[2] = 1500;
		guiTherapyCtx.maxValue[2] = 2000;
		guiTherapyCtx.step[2] = 10;
		guiTherapyCtx.units[0] = "ug/Nml";
		guiTherapyCtx.units[1] = "ug";
		guiTherapyCtx.units[2] = "";
		guiTherapyCtx.therapyTargetValue[0] = TTV_CONCENTRATION;
		guiTherapyCtx.therapyTargetValue[1] = TTV_DOSE;
		guiTherapyCtx.therapyTargetValue[2] = TTV_VOLUME;
		guiTherapyCtx.endTotalDoseVisible = true;
		guiTherapyCtx.endTotalVolumeVisible = true;
		guiTherapyCtx.endTotalTimeVisible = true;
		guiTherapyCtx.okButtonVisible = false;
		guiTherapyCtx.bigPauseButtonVisible = false;
		break;
	case VACUUM_MODE:
	case VACUUM_TIME_MODE:
		guiTherapyCtx.stepsNum = 1;
		guiTherapyCtx.mainIcon = ( therapyID == VACUUM_MODE ) ? BITMAP_VACUUM_01_ID : BITMAP_VACUUM_T_01_ID;
		guiTherapyCtx.icon[0] = BITMAP_TIME_00_ID;
		guiTherapyCtx.icon[1] = BITMAP_FLOW_00_ID;
		guiTherapyCtx.icon[2] = BITMAP_TIME_00_ID;
		guiTherapyCtx.minValue[0] = 1;
		guiTherapyCtx.defValue[0] = 3;
		guiTherapyCtx.maxValue[0] = 120;
		guiTherapyCtx.step[0] = 1;
		guiTherapyCtx.minValue[1] = 1;
		guiTherapyCtx.defValue[1] = 2000;
		guiTherapyCtx.maxValue[1] = 5000;
		guiTherapyCtx.step[1] = 1;
		guiTherapyCtx.minValue[2] = 100;
		guiTherapyCtx.defValue[2] = 1500;
		guiTherapyCtx.maxValue[2] = 2000;
		guiTherapyCtx.step[2] = 10;
		guiTherapyCtx.units[0] = "secs";
		guiTherapyCtx.units[1] = "";
		guiTherapyCtx.units[2] = "";
		guiTherapyCtx.therapyTargetValue[0] = TTV_VACUUM_TIME;
		guiTherapyCtx.therapyTargetValue[1] = TTV_DOSE;
		guiTherapyCtx.therapyTargetValue[2] = TTV_VOLUME;
		guiTherapyCtx.endTotalDoseVisible = true;
		guiTherapyCtx.endTotalVolumeVisible = true;
		guiTherapyCtx.endTotalTimeVisible = true;
		guiTherapyCtx.okButtonVisible = true;
		guiTherapyCtx.bigPauseButtonVisible = false;
		break;
	case VACUUM_PRESSURE_MODE:
		guiTherapyCtx.stepsNum = 1;
		guiTherapyCtx.mainIcon = BITMAP_VACUUM_P_01_ID;
		guiTherapyCtx.icon[0] = BITMAP_OVERPRESSURE_00_ID;
		guiTherapyCtx.icon[1] = BITMAP_FLOW_00_ID;
		guiTherapyCtx.icon[2] = BITMAP_TIME_00_ID;
		guiTherapyCtx.minValue[0] = 1;
		guiTherapyCtx.defValue[0] = 970;
		guiTherapyCtx.maxValue[0] = 120;
		guiTherapyCtx.step[0] = 1;
		guiTherapyCtx.minValue[1] = 1;
		guiTherapyCtx.defValue[1] = 2000;
		guiTherapyCtx.maxValue[1] = 5000;
		guiTherapyCtx.step[1] = 1;
		guiTherapyCtx.minValue[2] = 100;
		guiTherapyCtx.defValue[2] = 1500;
		guiTherapyCtx.maxValue[2] = 2000;
		guiTherapyCtx.step[2] = 10;
		guiTherapyCtx.units[0] = "mbar";
		guiTherapyCtx.units[1] = "";
		guiTherapyCtx.units[2] = "";
		guiTherapyCtx.therapyTargetValue[0] = TTV_VACUUM_PRESSURE;
		guiTherapyCtx.therapyTargetValue[1] = TTV_DOSE;
		guiTherapyCtx.therapyTargetValue[2] = TTV_VOLUME;
		guiTherapyCtx.endTotalDoseVisible = true;
		guiTherapyCtx.endTotalVolumeVisible = true;
		guiTherapyCtx.endTotalTimeVisible = true;
		guiTherapyCtx.okButtonVisible = true;
		guiTherapyCtx.bigPauseButtonVisible = false;
		break;
	case BAG_MODE:
	case CLOSED_BAG_MODE:
		guiTherapyCtx.stepsNum = 4;
		guiTherapyCtx.mainIcon = ( therapyID == BAG_MODE ) ? BITMAP_BAG_01_ID : BITMAP_CLOSEDBAG_01_ID;
		guiTherapyCtx.icon[0] = BITMAP_VACUUM_T_01_ID;
		guiTherapyCtx.icon[1] = BITMAP_O3_00_ID;
		guiTherapyCtx.icon[2] = BITMAP_VOLUME_00_ID;
		guiTherapyCtx.icon[3] = BITMAP_TIME_00_ID;
		guiTherapyCtx.minValue[0] = 1;
		guiTherapyCtx.defValue[0] = 10;
		guiTherapyCtx.maxValue[0] = 30;
		guiTherapyCtx.step[0] = 1;
		guiTherapyCtx.minValue[1] = 1;
		guiTherapyCtx.defValue[1] = 5;
		guiTherapyCtx.maxValue[1] = 80;
		guiTherapyCtx.step[1] = 1;
		guiTherapyCtx.minValue[2] = 1;
		guiTherapyCtx.defValue[2] = 2;
		guiTherapyCtx.maxValue[2] = 10;
		guiTherapyCtx.step[2] = 1;
		guiTherapyCtx.minValue[3] = 1;
		guiTherapyCtx.defValue[3] = 2;
		guiTherapyCtx.maxValue[3] = 20;
		guiTherapyCtx.step[3] = 1;
		guiTherapyCtx.units[0] = "secs";
		guiTherapyCtx.units[1] = "ug/Nml";
		guiTherapyCtx.units[2] = "l";
		guiTherapyCtx.units[3] = "min";
		guiTherapyCtx.therapyTargetValue[0] = TTV_VACUUM_TIME;
		guiTherapyCtx.therapyTargetValue[1] = TTV_CONCENTRATION;
		guiTherapyCtx.therapyTargetValue[2] = TTV_VOLUME;
		guiTherapyCtx.therapyTargetValue[3] = TTV_TIME;
		guiTherapyCtx.endTotalDoseVisible = true;
		guiTherapyCtx.endTotalVolumeVisible = true;
		guiTherapyCtx.endTotalTimeVisible = true;
		guiTherapyCtx.okButtonVisible = true;
		guiTherapyCtx.bigPauseButtonVisible = false;
		break;
	case OPEN_BAG_MODE:
		guiTherapyCtx.stepsNum = 4;
		guiTherapyCtx.mainIcon = BITMAP_OPENBAG_01_ID;
		guiTherapyCtx.icon[0] = BITMAP_O3_00_ID;
		guiTherapyCtx.icon[1] = BITMAP_FLOW_00_ID;
		guiTherapyCtx.icon[2] = BITMAP_TIME_00_ID;
		guiTherapyCtx.icon[3] = BITMAP_VACUUM_T_01_ID;
		guiTherapyCtx.minValue[0] = 0;
		guiTherapyCtx.defValue[0] = 10;//160;
		guiTherapyCtx.maxValue[0] = 900;
		guiTherapyCtx.step[0] = 1;
		guiTherapyCtx.minValue[1] = 10;
		guiTherapyCtx.defValue[1] = 40;//3100;
		guiTherapyCtx.maxValue[1] = 9000;
		guiTherapyCtx.step[1] = 1;
		guiTherapyCtx.minValue[2] = 1;
		guiTherapyCtx.defValue[2] = 2;//57000;
		guiTherapyCtx.maxValue[2] = 90000;
		guiTherapyCtx.step[2] = 1;
		guiTherapyCtx.minValue[3] = 1;
		guiTherapyCtx.defValue[3] = 10;
		guiTherapyCtx.maxValue[3] = 30;
		guiTherapyCtx.step[3] = 1;
		guiTherapyCtx.units[0] = "ug/Nml";
		guiTherapyCtx.units[1] = "l/h";
		guiTherapyCtx.units[2] = "min";
		guiTherapyCtx.units[3] = "secs";
		guiTherapyCtx.therapyTargetValue[0] = TTV_CONCENTRATION;
		guiTherapyCtx.therapyTargetValue[1] = TTV_FLOW;
		guiTherapyCtx.therapyTargetValue[2] = TTV_TIME;
		guiTherapyCtx.therapyTargetValue[3] = TTV_VACUUM_TIME;
		guiTherapyCtx.endTotalDoseVisible = true;
		guiTherapyCtx.endTotalVolumeVisible = true;
		guiTherapyCtx.endTotalTimeVisible = true;
		guiTherapyCtx.okButtonVisible = true;
		guiTherapyCtx.bigPauseButtonVisible = false;
		break;
	case SALINE_MODE:
		guiTherapyCtx.stepsNum = 4;
		guiTherapyCtx.mainIcon = BITMAP_SALINE_01_ID;
		guiTherapyCtx.icon[0] = BITMAP_VOLUME_00_ID;
		guiTherapyCtx.icon[1] = BITMAP_O3_00_ID;
		guiTherapyCtx.icon[2] = BITMAP_FLOW_00_ID;
		guiTherapyCtx.icon[3] = BITMAP_TIME_00_ID;
		guiTherapyCtx.minValue[0] = 1;
		guiTherapyCtx.defValue[0] = 2000;
		guiTherapyCtx.maxValue[0] = 10000;
		guiTherapyCtx.step[0] = 100;
		guiTherapyCtx.minValue[1] = 1;
		guiTherapyCtx.defValue[1] = 2;
		guiTherapyCtx.maxValue[1] = 80;
		guiTherapyCtx.step[1] = 1;
		guiTherapyCtx.minValue[2] = 1;
		guiTherapyCtx.defValue[2] = 20;
		guiTherapyCtx.maxValue[2] = 50;
		guiTherapyCtx.step[2] = 1;
		guiTherapyCtx.minValue[3] = 1;
		guiTherapyCtx.defValue[3] = 2;
		guiTherapyCtx.maxValue[3] = 30;
		guiTherapyCtx.step[3] = 1;
		guiTherapyCtx.units[0] = "ml";
		guiTherapyCtx.units[1] = "ug/Nml";
		guiTherapyCtx.units[2] = "l/h";
		guiTherapyCtx.units[3] = "min";
		guiTherapyCtx.therapyTargetValue[0] = TTV_VOLUME;///??????????????????????
		guiTherapyCtx.therapyTargetValue[1] = TTV_CONCENTRATION;
		guiTherapyCtx.therapyTargetValue[2] = TTV_FLOW;
		guiTherapyCtx.therapyTargetValue[3] = TTV_TIME;
		guiTherapyCtx.secondSelectionVisible[2] = true;
		guiTherapyCtx.endTotalDoseVisible = true;
		guiTherapyCtx.endTotalVolumeVisible = true;
		guiTherapyCtx.endTotalTimeVisible = true;
		guiTherapyCtx.okButtonVisible = false;
		guiTherapyCtx.bigPauseButtonVisible = false;
		break;
	case CAL_PRESS_MODE:
		guiTherapyCtx.stepsNum = 2;
		guiTherapyCtx.mainIcon = BITMAP_SETTINGS_100_A_ID;
		guiTherapyCtx.icon[0] = BITMAP_OVERPRESSURE_00_ID;
		guiTherapyCtx.icon[1] = BITMAP_OVERPRESSURE_00_ID;
		guiTherapyCtx.minValue[0] = 800;
		guiTherapyCtx.defValue[0] = 940;
		guiTherapyCtx.maxValue[0] = 1100;
		guiTherapyCtx.step[0] = 1;
		guiTherapyCtx.minValue[1] = 1;
		guiTherapyCtx.defValue[1] = 2000;
		guiTherapyCtx.maxValue[1] = 5000;
		guiTherapyCtx.step[1] = 10;
		guiTherapyCtx.units[0] = "P.Atm (mbar)";
		guiTherapyCtx.units[1] = "500 mbar";
		guiTherapyCtx.selectAvailable[0] = true;
		guiTherapyCtx.selectAvailable[1] = false;
		guiTherapyCtx.therapyTargetValue[0] = TTV_CALIBRATION_VAL_0;
		guiTherapyCtx.therapyTargetValue[1] = TTV_CALIBRATION_VAL_1;
		guiTherapyCtx.endTotalDoseVisible = true;   //TODO: Should be false????????????????????????????????????????
		guiTherapyCtx.endTotalVolumeVisible = true;
		guiTherapyCtx.endTotalTimeVisible = true;
		guiTherapyCtx.okButtonVisible = false;
		guiTherapyCtx.bigPauseButtonVisible = false;
		guiTherapyCtx.okAction[0] = fsm_o3_calibratePressureStep1;
		guiTherapyCtx.okAction[1] = fsm_o3_calibratePressureStep2;
		break;
	case CAL_FLOW_MODE:
		guiTherapyCtx.stepsNum = 2;
		guiTherapyCtx.mainIcon = BITMAP_SETTINGS_100_A_ID;
		guiTherapyCtx.icon[0] = BITMAP_FLOW_00_ID;
		guiTherapyCtx.icon[1] = BITMAP_FLOW_00_ID;
		guiTherapyCtx.minValue[0] = 500;
		guiTherapyCtx.defValue[0] = 700;
		guiTherapyCtx.maxValue[0] = 1100;
		guiTherapyCtx.step[0] = 1;
		guiTherapyCtx.minValue[1] = 450;
		guiTherapyCtx.defValue[1] = 500;
		guiTherapyCtx.maxValue[1] = 550;
		guiTherapyCtx.step[1] = 1;
		guiTherapyCtx.units[0] = "Fix 30 l/h";
		guiTherapyCtx.units[1] = "External read (l/m)";
		guiTherapyCtx.therapyTargetValue[0] = TTV_CALIBRATION_VAL_0;
		guiTherapyCtx.therapyTargetValue[1] = TTV_CALIBRATION_VAL_1;
		guiTherapyCtx.endTotalDoseVisible = true;
		guiTherapyCtx.endTotalVolumeVisible = true;
		guiTherapyCtx.endTotalTimeVisible = true;
		guiTherapyCtx.okButtonVisible = false;
		guiTherapyCtx.bigPauseButtonVisible = false;
		guiTherapyCtx.okAction[0] = fsm_o3_calibrateFlowStep1Ok;
		guiTherapyCtx.okAction[1] = fsm_o3_calibrateFlowStep2;
		guiTherapyCtx.sliderAction[0] = fsm_o3_calibrateFlowStep1Value;
		break;
	case CAL_O3_MODE:
		if(GLB_fsm_o3.DeviceType == DEV_OZT)
		{
			guiTherapyCtx.stepsNum = 2;
			guiTherapyCtx.mainIcon = BITMAP_SETTINGS_100_A_ID;
			guiTherapyCtx.icon[0] = BITMAP_O3_00_ID;
			guiTherapyCtx.icon[1] = BITMAP_O3_00_ID;
			guiTherapyCtx.icon[2] = BITMAP_O3_00_ID;
			guiTherapyCtx.minValue[0] = 1000;
			guiTherapyCtx.defValue[0] = 1500;
			guiTherapyCtx.maxValue[0] = 1800;
			guiTherapyCtx.step[0] = 1;
			guiTherapyCtx.minValue[1] = 1;
			guiTherapyCtx.defValue[1] = 40;
			guiTherapyCtx.maxValue[1] = 200;
			guiTherapyCtx.step[1] = 1;
			guiTherapyCtx.minValue[2] = 100;
			guiTherapyCtx.defValue[2] = 350;
			guiTherapyCtx.maxValue[2] = 900;
			guiTherapyCtx.step[2] = 1;
			guiTherapyCtx.units[0] = "60 ug/Nml";
			guiTherapyCtx.units[1] = "15 ug/Nml";
			guiTherapyCtx.units[2] = "1 ug/Nml";
			guiTherapyCtx.therapyTargetValue[0] = TTV_CALIBRATION_VAL_0;
			guiTherapyCtx.therapyTargetValue[1] = TTV_CALIBRATION_VAL_1;
			guiTherapyCtx.therapyTargetValue[2] = TTV_CALIBRATION_VAL_2;
			guiTherapyCtx.endTotalDoseVisible = false;
			guiTherapyCtx.endTotalVolumeVisible = false;
			guiTherapyCtx.endTotalTimeVisible = false;
			guiTherapyCtx.okButtonVisible = false;
			guiTherapyCtx.bigPauseButtonVisible = false;
			guiTherapyCtx.okAction[0] = CalibrateO3Step1_Ok;
			guiTherapyCtx.okAction[1] = CalibrateO3Step2_Ok;
			guiTherapyCtx.okAction[2] = CalibrateO3Step3;
			guiTherapyCtx.sliderAction[0] = CalibrateO3Step1_Value;
			guiTherapyCtx.sliderAction[1] = CalibrateO3Step2_Value;
		}
		else if(GLB_fsm_o3.DeviceType == DEV_OZP)
		{
			guiTherapyCtx.stepsNum = 5;
			guiTherapyCtx.mainIcon = BITMAP_SETTINGS_100_A_ID;
			guiTherapyCtx.icon[0] = BITMAP_O3_00_ID;
			guiTherapyCtx.icon[1] = BITMAP_O3_00_ID;
			guiTherapyCtx.icon[2] = BITMAP_O3_00_ID;
			guiTherapyCtx.icon[3] = BITMAP_O3_00_ID;
			guiTherapyCtx.icon[4] = BITMAP_O3_00_ID;
			guiTherapyCtx.minValue[0] = 1;
			guiTherapyCtx.defValue[0] = 2;
			guiTherapyCtx.maxValue[0] = 3;
			guiTherapyCtx.step[0] = 1;
			guiTherapyCtx.minValue[1] = 1;
			guiTherapyCtx.defValue[1] = 250;
			guiTherapyCtx.maxValue[1] = 1023;
			guiTherapyCtx.step[1] = 1;
			guiTherapyCtx.minValue[2] = 1;
			guiTherapyCtx.defValue[2] = 2;
			guiTherapyCtx.maxValue[2] = 3;
			guiTherapyCtx.step[2] = 1;
			guiTherapyCtx.minValue[3] = 1;
			guiTherapyCtx.defValue[3] = 50;
			guiTherapyCtx.maxValue[3] = 1023;
			guiTherapyCtx.step[3] = 1;
			guiTherapyCtx.minValue[4] = 1;
			guiTherapyCtx.defValue[4] = 15;
			guiTherapyCtx.maxValue[4] = 1023;
			guiTherapyCtx.step[4] = 1;
			guiTherapyCtx.units[0] = "60 ug/Nml";
			guiTherapyCtx.units[1] = "60 ug/Nml";
			guiTherapyCtx.units[2] = "15 ug/Nml";
			guiTherapyCtx.units[3] = "15 ug/Nml";
			guiTherapyCtx.units[4] = "1 ug/Nml";
			guiTherapyCtx.therapyTargetValue[0] = TTV_CALIBRATION_VAL_0;
			guiTherapyCtx.therapyTargetValue[1] = TTV_CALIBRATION_VAL_0;
			guiTherapyCtx.therapyTargetValue[2] = TTV_CALIBRATION_VAL_1;
			guiTherapyCtx.therapyTargetValue[3] = TTV_CALIBRATION_VAL_1;
			guiTherapyCtx.therapyTargetValue[4] = TTV_CALIBRATION_VAL_2;
			guiTherapyCtx.endTotalDoseVisible = false;
			guiTherapyCtx.endTotalVolumeVisible = false;
			guiTherapyCtx.endTotalTimeVisible = false;
			guiTherapyCtx.okButtonVisible = false;
			guiTherapyCtx.bigPauseButtonVisible = false;
			guiTherapyCtx.selectAvailable[0] = false;
			guiTherapyCtx.selectAvailable[2] = false;
			guiTherapyCtx.delayIndicatorTime[0] = 107000; // one minute and 48 seconds
			guiTherapyCtx.delayIndicatorTime[2] = 12000;
			guiTherapyCtx.okAction[1] = CalibrateO3Step1_Ok;
			guiTherapyCtx.okAction[3] = CalibrateO3Step2_Ok;
			guiTherapyCtx.okAction[4] = CalibrateO3Step3;
			guiTherapyCtx.sliderAction[1] = CalibrateO3Step1_Value;
			guiTherapyCtx.sliderAction[3] = CalibrateO3Step2_Value;
			guiTherapyCtx.sliderAction[4] = CalibrateO3Step3_Value;
		}
		break;
	case CAL_PFACTOR_MODE:
		UpdateInitialPfactor(); // walk around to get the last saved P factor value
		guiTherapyCtx.stepsNum = 2;
		guiTherapyCtx.mainIcon = BITMAP_SETTINGS_100_A_ID;
		guiTherapyCtx.icon[0] = BITMAP_JERINGA_01_ID;
		guiTherapyCtx.icon[1] = BITMAP_JERINGA_01_ID;
		guiTherapyCtx.minValue[0] = 1;
		guiTherapyCtx.defValue[0] = 2;
		guiTherapyCtx.maxValue[0] = 3;
		guiTherapyCtx.step[0] = 1;
		guiTherapyCtx.minValue[1] = 1;
		guiTherapyCtx.defValue[1] = fsm_o3_getTherapyParam(TTV_CALIBRATION_VAL_2); // walk around to get the last saved P factor value
		guiTherapyCtx.maxValue[1] = 64;
		guiTherapyCtx.step[1] = 1;
		guiTherapyCtx.units[0] = "P Factor";
		guiTherapyCtx.units[1] = "P Factor";
		guiTherapyCtx.selectAvailable[0] = false;
		guiTherapyCtx.selectAvailable[1] = true;
		guiTherapyCtx.delayIndicatorTime[0] = 10000;
		guiTherapyCtx.therapyTargetValue[0] = TTV_CALIBRATION_VAL_0;
		guiTherapyCtx.therapyTargetValue[1] = TTV_CALIBRATION_VAL_0;
		guiTherapyCtx.endTotalDoseVisible = true;
		guiTherapyCtx.endTotalVolumeVisible = true;
		guiTherapyCtx.endTotalTimeVisible = true;
		guiTherapyCtx.okButtonVisible = false;
		guiTherapyCtx.bigPauseButtonVisible = false;
		guiTherapyCtx.okAction[1] = CalibratePfactor_Ok;
		guiTherapyCtx.sliderAction[1] = CalibratePfactor_Value;


		break;
	}
}
#endif

/* ---------------------------------------------------------------------------
 * ConfigLoaderTask — runs once at startup in its own FreeRTOS task.
 *
 * Phase 1: USB detection window (500 ms – 1500 ms).
 *          If a USB flash is found, imports service config onto the SD card.
 * Phase 2: SD card config loading with up to 5 s timeout.
 *          On success loads all config files; on timeout uses defaults.
 * Releases hSysConfigReady semaphore when done, unblocking defaultTask.
 * --------------------------------------------------------------------------*/
void Model::configLoaderTask(void)
{
#ifndef SIMULATOR
	bool usb_found = false;
	bool sd_found  = false;

	/* --- Phase 1+2: detect USB and SD concurrently ---------------------- */
	/* Timeouts are relative to when this task actually starts (not boot time) */
	uint32_t t0 = HAL_GetTick();

	printf("Waiting for USB flash...\n");
	for(;;)
	{
		uint32_t elapsed = HAL_GetTick() - t0;

		/* USB: detection window opens at 500 ms, closes at 1500 ms */
		if(!usb_found && elapsed > 500)
		{
			if(is_USB_flash() && is_USB_flash_mounted())
			{
				usb_found = true;
				printf("USB flash detected [%lu ms]\n", HAL_GetTick());
			}
		}

		/* SD: check any time, timeout at 5000 ms */
		if(!sd_found && is_SD() && is_SD_mounted())
		{
			sd_found = true;
			printf("SD card ready [%lu ms]\n", HAL_GetTick());
		}

		bool usb_done = usb_found || (elapsed > 1500);
		bool sd_done  = sd_found  || (elapsed > 5000);

		if(usb_done && sd_done)
			break;

		osDelay(10);
	}

	/* --- Phase 3: act --------------------------------------------------- */
	uint32_t now = HAL_GetTick();

	if(usb_found && sd_found)
	{
		/* Service update: import config files from USB onto SD */
		printf("Updating SD from USB flash... [%lu ms]\n", now);
		ImportDirFromUSB("1:/Service/Hw",      "0:/Config/Hw");
		ImportDirFromUSB("1:/Service/Menu",    "0:/Config/Menu");
		ImportDirFromUSB("1:/Service/Modes",   "0:/Config/Modes");
		ImportDirFromUSB("1:/Service/Params",  "0:/Config/Params");
		ImportDirFromUSB("1:/Service/Syringe", "0:/Config/Syringe");
		ImportDirFromUSB("1:/Service/User",    "0:/Config/User");
	}
	if(sd_found)
	{
		/* Normal startup: load config from SD */
		if( loadHardwareConfig(fsm_o3_getHwConfig()) == 0 )
			printf("Hardware configuration loaded [%lu ms]\n", now);
		else
			printf("Hardware configuration not loaded, using default.\n");

		if( loadSyringeConfig(fsm_o3_getSyringePattern()) == 0 )
			printf("Syringe stop pattern loaded [%lu ms]\n", now);
		else
			printf("Syringe stop pattern not loaded, using default.\n");

		if( loadUserConfig((USR_CONFIG_T*)fsm_o3_getUsrConfig()) == 0 )
			printf("User configuration loaded [%lu ms]\n", now);
		else
			printf("User configuration not loaded, using default.\n");

		if( loadMainMenu(deviceConfig, MAX_DEV_THERAPIES) != 0 )
		{
			setDefaultMainMenu();
			printf("Main menu not loaded, using default [%lu ms]\n", now);
		}
		else
			printf("Main menu loaded [%lu ms]\n", now);
	}
	else
	{
		/* SD not available */
		setDefaultMainMenu();
		if(usb_found)
			printf("USB found but SD not available, using defaults [%lu ms]\n", now);
		else
			printf("No SD or USB available, using defaults [%lu ms]\n", now);
	}

	osSemaphoreRelease(hSysConfigReady);
#else
	osSemaphoreRelease(hSysConfigReady);
#endif
}

extern "C" void StartConfigLoaderTask(void *argument)
{
	/* Wait until TouchGFXTask has constructed the Model object */
	while(modelInstance == nullptr)
		osDelay(10);

	modelInstance->configLoaderTask();
	osThreadExit();
}



