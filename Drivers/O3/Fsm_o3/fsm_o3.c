#include "fsm_o3.h"
#include "fsm_o3_operation.h"


uint32 GLB_FSM_ProcessEvent_Count = 0; // debug purposes
uint32 GLB_TickCounter = 0;
uint32 GLB_SecondCounter = 0;
uint16 GLB_SyringeStopPattern[SYRINGE_PATTERN_NUM  + 1] = { DEF_SYRINGE_STOP_5ML,
		                                                    DEF_SYRINGE_STOP_10ML,
		                                                    DEF_SYRINGE_STOP_20ML,
		                                                    DEF_SYRINGE_STOP_50ML,
	                                                     	DEF_SYRINGE_STOP_100ML,
                                                            0 };
FSM_O3_EVENT_T GLB_FsmEvents= {
	0,	// Enter
	0,	// Cancel
	0	// Generic
};

FSM_O3_OPERATION_T GLB_fsm_o3 = {
    /* LastState */
    &GLB_fsm_o3.States[STATE_INIT],
    /* CurrentState */
    &GLB_fsm_o3.States[STATE_INIT],
    /* States */
    {
	  /* Entry                    Enter                Cancel                 Animate                   Error              Exit             GenericEvent     State ID                            Owner       */
		{Initialize,              GotoService,         DelegateDummy,         DelegateDummy,            DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_INIT,                         &GLB_fsm_o3},
		{DelegateDummy,           DelegateDummy,       ExitError,             DelegateDummy,            DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_ERROR,                        &GLB_fsm_o3},
		{InitialCheck_1,          DelegateDummy,       DelegateDummy,         InitialCheck_1,           DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_INIT_CHECK_1,                 &GLB_fsm_o3},
		{InitialCheck_2_Ini,      DelegateDummy,       DelegateDummy,         InitialCheck_2,           DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_INIT_CHECK_2,                 &GLB_fsm_o3},
		{DelegateDummy,           DelegateDummy,       DelegateDummy,         InitialCheck_3,           DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_INIT_CHECK_3,                 &GLB_fsm_o3},
		{WaitingForProtocol,      LaunchTherapy,       GotoRepose,            DelegateDummy,            DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_WAITING_FOR_PROTOCOL,         &GLB_fsm_o3},
		{O3SensorTune_Ini,        DelegateDummy,       GotoRepose,            O3SensorTune,             DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_TUNING_O3_SENSOR,             &GLB_fsm_o3},
		{DelegateDummy,           DelegateDummy,       GotoRepose,            DelegateDummy,            DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_ADJUSTING,                    &GLB_fsm_o3},
		{GotoGenerating,          KeyOnPush,           GotoUserCancelled,     UpdateGenerating,         DelegateDummy,     DelegateDummy,   KeyOnRelease,    STATE_O3_GENERATING,                &GLB_fsm_o3},
		{DelegateDummy,           LaunchTherapy,       GotoRepose,            DelegateDummy,            DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_USER_CANCELLED,               &GLB_fsm_o3},
		{DelegateDummy,           LaunchTherapy,       GotoRepose,            DelegateDummy,            DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_COMPLETED,                    &GLB_fsm_o3},
		{DelegateDummy,           LaunchTherapy,       GotoRepose,            DelegateDummy,            DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_OVERPRESSURE,                 &GLB_fsm_o3},
		{DelegateDummy,           ModifyVacuum,        CancelVacuum,          UpdateVacuum,             DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_VACUUM_GENERATING,            &GLB_fsm_o3},
		{DelegateDummy,           GotoEndOperation,    CancelTherapyTime,     UpdateTherapyTime,        DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_WAITING_THERAPY_TIME,         &GLB_fsm_o3},  //TODO: review cancel and enter events it shoul be inter changed
		{DelegateDummy,           GotoStuffConnected,  GotoUserCancelled,     DelegateDummy,            DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_WAITING_EXTERNAL_STUFF,       &GLB_fsm_o3},
		{WashingIni,              WashingExtesion,     CancelWashing,         UpdateWashing,            DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_WASHING,                      &GLB_fsm_o3},
		{DelegateDummy,           DelegateDummy,       DelegateDummy,         UpdateDepressure,         DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_DEPRESSURE,                   &GLB_fsm_o3},
		{ResetOption,             DelegateDummy,       DelegateDummy,         DelegateDummy,            DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_WAITING_FOR_SERVICE,          &GLB_fsm_o3},
		{InitialCheck_2_Ini,      DelegateDummy,       GotoService,           InitialCheck_2,           DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_CALIBRATE_O3_1,               &GLB_fsm_o3},
		{StartForO3Calibration,   DelegateDummy,       GotoService,           DelegateDummy,            DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_CALIBRATE_O3_2,               &GLB_fsm_o3},
		{O3SensorCalibrationEnd,  DelegateDummy,       GotoService,           DelegateDummy,            DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_CALIBRATE_O3_3,               &GLB_fsm_o3},
		{DelegateDummy,           DelegateDummy,       GotoService,           DelegateDummy,            DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_SELECT_PRESS_COMP_FACTOR,     &GLB_fsm_o3},
		{DelegateDummy,           DelegateDummy,       GotoService,           CalibratePeriodUpdate,    DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_CALIBRATE_PERIOD,             &GLB_fsm_o3},
		{DelegateDummy,           GotoService,         GotoService,           SaveParameters,           DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_SAVING_PARAMETERS,            &GLB_fsm_o3},
		{DelegateDummy,           GotoService,         GotoService,           LoadParameters,           DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_LOADING_PARAMETERS,           &GLB_fsm_o3},
		{DelegateDummy,           GotoService,         DelegateDummy,         DelegateDummy,            DelegateDummy,     DelegateDummy,   DelegateDummy,   STATE_CALIBRATION_END,              &GLB_fsm_o3},
    },
	/* HwConfig */
	{
        /* CBoard */
	    A10068_XX_X,
        /* O3Sensor */
		NO_O3_SENSOR,
        /* Transformer */
		A9462_03,
        /* O3Tube */
		A10021_02,
        /* VPump */
		NO_VACUUM_PUMP,
        /* LEDStrip */
		NO_LED_STRIP,
        /* Comm */
		SERIAL,
	},
	/* UsrConfig */
	{
		/* userGenerationMode */
		O3_GENERATION_BASED_ON_TUBE_CALIBRATION, //O3_GENERATION_BASED_ON_O3_PHOTOSENSOR
		/* rfu_0 */
		0,
		/* rfu_1 */
		1,
	},
	/* GenerationMode */
	O3_GENERATION_BASED_ON_TUBE_CALIBRATION, //O3_GENERATION_BASED_ON_O3_PHOTOSENSOR
    /* option */
    NO_MODE,
    /* AnimationTimer */
    {
        /* *Counter */
        &GLB_TickCounter,
        /* TimeOut */
        0,
        /* StartTime*/
        0,
        /* State */
        TIMER_STATE_STOP
    },
    /* KeepAliveTimer */
    {
        /* *Counter; */
        &GLB_TickCounter,
        /* TimeOut */
        0,
        /* StartTime*/
        0,
        /* State */
        TIMER_STATE_STOP
    },
    /* DataReadTimer */
    {
        /* *Counter; */
        &GLB_TickCounter,
        /* TimeOut */
        0,
        /* StartTime*/
        0,
        /* State */
        TIMER_STATE_STOP
    },
    /* AdjustTimer */
    {
        /* *Counter; */
        &GLB_TickCounter,
        /* TimeOut */
        0,
        /* StartTime*/
        0,
        /* State */
        TIMER_STATE_STOP
    },
    /* WaitForServiceKeyTimer */
    {
        /* *Counter; */
        &GLB_TickCounter,
        /* TimeOut */
        0,
        /* StartTime*/
        0,
        /* State */
        TIMER_STATE_STOP
    },
    /* HihgVoltDisconectTimer */
    {
        /* *Counter */
        &GLB_TickCounter,
        /* TimeOut */
        0,
        /* StartTime*/
        0,
        /* State */
        TIMER_STATE_STOP
    },
    /* CleanGeneratorTimer */
    {
        /* *Counter */
        &GLB_TickCounter,
        /* TimeOut */
        0,
        /* StartTime*/
        0,
        /* State */
        TIMER_STATE_STOP
    },
    /* AuxTimer */
    {
        /* *Counter */
        &GLB_TickCounter,
        /* TimeOut */
        0,
        /* StartTime*/
        0,
        /* State */
        TIMER_STATE_STOP
    },
    /* SyringeCtrl */
    {
        /* PeakDetected */
        0,
        /* DeltaPressureLimit */
        INITIAL_DELTA_PRESSURE_LIMIT,
        /* DeltaPressureBefore */
        0,
        /* EndOfFillingCounter */
        0,
        /* OperatingPressureBefore */
        0,
        /* IniTime */
        0,
        /* FillingTime */
        0,
        /* DetachTime */
        0,
        /* PressStableTime */
        0,
        /* ModifyDeltaPressureLimitTime */
        0,
        /* *BaseTime */
        &GLB_TickCounter,
        /* *SyringePattern */
        &GLB_SyringeStopPattern[0],
    },
	/* DebLevel */
	D_LEV_ALL | D_LEV_HIDE_TIME_STAMP,
	/* StartStorageSave */
	DelegateDummy2,
	/* StartStorageLoad */
	DelegateDummy2,
	/* writeStorageLine */
	DelegateDummy3,
	/* ReadStorageLine */
	DelegateDummy4,
	/* StoptStorage */
	DelegateDummy,
    /* SharedBuffer[16*8] */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    /* Sample Position */
/////    0,
    /* ConfiguredO3Concentration */
    0,
    /* ConfiguredFlow */
    30,
    /* ConfiguredTime */
    0,
	/*  ConfiguredVacuumTime */
    0,
    /*  ConfiguredVacuumPressure */
    0,
	/* ConfiguredDose */
	0,
	/* ConfiguredPressure */
	0,
    /* ConfiguredVolume */
    0,
    /* RemainingHours */
    0,
    /* RemainingMinutes */
    0,
    /* RemainingSeconds */
    0,
    /* FreezeRemainingTime */
    0,
    /* StableFlow */
    FALSE,
    /* StableOzone */
    FALSE,
    /* SensorData */
    {
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0}
    },
    /* SensorDataIndex */
	0,
    /* SensorDataFilled */
	0,
    /* ErrorState */
    NO_ERROR,
    /* CurrentMeanOzone */
    0,
    /* CurrentMeanFlow */
    0,
    /* CurrentTotalDose */
    0,
    /* CurrentOutputVolume */
    0,
    /* ReferenceOutputVolume */
    0,
    /* CumulatedOutputVolume */
    0,
    /* KeyEnterPushed */
    NOT_PUSHED,
    /* PressAtm */
    0,
    /* PressAdjust */
    0,
    /* MaxAllowedFlow */
    MAX_FLOW_VALUE,
    /* MaxAllowedTime */
    MAX_TIME_VALUE,
    /* TemperatureMonitoring */
    0,
    /* ManageCommandError */
	PROTOCOL_RET_SUCCESS,
    /* MaxAvailableFlow */
    MIN_FLOW_VALUE,
    /* WashingConfiguredTime */
    0,
    /* WashingSeconds */
    0,
    /* WashingReturnState */
    STATE_COMPLETED,
    /* SyringeDetected */
    0,
    /* CalibrationErrorDuringStartUp */
    0,
