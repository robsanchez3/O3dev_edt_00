#ifndef FSM_H_
#define FSM_H_

#include <stdio.h>
#include <stdarg.h>

#include "fsm_o3.h"
#include "../Typedef.h"
#include "../Timer/Timer.h"
#include "../Syringe/Syringe.h"
#include "../Protocol/Protocol.h"

/*
╭───────────┬──────────┬─────────┬──────────┬─────────┬────────┬─────────────┬────────────────╮
│   TUBE    │  TRAFO   │ SENSOR  │  VACUUM  │ LED_STR │  BUS   │ CONT. BOARD │     DEVICE     │
├───────────┼──────────┼─────────┼──────────┼─────────┼────────┼─────────────┼────────────────┤
│ A10021_02 │ A9462_03 │   NO    │    NO    │   NO    │ SERIAL │ A10068 62 A │ Ozonette       │
├───────────┼──────────┼─────────┼──────────┼─────────┼────────┼─────────────┼────────────────┤
│ A10055_01 │ A9462_04 │ AS03170 │ 52402054 │   NO    │ SERIAL │ A10026 04 A │ Ozonobaric P   │
├───────────┼──────────┼─────────┼──────────┼─────────┼────────┼─────────────┼────────────────┤
│           │          │         │          │         │        │             │                │
├───────────┼──────────┼─────────┼──────────┼─────────┼────────┼─────────────┼────────────────┤
│           │          │         │          │         │        │             │                │
├───────────┼──────────┼─────────┼──────────┼─────────┼────────┼─────────────┼────────────────┤
│           │          │         │          │         │        │             │                │
╰───────────┴──────────┴─────────┴──────────┴─────────┴────────┴─────────────┴────────────────╯


╭─────────┬─────────┬─────────┬─────────┬─────────────┬────────────────╮
│  TUBE   │  TRAFO  │ SENSOR  │   BUS   │ CONT. BOARD │     DEVICE     │
├─────────┼─────────┼─────────┼─────────┼─────────────┼────────────────┤
│         │         │         │   I2C   │             │                │
│         │         │    NO   ├─────────┼─────────────┼────────────────┤
│         │         │         │  SERIAL │             │                │
│         │  SMALL  ├─────────┼─────────┼─────────────┼────────────────┤
│         │         │         │   I2C   │             │                │
│         │         │   YES   │─────────┼─────────────┼────────────────┤
│         │         │         │  SERIAL │             │                │
│  SMALL  ├─────────┼─────────┼─────────┼─────────────┼────────────────┤
│         │         │         │   I2C   │             │                │
│         │         │    NO   ├─────────┼─────────────┼────────────────┤
│         │         │         │  SERIAL │ A10068 62 A │ Ozonette       │
│         │   BIG   ├─────────┼─────────┼─────────────┼────────────────┤
│         │         │         │   I2C   │             │                │
│         │         │   YES   │─────────┼─────────────┼────────────────┤
│         │         │         │  SERIAL │             │                │
├─────────┼─────────┼─────────┼─────────┼─────────────┼────────────────┤
│         │         │         │   I2C   │             │                │
│         │         │    NO   ├─────────┼─────────────┼────────────────┤
│         │         │         │  SERIAL │             │                │
│         │  SMALL  ├─────────┼─────────┼─────────────┼────────────────┤
│         │         │         │   I2C   │             │                │
│         │         │   YES   │─────────┼─────────────┼────────────────┤
│         │         │         │  SERIAL │             │                │
│   BIG   ├─────────┼─────────┼─────────┼─────────────┼────────────────┤
│         │         │         │   I2C   │             │                │
│         │         │    NO   ├─────────┼─────────────┼────────────────┤
│         │         │         │  SERIAL │             │                │
│         │   BIG   ├─────────┼─────────┼─────────────┼────────────────┤
│         │         │         │   I2C   │             │                │
│         │         │   YES   ├─────────┼─────────────┼────────────────┤
│         │         │         │  SERIAL │ A10026 04 A │ Ozonobaric P   │
╰─────────┴─────────┴─────────┴─────────┴─────────────┴────────────────╯
*/
typedef enum
{
  NO_ERROR = 0,
  TEMP_ERROR = 1,
  CURRENT_ERROR = 2,
  OZONE_ERROR = 3,
  FLOW_ERROR = 4,
  PRESS_ERROR = 5,
  LEAKAGE_ERROR = 6,
  PIN_ERROR = 7,
  COMS_ERROR = 8,
  CAL_ERROR = 9,
  TRANSFORMER_ERROR = 10,
  PROPORIONAL_VALVE_ERROR = 11,
  PASSWORD_ERROR = 12,
  PRESS_SENSOR_ERROR = 13,
  VALVE_0_ERROR = 14,
  VALVE_1_ERROR = 15,
  SAVE_PARAMETERS_ERROR = 16,
  LOAD_PARAMETERS_ERROR = 17,
  PERIOD_ERROR = 18,
  UNEXPECTED_ERROR = 19,
  MAX_ERROR
}FSM_O3_APP_ERRORS;

