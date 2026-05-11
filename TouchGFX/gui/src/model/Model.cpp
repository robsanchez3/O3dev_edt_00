#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <gui/common/FrontendApplication.hpp>
#include <gui/model/parseappconfig.hpp>
#include <gui/model/parseUpdates.hpp>

#ifndef SIMULATOR

#include <gui/model/TherapyTemplates.hpp>
#include <gui/model/ParamStore.hpp>
#include "BitmapDatabase.hpp"
#include "main.h"
#include "cmsis_os2.h"
#include "stm32u5xx_hal.h"
#include "../../../../Drivers/O3/Fsm_o3/fsm_o3_api.h"
#include "../../../../Drivers/IAP/gen_updater.h"
#include "../../../../Drivers/Log/log.h"
#include <stdio.h>
#include <string.h>
extern "C" osSemaphoreId_t hSysConfigReady;  /* signals defaultTask that sys config is ready */
extern "C" osThreadId_t    defaultTaskHandle;
#endif

#define PIN "1234"  // hard coded PIN

// TODO review fsm_o3_setTemperatureMonitoring politic ... force fan while generation and monitoring while repose isn`t it??

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

static const char* therapyName(uint16_t mode);

void Model::tick()
{
#ifndef SIMULATOR
	if (tickDevUpdate()) return;
#endif
	tickScreenUpdate();
}

