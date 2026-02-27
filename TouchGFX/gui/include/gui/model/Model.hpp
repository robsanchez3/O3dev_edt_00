#ifndef MODEL_HPP
#define MODEL_HPP

#include <touchgfx/Utils.hpp>
#include <../../../../../Drivers/O3/Fsm_o3/fsm_o3_operation.h>/*TODO......  improve path*/
//#include "ff.h"			/* Declarations of FatFs API */

#define MAX_THERAPY_STEPS	8
#define MAX_DEV_THERAPIES   MAX_OPERATION_MODE


 //extern Model* g_modelInstance;


enum MAIN_MENU_FORMATING {
	MMF_END = -1,
	MMF_END_SHIFT_0_5 = -2,
	MMF_END_SHIFT_1 = -3,
	MMF_END_SHIFT_2 = -4}
;

enum THERAPY_TARGET_VALUES {TTV_CONCENTRATION,
	TTV_FLOW,
	TTV_TIME,
	TTV_VOLUME,
	TTV_DOSE,
	TTV_PRESSURE,
	TTV_VACUUM_TIME,
	TTV_VACUUM_PRESSURE,
	TTV_CALIBRATION_VAL_0,
	TTV_CALIBRATION_VAL_1,
	TTV_CALIBRATION_VAL_2,
	TTV_MAX
};

struct THERAPY_CTX{
	int8_t  stepsNum;                                 // number of selection steps in current therapy
	int16_t mainIcon;                                 // current therapy icon
	bool endTotalDoseVisible;                         // total dose visible at the end of this therapy
	bool endTotalVolumeVisible;                       // total volume visible at the end of this therapy
	bool endTotalTimeVisible;                         // total time visible at the end of this therapy
	bool okButtonVisible;                             // OK button visible during the therapy
	bool bigPauseButtonVisible;                       // OK big pause button visible during the therapy
	int16_t maxPressure;                              // maximum pressure allowed during therapy  TODO: integrate it in the code (not aready done 12/12/26)
	int16_t icon[MAX_THERAPY_STEPS];                  // icon for current selection step
	int32_t minValue[MAX_THERAPY_STEPS];              // minimum value for current selection step
	int32_t maxValue[MAX_THERAPY_STEPS];              // maximum selected value for current selection step
	int32_t defValue[MAX_THERAPY_STEPS];              // default selected value for current selection step
	int8_t negativeValue[MAX_THERAPY_STEPS];          // if current selection step is negative
	int16_t step[MAX_THERAPY_STEPS];                  // step selection
	const char * units[MAX_THERAPY_STEPS];            // units for current selection step
	uint8_t therapyTargetValue[MAX_THERAPY_STEPS];    // state machine target value to update in current selection step
	bool secondSelectionVisible[MAX_THERAPY_STEPS];   // complementary selection magnitude visibility
	int32_t delayIndicatorTime[MAX_THERAPY_STEPS];    // delay indicator time visible before any step, zero not visible (calibration phase)
	bool selectAvailable[MAX_THERAPY_STEPS];          // selection available in current selection step
	bool vacuumNeeded;                                // vacuum needed for this therapy
	void (*okAction[MAX_THERAPY_STEPS])(void); 	      // function pointer to OK button action
	void (*sliderAction[MAX_THERAPY_STEPS])(void);    // function pointer to slider change action
};

class ModelListener;
/*
 * The Model class defines the data model in the model-view-presenter paradigm.
 * The Model is a singular object used across all presenters. The currently active
 * presenter will have a pointer to the Model through deriving from ModelListener.
 *
 * The Model will typically contain UI state information that must be kept alive
 * through screen transitions. It also usually provides the interface to the rest
 * of the system (the backend). As such, the Model can receive events and data from
 * the backend and inform the current presenter of such events through the modelListener
 * pointer, which is automatically configured to point to the current presenter.
 * Conversely, the current presenter can trigger events in the backend through the Model.
 */
class Model
{
public:
	Model();

	Model& getModel();

	/**
	 * Sets the modelListener to point to the currently active presenter. Called automatically
	 * when switching screen.
	 */
	void bind(ModelListener* listener)
	{
		modelListener = listener;
	}

	/**
	 * This function will be called automatically every frame. Can be used to e.g. sample hardware
	 * peripherals or read events from the surrounding system and inject events to the GUI through
	 * the ModelListener interface.
	 */
	void tick();