typedef enum
{
  NO_WARNING = NO_ERROR,
  MAX_WARNING = -1
}FSM_O3_APP_WARNINGS;

/* Compilation date */
#define DATE_STAMP  __DATE__    /*!< Compilation date. */
#define TIME_STAMP  __TIME__    /*!< Compilation time. */

#define SW_VERSION                       "V0.R0.P0_a"


/* Pressure references for different therapies */
#define OVERPRESSURE_THRESHOLD                    700	// mbar  General over pressure threshold
#define OVERPRESSURE_THRESHOLD_MANUAL             700   // mBar  Over pressure threshold for manual mode
#define OVERPRESSURE_THRESHOLD_SYRINGE            950   // mBar  Over pressure threshold for syringe mode
#define OVERPRESSURE_THRESHOLD_INSUFLATION        200   // mBar  Over pressure threshold for insufflation mode
#define LOWPRESSURE_THRESHOLD_VACUUM             -200   // mBar  Low pressure threshold for vacuum generation mode
#define OVERPRESSURE_STEP_THRESHOLD_DENTAL        200   // mBar  Step pressure threshold for dental therapy
#define DENTAL_PRESSURE_ALERT_THRESHOLD		      300   // mBar  Pressure alert threshold for dental therapy
#define PISTON_PRESSURE_INITIAL_PEAK               32   // mBar  Initial peak over the operating pressure when piston start to move
#define FILL_PRESSURE_DEVIATION                    20	// mBar  Maximum pressure deviation during filling process

/* Ozone references for different utilities */
#define MAX_OZONE_DEVIATION                       2*8	// ug/Nml * 8
#define OZONE_START_UP_TEST_CURRENT              40*8	// ug/Nml * 8  ( Start up O3 for test current )
#define O3_CALIBRATION_POINT_2                   15*8	// ug/Nml * 8  TODO: review naming
#define O3_CALIBRATION_POINT_3                   60*8	// ug/Nml * 8


/* Flow references for different utilities */
#define MAX_FLOW_VALUE                             50	// l/h
#define MIN_FLOW_VALUE                             10	// l/h
#define WASHING_FLOW                               50	// l/h
#define DEFAULT_FLOW_VALUE                         30	// l/h
#define FLOW_O3_CALIBRATION_OZP                    20*8	// l/h * 8
#define MAX_FLOW_LEVEL_WITHOUT_O3_ON_GENERATION_START   15	// l/h  /* Under this flow level O3 generation will start when stable flow were reached */  TODO: review if still necessary
#define FLOW_START_UP_TEST_CURRENT                20*8	// l/h * 8
#define FLOW_START_UP_TEST_CURRENT_FOR_OZT        30*8  // l/h * 8

/* Time references for different utilities */
#define MAX_TIME_VALUE                             20	// min
#define DEFAULT_WHASHING_TIME                      10	// seconds
#define COMMUNICATION_TEST_PERIOD_MS             1500	// ms
#define ANIMATION_PERIOD_MS                      1000	// ms
#define ADJUST_TIMEOUT_MS                       30000	// ms
#define SERVICE_ENTRY_TIME_WINDOW                3500	// ms
#define HIGH_VOLT_DETECTION_TIMEOUT              5000	// ms

/*  TODO: Review this values and comments (NOT USED NOW)
 * There are to events at the end of filling process during in syringe mode:
 * - Pressure increase.
 * - Flow decrease.
 *
 * At 'high speed reading', flow decrease is detected before syringe full pressure
 * is detected. Then 'flow error' is displayed instead of 'syringe full'.
 * Using 'low speed reading' syringe full pressure is detected before flow media
 * results in 'flow error'.
 */
#define HI_SPEED_DATA_READ_PERIOD_MS                1	// ms
#define LOW_SPEED_DATA_READ_PERIOD_MS             250 	// ms

/* Temperature references for different utilities */
#define FAN_ON_TEMPERATURE                     35*127	// ºC * 127  ( Temperature limit to start fan when therapy is selected )
#define FAN_HYSTERESIS                          2*127	// ºC * 127

/* Percentages for different utilities */
#define FLOW_DEVIATION_PERCENTAGE                  15	// %
#define EXTENDED_FLOW_DEVIATION_PERCENTAGE         20	// %