//    /* Starting */
//    1,  remove when properly tested
    /* MaxFlowDeviation */
    0,
	/* RefreshDisplay */
	0,
    /* CurrentOperatingPressure; */
	0,
	/* DepressureSeconds */
	0,
	/* DepressureReturnState */
	STATE_COMPLETED,
	/* DepressureReturnState */
	NO_ERROR,
    /* GeneratorCleaned */
/////    0,
	/* VacuumStatus */
	VACUUM_STATE_STOP,
	/* SalineCycle */
	SALINE_MIXING_CYCLE,
	/* PressureAlertDone */
	FALSE,
	/* CalibrationValue_0 */
	0,
	/* CalibrationValue_1 */
	0,
	/* CalibrationValue_2 */
	0,
    /*  InsufflationState */
	INSUFFLATION_STATE_PAUSED,
	/* SyringeManualState; */
	SYRINGE_MANUAL_STATE_PAUSED,
    /* MaxCurrent */
	0,
    /* LastModBasePeriod */
	0,
    /* MaxModBasePeriod */
	0,
	/* PressThreshold */
	200,
	/* DentalSupplyDetectionPressure */
	0,
	/* DentalSupplying */
	0
};

void fsm_o3_timeHandler(void)
{
  GLB_TickCounter++;
  GLB_SecondCounter++;
  /* Per second timer management */
  if (GLB_SecondCounter == 1000)
  {
//	printf("Miliseconds since starting: %ld\n", GLB_TickCounter);

    GLB_SecondCounter = 0;

#if 0
    /* Force repainting */   // TODO: is it necessary or is legacu from OZP/T
//  Commented out on 19/12/2025 to test it it is necessary or is just legacy from OZP/T
    GLB_fsm_o3.AnimationTimer.StartTime = GLB_TickCounter;
    GLB_fsm_o3.AnimationTimer.TimeOut = GLB_fsm_o3.AnimationTimer.StartTime + 1;
    GLB_fsm_o3.AnimationTimer.State = TIMER_STATE_STARTED;
#endif
    GLB_fsm_o3.RefreshScreen = 1;  // TODO test user experience refreshing more often (500 or 250 ms)

    if(GLB_fsm_o3.FreezeRemainingTime == 0)
    {
      if (GLB_fsm_o3.RemainingSeconds == 0)
      {
        if (GLB_fsm_o3.RemainingMinutes != 0)
        {
          GLB_fsm_o3.RemainingMinutes--;
          GLB_fsm_o3.RemainingSeconds = 59;
        }
      }
      else
      {
        GLB_fsm_o3.RemainingSeconds--;
      }
    }

    if(GLB_fsm_o3.WashingSeconds)
    {
      GLB_fsm_o3.WashingSeconds--;
    }
    if(GLB_fsm_o3.DepressureSeconds)
    {
      GLB_fsm_o3.DepressureSeconds--;
    }
  }
}

void fsm_o3_main(void)
{
	static uint8  GLB_Initialized = 0;

	if (!GLB_Initialized)
	{
		GLB_Initialized = 1;
		GLB_fsm_o3.CurrentState->Entry();
	}
	else
	{
//		printf("In..........................................................................................................................................\n");
		FSM_ProcessEvents();
//		printf("Out..........................................................................................................................................\n");
	}
}