	void checkPin(const char* pin);
	void blockConfigOptions();
	void unblockConfigOptions();
	bool configOptionsAllowed();
	void setGuiTherapy(int8_t value);
	int8_t getGuiTherapy();
	void initTherapyTargetValues();
	void initStorageDelegates(void);
	void setTherapyTargetValue(uint8_t valueID, uint16_t value);
	uint16_t getTherapyTargetValue(uint8_t valueID);
	void setGenerationMode(bool state);
	bool getGenerationMode();
	bool isGenerationModeAvailable();
	bool isVaccumAvailable();
	char * getSoftwareVersion();
	char * getControlSoftwareVersion();
	int16_t getPressure();
	int16_t getTemperature();
	char * getParameters();
	char * getStartupInfo();
	int8_t * getDeviceConfig();
	void periodCalibrationStart();
	void loadParameterStart();
	void saveParameterStart();
	void flowCalibrationStart();
	void o3CalibrationStart();
	void onSelectionAction(uint8_t selectionStep);
	void onSliderAction(uint8_t step);
	THERAPY_CTX * getTherapyCtx();
	uint16_t ** getTherapyTargets();
	void EndSelection(void);
	void StartGeneration(void);
	void CancelSelection(void);
	void StopGeneration(void);
	void exitError(void);
	void userCancelled(void);
	void userOk(void);
	void userOkRelease(void);
	void cancelWashing(void);
	void washingExtesion(void);
	void gotoRepose(void);
	bool isStarting();
	bool isCalibrationErrorDuringStartUp();
	bool isCompensatingPressure();
	bool isPaused();
	bool isPauseAvailable();
	uint8_t getFsmState();
	uint8_t getErrorState();
	uint8_t getRemainingMinutes();
	uint8_t getRemainingSeconds();
	uint8_t getWashingSeconds();
	int16_t getCurrentOperatingPressure();
	uint32_t getCurrentTotalDose();
	uint32_t getCurrentOutputVolume();
	uint32_t getConfiguredTime();
	uint32_t getConfiguredVolume();
	bool refreshGenerationInfo();
	void clearGenerationInfoFlag();
	uint8_t getCalibrationSucessState();
	void setupDefaultTherapyContext(int8_t therapyID);
	void setupTherapyContext(int8_t therapyID);
	int16_t configureMainMenu(void);
	void setDefaultMainMenu(void);
	void printTherapyTargetValues(void);
	uint8_t is_SD(void);
	uint8_t is_fs_mounted(void);



	//  functions from original demonstration application (start declaration)
#if 0
	float getDevTemperature() const { return Temperature; }
	float ReadTemperatureSensor();
	void  USBCDCSend(int value);
	void  SendRS232(uint8_t value);
	void  SendRS485(uint8_t value);
	void  SetGPIO4Active();
	void  SetGPIO5(char value);
	void  SendCAN(uint8_t value);
	bool  GetGPIO_0_State() const {return ! (GPIO_0_State & 0x01); }
	bool  GetGPIO_1_State() const {return ! (GPIO_1_State & 0x02); }
#endif
	//  functions from original demonstration application (end declaration)


	THERAPY_CTX guiTherapyCtx;

protected:
	/**
	 * Pointer to the currently active presenter.
	 */
	ModelListener* modelListener;

private:
	int8_t      guiTherapy = -1;
	uint16_t   *therapyTargetValues[TTV_MAX];
//	THERAPY_CTX guiTherapyCtx;
	int8_t deviceConfig[MAX_DEV_THERAPIES + 1] = { SYRINGE_AUTO_MODE, CONTINUOUS_MODE, INSUFFLATION_R_MODE, MANUAL_MODE, DENTAL_MODE, VACUUM_TIME_MODE, CLOSED_BAG_MODE, DOSE_MODE, SALINE_MODE, RFU_1_MODE, OPEN_BAG_MODE, SYRINGE_MANUAL_MODE, MMF_END };
	int8_t deviceConfig_default[MAX_DEV_THERAPIES + 1] = { CONTINUOUS_MODE, MMF_END_SHIFT_1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };



	// variables from original demonstration application (start declaration)
	bool    configBlocked = true;
	int     pulsecount, tickcount;
	char    GPIO_0_State, GPIO_1_State, GPIO_5_State, GPIO5_Level;
	char    old_GPIO_0_State, old_GPIO_1_State, old_GPIO_5_State;
	float   Temperature;
	uint8_t n = 0;
	int16_t raw;
	uint8_t Tx_Buffer[50];
	uint8_t Rx_Buffer[50];
	int16_t intTemperature;
	uint8_t USB_CDC_Tx_Buffer[64];
	// variables from original demonstration application (end declaration)

};

#endif /* MODEL_HPP */