/* Save restore parameters */
#define MAX_PARAMETER_WORDS                       197
#define PARAM_CALIBRATED_VALUES                0x0CA0	// Table contents calibrated values

/* Pushed state for manual mode */
#define PUSHED                                      1
#define NOT_PUSHED                                  0

/* Insufflation states */
#define INSUFFLATION_STATE_ON                       1
#define INSUFFLATION_STATE_PAUSED                   0

/* Manual syringe states */
#define SYRINGE_MANUAL_STATE_ON                     1
#define SYRINGE_MANUAL_STATE_PAUSED                 0

/* Syringe control data */  // TODO: reconsider location
#define SYRINGE_PATTERN_NUM                         5
#define DEF_SYRINGE_STOP_5ML                     4000
#define DEF_SYRINGE_STOP_10ML                    6150
#define DEF_SYRINGE_STOP_20ML                   10828
#define DEF_SYRINGE_STOP_50ML                   23404
#define DEF_SYRINGE_STOP_100ML                  45000

/* O3 generation control*/
#define O3_FLOW_REACHED                             2
#define O3_CONCENTRATION_REACHED                    2
#define O3_CONCENTRATION_REACHED_ACCURATE           3
#define O3_ERROR_DETECTION_ITERATIONS               7    /* @ 10l/h - 1ug/Nml */

/* Miscellaneous definitions */
#define FALSE   0
#define TRUE    1

#define DEFAULT_PRESSURE_GAIN                     506	// Default pressure gain value for calibration purposes
#define FLOW_START_UP_TEST_FLOW                   800	// Set proportional valve completely opened ( Start up flow for test flow)

#define NUM_SENSOR_SAMPLES                         16	// Chose multiple of 2 value and review each constant use to adjust '>>' operator
#define MAX_POSITIVE_FLOW_ERRORS                   20
#define PROP_VALVE_CLOSED_VALUE                     5

#define INITIAL_PERIOD_FOR_O3_CALIBRATION         1500
#define INITIAL_PULSES_FOR_O3_CALIBRATION          220
#define INITIAL_PRESSURE_COMPENSATION_FACTOR        32

#define INI_BASE_PERIOD_CALCULATION_EQUATION(period, pressure, factor)   (period + (uint16)((uint32)(((uint32)factor * (uint32)pressure) >> 6)))
#define PERIOD_PRESSURE_COMPENSATION_EQUATION(period, pressure, factor)  (period - (uint16)((uint32)(((uint32)factor * (uint32)pressure) >> 6)))

// TODO: review why these definitions are not necessary and delete them if so
#define GUN_DEPRESSURE_THRESHOLD_MAX                             800    /* mBar */
#define GUN_DEPRESSURE_THRESHOLD_MIN                             400    /* mBar */
#define GUN_OVERPRESSURE_STEP_THRESHOLD                          200    /* mBar */
#define GUN_PRESSURE_ALERT_THRESHOLD                             300    /* mBar */
#define OVERPRESSURE_DEFAULT_THRESHOLD_GUN                       700    /* mBar */
#define PRESSURE_THRESHOLD_FOR_GUN_SUPPLYING_DETECTION            50    /* mBar */
#define MINIMUM_PRESSURE_THRESHOLD_FOR_GUN_SUPPLYING_DETECTION     8    /* mBar */
#define MINIMUM_PRESSURE_THRESHOLD_FOR_DENTAL_SUPPLYING_DETECTION  8    /* mBar */
#define DEPRESSURE_TIME_MS                                       100    /* ms   */
#define HIDE_FLOW_ERROR_TOUT_GUN_MODE                           3000    /* ms   */
#define DECREASE_FLOW_FACTOR                                       1


 /* Device type enumeration. */
/*
typedef enum
{
   DEV_OZT = 0,
   DEV_OZP,
   DEV_V3,
   DEV_V3S,
   DEV_MAX,
}DEVICE_TYPE_E;
*/
/* Sensor type enumeration. */
typedef enum
{
   SYRINGE_ID_5ML = 0,
   SYRINGE_ID_10ML,
   SYRINGE_ID_20ML,
   SYRINGE_ID_50ML,
   SYRINGE_ID_100ML,
   SYRINGE_ID_MAX
}SYRINGE_ID_E;