void Model::tickScreenUpdate()
{
	static char debug_done = 0;

//  REMEMBER: The Model has a pointer to your currently active Presenter by means of ModelListener (where getVisibleScreen() has been defined as virtual)

//  Update screen according to current FSM state
	// TODO: analyze if it is possible change screen in "controlled way"
	// TODO: analyze if it is possible change screen in "controlled way"
	switch(getFsmState())
	{
	case STATE_ERROR:
		if(modelListener->getVisibleScreen() != SID_ERROR)
		{
			log_finish(LOG_RESULT_ERROR, (int32_t)fsm_o3_getErrorState());
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
			uint8_t st = getFsmState();
			LogResult_t r = (st == (uint8_t)STATE_COMPLETED)      ? LOG_RESULT_OK :
			                (st == (uint8_t)STATE_USER_CANCELLED)  ? LOG_RESULT_USER_CANCEL :
			                                                          LOG_RESULT_ERROR;
			log_finish(r, (r == LOG_RESULT_ERROR) ? (int32_t)st : 0);
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
	case STATE_WAITING_EXTERNAL_STUFF:
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
}

#if 0
void Model::tickScreenUpdate_Back()
{
	static char debug_done = 0;

//  REMEMBER: The Model has a pointer to your currently active Presenter by means of ModelListener (where getVisibleScreen() has been defined as virtual)

//  Update screen according to current FSM state
	// TODO: analyze if it is possible change screen in "controlled way"
	// TODO: analyze if it is possible change screen in "controlled way"
	switch(getFsmState())
	{
	case STATE_ERROR:
		if(modelListener->getVisibleScreen() != SID_ERROR)
		{
			log_finish(LOG_RESULT_ERROR, (int32_t)fsm_o3_getErrorState());
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
			uint8_t st = getFsmState();
			LogResult_t r = (st == (uint8_t)STATE_COMPLETED)      ? LOG_RESULT_OK :
			                (st == (uint8_t)STATE_USER_CANCELLED)  ? LOG_RESULT_USER_CANCEL :
			                                                          LOG_RESULT_ERROR;
			log_finish(r, (r == LOG_RESULT_ERROR) ? (int32_t)st : 0);
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
	case STATE_WAITING_EXTERNAL_STUFF:
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
} // Model::tickScreenUpdate()
#endif
#ifndef SIMULATOR
bool Model::tickDevUpdate()
{
	bool import_active = dev_upd_import_is_active();
	bool export_active = dev_upd_export_is_active();
	bool fw_pending    = dev_upd_is_pending();

	/* Navigate to DevUpdate if any process is active and we're not already there */
	if ( (import_active || export_active || fw_pending) && (modelListener->getVisibleScreen() != SID_DEV_UPDATE) )
	{
		if (fw_pending && !import_active && !export_active)
		{
			osThreadSuspend(defaultTaskHandle);
		}
		static_cast<FrontendApplication*>(Application::getInstance())->gotoDevUpdateScreenNoTransition();
		return true;
	}

	if (modelListener->getVisibleScreen() != SID_DEV_UPDATE)
	{
		return false;
	}
	/* Device update state machine */
	if (import_active && dev_upd_import_is_pending())
	{
		modelListener->onDevUpdTick(0, (int)DEV_UPD_IMPORT_CONFIRM, dev_upd_import_get_dir());
	}
	else if (export_active && dev_upd_export_is_pending())
	{
		modelListener->onDevUpdTick(0, dev_upd_export_is_notify() ? (int)DEV_UPD_EXPORT_DONE : (int)DEV_UPD_EXPORT_CONFIRM, dev_upd_export_get_msg());
	}
	else if (fw_pending)
	{
		modelListener->onDevUpdTick(dev_upd_get_progress(), (int)dev_upd_get_state(), dev_upd_get_msg());
	}
	else if (!import_active && !export_active && !fw_pending)
	{
		/* Debounce ~83 ms: avoids flash to Starting when configLoaderTask transitions between phases and briefly leaves all flags clear. */
		static uint8_t s_idle_ticks = 0;
		if (++s_idle_ticks >= 5)
		{
			s_idle_ticks = 0;
			static_cast<FrontendApplication*>(Application::getInstance())->gotoStartingScreenNoTransition();
		}
	}

	return true;
}
#endif

void Model::resetSystem()
{
	HAL_NVIC_SystemReset();
}

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
	setupTherapyContext(value);

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
		(int8 (*)())p_store_openWrite,
		(int8 (*)())p_store_openRead,
		(int8 (*)(uint16, int32))p_store_writeLine,
		(int8 (*)(uint16, int32 *))p_store_readLine,
		p_store_close
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

static const char* therapyName(uint16_t mode)
{
    switch ((OPERATION_MODE_E)mode) {
    case SYRINGE_MODE:          return "Syringe";
    case SYRINGE_AUTO_MODE:     return "Syringe Auto";
    case SYRINGE_MANUAL_MODE:   return "Syringe Manual";
    case CONTINUOUS_MODE:       return "Continuous";
    case INSUFFLATION_MODE:     return "Insufflation";
    case INSUFFLATION_R_MODE:   return "Insufflation R";
    case INSUFFLATION_V_MODE:   return "Insufflation V";
    case MANUAL_MODE:           return "Manual";
    case DENTAL_MODE:           return "Dental";
    case VACUUM_MODE:           return "Vacuum";
    case VACUUM_TIME_MODE:      return "Vacuum Time";
    case VACUUM_PRESSURE_MODE:  return "Vacuum Pressure";
    case BAG_MODE:              return "Bag";
    case CLOSED_BAG_MODE:       return "Closed Bag";
    case OPEN_BAG_MODE:         return "Open Bag";
    case DOSE_MODE:             return "Dose";
    case SALINE_MODE:           return "Saline";
    default:                    return "Unknown";
    }
}

void Model::EndSelection(void)
{
	printf("End of selection (model)...\n");
#ifndef SIMULATOR
	fsm_o3_sendEnter();

	if(getFsmState() >= STATE_WAITING_FOR_SERVICE)
	{
		static_cast<FrontendApplication*>(Application::getInstance())->gotoCalibrationScreenNoTransition();
	}
	else
	{
		log_start(therapyName(fsm_o3_getOption()));

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
	fsm_o3_sendCancel();
#endif
}

void Model::exitError(void)
{
	printf("Exit error (model)...\n");
#ifndef SIMULATOR
	fsm_o3_sendCancel();
#endif
}

void Model::userCancelled(void)
{
//	printf("User cancelled (model)...\n");
#ifndef SIMULATOR
	fsm_o3_sendCancel();
	fsm_o3_setTemperatureMonitoring(0);
#endif
}

void Model::userOk(void)
{
	printf("User OK (model)...\n");
#ifndef SIMULATOR
	if(getFsmState() == (uint8_t)STATE_WAITING_FOR_PROTOCOL)
	{
		log_start(therapyName(fsm_o3_getOption()));
	}
	fsm_o3_sendEnter();
#endif
}

void Model::userOkRelease(void)
{
//	printf("User OK released (model)...\n");
#ifndef SIMULATOR
	fsm_o3_sendGeneric(); // to handle OK release event
#endif
}

void Model::setLogEnabled(bool enabled)
{
#ifndef SIMULATOR
    log_set_enabled(enabled);
#endif
}

void Model::cancelWashing(void)
{
	printf("Washing cancelled (model)...\n");
#ifndef SIMULATOR
	fsm_o3_sendCancel();
#endif
}

void Model::washingExtesion(void)
{
	printf("Washing extension (model)...\n");
#ifndef SIMULATOR
	fsm_o3_sendEnter();
#endif
}

void Model::gotoRepose(void)
{
//	printf("Go to repose (model)...\n");
#ifndef SIMULATOR
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
void Model::setupDefaultTherapyContext(int8_t therapyID)
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

void Model::setupTherapyContext(int8_t therapyID)
{
	memset(&guiTherapyCtx, 0, sizeof(guiTherapyCtx));

	for(int8_t i = 0; i < MAX_THERAPY_STEPS; i++)
	{
		guiTherapyCtx.okAction[i] = fsm_o3_no_op;       // avoid null pointer dereference
		guiTherapyCtx.sliderAction[i] = fsm_o3_no_op;   // avoid null pointer dereference
		guiTherapyCtx.units[i] = "";                    // point to empty string literal
	}
	setupDefaultTherapyContext(therapyID);
	print_guiTherapyCtx(&guiTherapyCtx);

	if( !loadTherapyFromFile((OPERATION_MODE_E)therapyID, &guiTherapyCtx) )
	{
		printf("Failed to load therapy context from file for therapy code %d. Using default values.\n", therapyID);
		setupDefaultTherapyContext(therapyID);
	}

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
    for(int j = MAX_THERAPY_STEPS - 1; j >= 0; j--)
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

	if(usb_found)
		gen_upd_scan_usb();  /* check for "1:/GEN_UPDATE" folder */

	if(dev_upd_is_pending())
	{
		/* Wait for user decision on DevUpdate screen */
		while(dev_upd_is_pending() && dev_upd_get_state() == DEV_UPD_IDLE)
			osDelay(50);

		if(dev_upd_get_state() == DEV_UPD_IN_PROGRESS)
		{
			/* User confirmed gen fw update — system resets after IAP, skip config load */
			setDefaultMainMenu();
			printf("Generator update running, skipping config load [%lu ms]\n", now);
			osSemaphoreRelease(hSysConfigReady);
			return;
		}
		printf("Generator update cancelled, continuing with service check [%lu ms]\n", now);
	}

	if(usb_found && sd_found)
	{
		uint8_t has_hw      = (f_stat("1:/Service/Hw",      NULL) == FR_OK);
		uint8_t has_menu    = (f_stat("1:/Service/Menu",     NULL) == FR_OK);
		uint8_t has_modes   = (f_stat("1:/Service/Modes",    NULL) == FR_OK);
		uint8_t has_params  = (f_stat("1:/Service/Params",   NULL) == FR_OK);
		uint8_t has_syringe = (f_stat("1:/Service/Syringe",  NULL) == FR_OK);
		uint8_t has_user    = (f_stat("1:/Service/User",     NULL) == FR_OK);

		if(has_hw || has_menu || has_modes || has_params || has_syringe || has_user)
		{
			printf("Service update detected, requesting user confirmation... [%lu ms]\n", now);
			dev_upd_import_begin();
			if (has_hw      && dev_upd_import_request("Hw"))      ImportDirFromUSB("1:/Service/Hw",      "0:/Config/Hw");
			if (has_menu    && dev_upd_import_request("Menu"))    ImportDirFromUSB("1:/Service/Menu",    "0:/Config/Menu");
			if (has_modes   && dev_upd_import_request("Modes"))   ImportDirFromUSB("1:/Service/Modes",   "0:/Config/Modes");
			if (has_params  && dev_upd_import_request("Params"))  ImportDirFromUSB("1:/Service/Params",  "0:/Config/Params");
			if (has_syringe && dev_upd_import_request("Syringe")) ImportDirFromUSB("1:/Service/Syringe", "0:/Config/Syringe");
			if (has_user    && dev_upd_import_request("User"))    ImportDirFromUSB("1:/Service/User",    "0:/Config/User");
			dev_upd_import_end();
		}
	}
	if(usb_found)
	{
		dev_upd_export_begin();
		if(dev_upd_export_request("Export logs to USB drive?"))
		{
			if(f_stat("1:/Log", NULL) == FR_OK)
			{
				if(dev_upd_export_request("Log directory exists.\nOverwrite?"))
				{
					ClearDirRecursive("1:/Log");
					ExportDirToUSB("0:/Config", "1:/Log");
					ExportDirToUSB("0:/log",    "1:/Log/log");
					dev_upd_export_notify("Export complete!\n\nRemove USB drive.\nPress OK to continue.");
				}
			}
			else
			{
				ExportDirToUSB("0:/Config", "1:/Log");
				ExportDirToUSB("0:/log",    "1:/Log/log");
				dev_upd_export_notify("Export complete!\n\nRemove USB drive.\nPress OK to continue.");
			}
		}
		dev_upd_export_end();
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


//****************************************************************************************************************************************************************************************************


#if 0 //  functions from original demonstration application (start declaration)

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