/* Sensor identifier enumeration. */
typedef enum
{
  ID_SENSOR_TEMPERATURE_0 = 0, /* ID_SENSOR_TEMPERATURE_0 */
  ID_SENSOR_PRESSURE_0,        /* ID_SENSOR_PRESSURE_0    */
  ID_SENSOR_PRESSURE_1,        /* ID_SENSOR_PRESSURE_1    */
  ID_SENSOR_FLOW,              /* ID_SENSOR_FLOW          */
  ID_SENSOR_CURRENT,           /* ID_SENSOR_CURRENT       */
  ID_SENSOR_OZONE,             /* ID_SENSOR_OZONE         */
  ID_SENSOR_VOLUME,            /* ID_SENSOR_VOLUME        */
  ID_SENSOR_OZONE_DIRECT,      /* ID_SENSOR_TEMPERATURE_1 */
  ID_SENSOR_MAX                /* ID_SENSOR_MAX           */
}ID_SENSOR_E;

 /* FSM O3 Operation modes. */
typedef enum
{
  SYRINGE_MODE         = 0,
  SYRINGE_AUTO_MODE    = 1,
  SYRINGE_MANUAL_MODE  = 2,
  CONTINUOUS_MODE      = 3,
  INSUFFLATION_MODE    = 4,
  INSUFFLATION_R_MODE  = 5,
  INSUFFLATION_V_MODE  = 6,
  MANUAL_MODE          = 7,
  DENTAL_MODE          = 8,
  VACUUM_MODE          = 9,
  VACUUM_TIME_MODE     = 10,
  VACUUM_PRESSURE_MODE = 11,
  BAG_MODE             = 12,
  CLOSED_BAG_MODE      = 13,
  OPEN_BAG_MODE        = 14,
  DOSE_MODE            = 15,
  SALINE_MODE          = 16,
  RFU_1_MODE           = 17,
  MAX_OPERATION_MODE   = 18,

  CAL_PRESS_MODE       = 19,
  CAL_FLOW_MODE        = 20,
  CAL_O3_MODE          = 21,
  CAL_PERIOD_MODE      = 22,
  GENERATION_TYPE_MODE = 23,
  SAVE_PARAMS_MODE     = 24,
  LOAD_PARAMS_MODE     = 25,
  SW_VERSION_MODE      = 26,
  PRESSURE_TEMP_MODE   = 27,
  SHOW_PARAMS_MODE     = 28,
  SHOW_STARTUP_MODE    = 29,

  NO_MODE              = 30,   // It always must be the last valid mode in this enumeration (include new ones above and increase number)
  SERVICE_MODE         = 100,
}OPERATION_MODE_E;

 /* Proportional valves enumeration. */
typedef enum
{
  PROP_VALVE_INPUT = 0 /* PROPORTIONAL INPUT VALVE */
}PROP_VALVE_ID_E;

/* Output valves enumeration. */
typedef enum
{
  VALVE_CATALYSER = '1',/* VALVE_CATALYSER */
  VALVE_OUTPUT = '0'    /* VALVE_OUTPUT */
}VALVE_ID_E;

/* Output valves possible configurations */
typedef enum
{
  OUTPUT_TO_CATALYSER = 0,
  OUTPUT_TO_EXTERIOR = 1,
  OUTPUT_TO_BOTH = 2,
  OUTPUT_TO_MAX
}OUTPUT_POSITION_E;

/* Valves position */
typedef enum
{
  VALVE_CLOSE = 0,
  VALVE_OPEN = 1
}VALVE_STATE_E;

/* Fan enumeration */
typedef enum
{
  FAN_0 = '0',
  FAN_1 = '1'
}FAN_ID_E;

/* Fan possible states. */
typedef enum
{
  FAN_OFF = 0,
  FAN_ON = 1
}FAN_STATE_E;

/* Flow running state. */
typedef enum
{
  FLOW_ALREADY_STARTED  = 1,
  FLOW_NOT_STARTED_YET = 0
}FLOW_RUNNING_STATE_E;

/* O3 generation modes. */
typedef enum
{
  O3_GENERATION_BASED_ON_TUBE_CALIBRATION = 0,
  O3_GENERATION_BASED_ON_O3_PHOTOSENSOR = 1,
  O3_GENERATION_BASED_ON_MAX
}GEN_MODE_E;

/* O3 sensor tunning modes. */
typedef enum
{
  TUNE_O3_SENSOR_LED = 0,
  TUNE_O3_SENSOR_LED_AND_START_LED_PID = 1,
}TUNE_O3_MODE_E;

/* Vacuum possible states. */
typedef enum
{
  VACUUM_OFF = 0,
  VACUUM_ON  = 1
}VACUUM_STATE_E;

/* Vacuum device enumeration */
typedef enum
{
  VACUUM_0 = '0',
  VACUUM_1 = '1'
}VACUUM_ID_E;

/* Vacuum possible logical states. */
typedef enum
{
  VACUUM_STATE_STOP = 0,
  VACUUM_STATE_RUNNING  = 1,
  VACUUM_STATE_PAUSE = 2
}VACUUM_LOGICAL_STATE_E;

/* Saline cycle description. */
typedef enum
{
  SALINE_MIXING_CYCLE = 0,
  SALINE_REINFUSION_CYCLE  = 1,
}SALINE_CYCLE_E;

/* Control boards allowed. */
typedef enum
{
  A10068_XX_X = 0, // OZT
  A10026_XX_X = 1, // OZP
  A40170_XX_X = 2, // V3
}CONTROL_BOARD_TYPES_E;

/* O3 sensors allowed. */
typedef enum
{
  NO_O3_SENSOR = 0,
  AS03170 = 1
}O3_SENSOR_TYPES_E;

/* Transformers allowed. */
typedef enum
{
  A9462_03 = 0, // OZT
  A9462_04 = 1, // OZP
  A9462_09 = 2	// V3
}TRANSFORMER_TYPES_E;

/* O3 tubes allowed. */
typedef enum
{
  A10021_02 = 0, // OZT
  A10055_01 = 1  // OZP
}O3_TUBE_TYPES_E;

/* Vacuum pump allowed. */
typedef enum
{
  NO_VACUUM_PUMP = 0,      // OZT
  _52402054 = 1, // OZP
  CC02111 = 2    // V3
}VACUMM_PUMP_TYPES_E;

/*LED strip allowed. */
typedef enum
{
  NO_LED_STRIP = 0,       // OZT - OZP
  A40236_XX_X = 1 // V3
}LED_STRIP_TYPES_E;

/* Communication channel allowed. */
typedef enum
{
  SERIAL = 0,
  I2C = 1
}COMM_CHANNEL_TYPES_E;

/* Communication channel allowed. */
typedef enum
{
	START_UP_LOG_TEST = 0,
	START_UP_LOG_TEST_REF = 0,
	START_UP_LOG_SENSOR_OFFSET,
	START_UP_LOG_O3_DIRECT,
	START_UP_LOG_PRESSURE,
	START_UP_LOG_TEMPERATURE
}START_UP_LOG_ITEMS_E;

typedef enum
{
    D_LEV_0 = 0x01,
    D_LEV_1 = 0x02,
    D_LEV_2 = 0x04,
    D_LEV_3 = 0x08,
    D_LEV_4 = 0x10,
    D_LEV_5 = 0x20,
    D_LEV_HIDE_TIME_STAMP = 0x40,
    D_LEV_SEND_WELCOME = 0x80,
    D_LEV_ALL = 0xBE,
    D_LEV_NONE = 0x00,
}DEBUG_LEVEL_T;

/* HW configuration */
typedef struct
{
  uint8 CBoard;
  uint8 O3Sensor;
  uint8 Transformer;
  uint8 O3Tube;
  uint8 VPump;
  uint8 LEDStrip;
  uint8 Comm;
}HW_CONFIG_T;

typedef struct
{
    uint8 userGenerationMode;
    uint8 rfu_0;
    uint8 rfu_1;
} USR_CONFIG_T;

/* FSM O3 events structure */
typedef struct
{
  uint8 Enter;
  uint8 Cancel;
  uint8 Generic;
}FSM_O3_EVENT_T;

//TODO: review if still necessary
typedef struct
{
    int16 X1;
    int16 X2;
    int16 Y1;
    int16 Y2;
}SOLVE_LINE_T;

typedef struct
{
    int16 R1;
    int16 R2;
}CALIBRATION_REF_T;


// TODO: reconsider definition location
/*
 * FSM O3 context structure.
 * It has all the context and variables involved in the implementation of the application Finite State Machine.
 */
typedef struct
{
    FSM_O3_STATE_T *LastState;
    FSM_O3_STATE_T *CurrentState;
    FSM_O3_STATE_T  States[FSM_O3_STATE_MAX];
    HW_CONFIG_T HwConfig;
    USR_CONFIG_T UsrConfig;
    GEN_MODE_E GenerationMode;
    OPERATION_MODE_E Option;
    TIMER_CTX_T AnimationTimer;
    TIMER_CTX_T KeepAliveTimer;
    TIMER_CTX_T DataReadTimer;
    TIMER_CTX_T AdjustTimer;
    TIMER_CTX_T WaitForServiceTimer;
    TIMER_CTX_T HihgVoltDisconectTimer;
    TIMER_CTX_T CleanGeneratorTimer;
    TIMER_CTX_T AuxTimer;
    SYRINGE_CTX_T SyringeCtrl;
    int8  DebLevel;
    int8  (*StartStorageSave)(void);
    int8  (*StartStorageLoad)(void);
    int8  (*WriteStorageLine)(uint16 storageId, int32 value);
    int8  (*ReadStorageLine)(uint16 storageId, int32 *value);
    void  (*StoptStorage)(void);
    uint8  SharedBuffer[16*16];
//  uint8  SamplePosition;
    uint16 ConfiguredO3Concentration;
    uint16 ConfiguredFlow;
    uint16 ConfiguredTime;
    uint16 ConfiguredVacuumTime;
    uint16 ConfiguredVacuumPressure;
    uint16 ConfiguredDose;
    uint16 ConfiguredPressure;
    uint32 ConfiguredVolume;
    uint8  RemainingHours;
    uint8  RemainingMinutes;
    uint8  RemainingSeconds;
    uint8  FreezeRemainingTime;
    uint8  StableFlow;
    uint8  StableOzone;
    uint16 SensorData[ID_SENSOR_MAX][NUM_SENSOR_SAMPLES];
    uint8  SensorDataIndex;
    uint8  SensorDataFilled;
    uint16 ErrorState; // TODO Change to uint8?
    uint16 CurrentMeanOzone;
    uint16 CurrentMeanFlow;
    uint32 CurrentTotalDose;
    uint32 CurrentOutputVolume;
    uint32 ReferenceOutputVolume;
    uint32 CumulatedOutputVolume;
    uint8  KeyEnterPushed;
    uint16 PressAtm;
    uint16 PressAdjust;                         /* Pressure during adjust process */
    uint8  MaxAllowedFlow;                      /* Maximum allowed flow depending on O3 concentration selected */
    uint8  MaxAllowedTime;
    uint8  TemperatureMonitoring;
    int8   ManageCommandError;                  /* Due to optimize flash space */
    uint8  MaxAvailableFlow;                    /* Maximum available flow depending on O3 input pressure applied */
    uint8  WashingConfiguredTime;
    uint8  WashingSeconds;
    uint8  WashingReturnState;
    uint8  SyringeDetected;                     /* Syringe already detected */
    uint8  CalibrationErrorDuringStartUp;
//    uint8  Starting;  TODO remove when properly tested
    uint16 MaxFlowDeviation;
    uint8  RefreshScreen;  // TODO analize if necessary
    int16  CurrentOperatingPressure;
    uint8  DepressureSeconds;
    uint8  DepressureReturnState;
    uint16 DepressureErrorReturnState; // TODO change to uint8 if ErrorState is changed to uint8
//    uint8  GeneratorCleaned;
    int8   VacuumStatus;
    uint8  SalineCycle;
    uint8  PressureAlertDone;
    uint16 CalibrationValue_0;
    uint16 CalibrationValue_1;
    uint16 CalibrationValue_2;
    uint8  InsufflationState;
    uint8  SyringeManualState;
    int16  MaxCurrent;
    int16  LastModBasePeriod;
    int16  MaxModBasePeriod;
    int16  PressThreshold;
    int16  DentalSupplyDetectionPressure;
    uint8  DentalSupplying;

}FSM_O3_OPERATION_T;


// Perform a software reset
void SoftwareReset(void);

// Activate beep sound
void Beep(void);

// Print welcome message via UART
void WelcomeMessage(void);

// Resolve generation mode based on user and hw configuration
void ResolveGenerationMode(void);

// Manage protocol command and response
int8 ManageCommand(PROTOCOL_COMMAND_T *Command, PROTOCOL_RESPONSE_T *Response);

// Clear all error states
void ClearErrors(void);

// Send reset command to hardware
void SendReset(void);

// Send stop command to generator and flow
void SendStopCommand(void);

// Start generator sequence with modulation parameters
//int8 SendGeneratorStartSequence(uint16 ModBasePeriod, uint16 ModBaseWith, uint16 ModPeriod, uint16 ModWith);

// Send generation commands for ozone and flow
void SendGenerationCommands(int16 GenerationMode, uint8 FlowRunningState);

// Switch output valves to specified position
void SwitchOutputValves(OUTPUT_POSITION_E OutputPosition);

uint8 DepressureNeededOnError(uint8 CurrentState, uint16 ErrorState);
// Go to depressure state
/*void GotoDepressure(void);*/
void GotoDepressure(uint8 ReturnState);

// Initial check step 1 for startup
void InitialCheck_1(void);

// Debug function to read O3 sensor LED
void DebugO3SensorTune(void);

// Initialize and run initial check step 2 FSM
void InitialCheck_2_Ini(void);

// Run initial check step 2 FSM
void InitialCheck_2(void);

// FSM for initial check step 2
void InitialCheck_2_FSM(uint8 init);

// Initial check step 3 for startup
void InitialCheck_3(void);

// Go to initial check state
void GotoInitialCheck(void);

// Go to service state
void GotoService(void);

// Start O3 calibration for OZP and V3S device
void GotoCalibrateO3forOZP_V3S_Start(void);

// End O3 calibration for OZP device
void GotoCalibrateO3_OZP_V3S_End(void);

// Go to error state
//void GotoError(void);
void GotoError(uint16 ErrorState);

// Go to washing state
//void GotoWashing(void);
void GotoWashing(uint8 ReturnState);
//void WashingIni(void);

// Go to generating state
void GotoGenerating(void);

// Go to end operation state
void GotoEndOperation(void);

// Go to overpressure state
void GotoOverPressure(void);

// Go to vacuum generation state
void GotoVacuumGeneration(void);

// Go to stuff connected state
void GotoStuffConnected(void);

// Get cumulated output volume
uint32 GetCumulatedVolume(void);

// Set fan depending on temperature
void SetFanDependingOnTemperature(uint16 Temperature);

// Update washing process
void UpdateWashing(void);

// Update depressure process
void UpdateDepressure(void);

// Calculate integer square root
int SquareRoot(int32 Dato);

// Compute flow slope for calibration
int16 ComputeFlowSlope(int16 P1, int16 P2, int16 Offset, int16 CurrentFlow);

// Test current start for OZT device
//void TestCurrentStart_for_OZT(void);
//void TestCurrentStart_for_V3(void);

// Test current start
void TestCurrentStart(void);

// Calibration test for current
//void CalibrationTestCurrent(void);

// Save parameters to flash
void SaveParameters(void);

// Load parameters from flash
void LoadParameters(void);

// Initialize system
void Initialize(void);

// Get instant absolute pressure
int16 GetInstantAbsolutePressure(uint16 PressAtm);

// Manage FSM events
void ManageFsmEvents(void);

// Send keep alive command
void KeepAlive(void);

// Process sensor data
void SensorProcess(void);

// Compute mean values for sensors
void SensorMeanComputation(void);

// Check flow stability for OZP, V3S, V3 devices
void CheckFlowStability_OZP_V3S_V3(void);

// Check flow stability for OZT device
void CheckFlowStabilityOZT(void);

// Check flow stability (unified)
void CheckFlowStability(void);

// Check ozone stability for OZP, V3S, V3 devices
void CheckOzoneStability_OZP_V3S_V3(void);

// Check ozone stability for OZT device
void CheckOzoneStabilityOZT(void);

// Check ozone stability (unified)
void CheckOzoneStability(void);

// Update process state
void UpdateProcess(void);

// Skip adjusting phase
void SkipAdjusting(void);

// Process FSM events
void FSM_ProcessEvents(void);

// Compute saline mixing time
uint32 ComputeSalineTime(uint16 Flow);

// Reset timing variables
void ResetTiming(void);

// Reset cumulative values
void ResetCumulativeValues(void);

// Initialize generation initial settings
void StartGenerationInitialSettings(void);

// Initialize generation process
void StartGeneration(uint8 FlowRunningState);

// Key on push for open saline mode
void KeyOnPushForSaline(void);

// Key on push for open bag mode
void KeyOnPushForOpenBag(void);

// Key on push for closed bag mode
void KeyOnPushForClosedBag(void);

// Key on push for insufflation mode
void KeyOnPushForInsufflation(void);

// Key on push for manual mode
void KeyOnPushForManual(void);

// Key on push for syringe manual mode
void KeyOnPushForSyringeManual(void);

// Release pressure during generation
void ReleasePressureDuringGeneration(void);

// Pause and release pressure
void PauseAndReleasePressure(void);

// Update generating state for dental mode
void UpdateGeneratingForDental(void);

// Update generating state for syringe mode
void UpdateGeneratingForSyringe(void);

// Get pressure threshold for current mode
uint16 GetPressureThreshold(void);

// Update generating state
void UpdateGenerating(void);

// Detect false flow situation
void DetectFalseFlowSituation(void);

// Detect high voltage disconnection
void DetectHighVoltageDisconnection(void);

// Tune sensor LED for calibration
//uint8 TuneSensorLED(int16 Mode);

// Start generator cleaning process
void GeneratorCleanStart(int16 CleanFlow, uint32 CleanTime);

// Stop generator cleaning process
//void GeneratorCleanStop(void);
/*
// Update zero adjusting step 1
void ZeroAdjustingUpdate_1(void);

// Update zero adjusting step 2
void ZeroAdjustingUpdate_2(void);

// Update zero adjusting step 3
void ZeroAdjustingUpdate_3(void);
*/
void O3SensorTune_FSM(uint8 init);
void O3SensorTune_Ini(void);
void O3SensorTune(void);



// End vacuum by pressure
void EndVacuumPressure(void);

// End vacuum by time
void EndVacuumTime(void);

// Cancel vacuum process
void CancelVacuum(void);

// Pause vacuum process
void PauseVacuum(void);

// Skip vacuum process
void SkipVacuum(void);

// Modify vacuum process
void ModifyVacuum(void);

// Update vacuum by pressure
void UpdateVacuumByPressure(void);

// Update vacuum by time
void UpdateVacuumBytime(void);

// Update vacuum process
void UpdateVacuum(void);

// Set vacuum state
void SetVacuum(int8 State);

// Set vacuum state and output valves
void SetVacuumState(int8 State);

// Start vacuum generation
void StartVacuumGeneration(void);

// Update therapy time
void UpdateTherapyTime(void);

// Cancel therapy time
void CancelTherapyTime(void);

// Reset operation mode option
void ResetOption(void);


#ifdef __cplusplus
extern "C" {
#endif
void WashingIni(void);

// Dummy delegate function
void DelegateDummy(void);

// Dummy delegate function
int8 DelegateDummy2(void);

// Dummy delegate function
int8 DelegateDummy3(uint16 a, int32 b);

// Dummy delegate function
int8 DelegateDummy4(uint16 a, int32 *b);

// Update second selection value
void UpdateSecondSelectorValue(void);

// Refresh generator version string
void refreshGeneratorVersion(void);

// Get pressure sensor value
int16 getPressureSensor(void);

// Get temperature sensor value
int16 getTemperatureSensor(void);

// Refresh parameters string
void refreshParameters(void);

// Refresh startup info string
void refreshStartupInfo(void);

// Go to period calibration state
void GotoCalibratePeriod(void);

// Go to save parameters state
void GotoSaveParameters(void);

// Go to load parameters state
void GotoLoadParameters(void);

// Go to flow calibration state
void GotoCalibrateFlow(void);

// Go to O3 calibration state
void GotoCalibrateO3(void);

// Start generator for O3 calibration
void StartForO3Calibration(void);

// Go to pressure compensation factor adjustment state
void GotoAdjustPressCompFactor(void);

// Init pressure calibration
void CalibratePressureInit(void);

// Calibration step 1 for pressure
void CalibratePressureStep1(void);

// Calibration step 2 for pressure
void CalibratePressureStep2(void);

// Calibration process for pressure
void CalibratePressure(uint8 step);

// Calibration step 1 OK for flow
void CalibrateFlowStep1_Ok(void);

// Calibration step 1 value for flow
void CalibrateFlowStep1_Value(void);

// Calibration step 2 for flow
void CalibrateFlowStep2(void);

// Calibration step 1 OK for O3
void CalibrateO3_Ref_1_Ok(void);

// Calibration step 1 OK end for O3
void CalibrateO3Step1_Ok_End(void);

// Calibration step 1 value for O3
void CalibrateO3_Ref_1_Value(void);

// Calibration step 2 OK for O3
void CalibrateO3_Ref_2_Ok(void);

// Calibration step 2 value for O3
void CalibrateO3_Ref_2_Value(void);

// Calibration step 3 value for O3
void CalibrateO3_Ref_3_Value(void);

// Calibration step 3 for O3
void CalibrateO3_Ref_3_Ok(void);

// O3 sensor calibration end procedure
void O3SensorCalibrationEnd(void);
// Change current FSM state
//void ChangeCurrentState(uint16 NewState);

// Launch therapy process
void LaunchTherapy(void);

// Exit error state
void ExitError(void);

// Go to user canceled state
void GotoUserCancelled(void);

// Cancel washing process
void CancelWashing(void);

// Extend washing time
void WashingExtesion(void);

// Go to repose state
void GotoRepose(void);
void WaitingForProtocol(void);

// General key on push handler
void KeyOnPush(void);

// General key on release handler
void KeyOnRelease(void);

// Set maximum allowed flow
void SetMaxAllowedFlow(void);

// Set maximum allowed time
void SetMaxAllowedTime(void);


// Convert therapy option to internal mode
OPERATION_MODE_E ConvertTherapyOption(OPERATION_MODE_E Option);







void StartResonanceFrequencySearch(void);
int16 MeasureCurrent(void);
void UpdateResonanceFrequencySearch(void);
void CalibratePeriodUpdate(void);
void SendGeneratorStartSequence(uint16 ModBasePeriod, uint16 ModWith, uint16 DeadTime);

void SkipErrors(void);
void RestoreErrors(void);

void StopManualGap(void);
void StartManualGap(void);

void TestIni(void);
void deb_printf(int8 deb_level, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* FSM_H_ */
