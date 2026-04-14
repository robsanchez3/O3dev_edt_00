/*
 * fsm_o3_types.h
 *
 * Public types shared between the O3 library and its consumers.
 *
 * This file exists to break the dependency on fsm_o3_operation.h (a heavy
 * internal header) from external code. Any type that must be visible outside
 * Drivers/O3 belongs here; all other types remain in fsm_o3_operation.h.
 *
 * External code should include fsm_o3_api.h, which includes this file.
 * Direct inclusion of fsm_o3_types.h from outside Drivers/O3 is discouraged.
 */
#ifndef FSM_O3_TYPES_H_
#define FSM_O3_TYPES_H_

#include "../Typedef.h"

#define O3_LIB_VERSION  "V0.R0.P0_a"

/* FSM state identifiers */
typedef enum
{
  STATE_INIT = 0,
  STATE_ERROR,
  STATE_INIT_CHECK_1,
  STATE_INIT_CHECK_2,
  STATE_INIT_CHECK_3,
  STATE_WAITING_FOR_PROTOCOL,
  STATE_TUNING_O3_SENSOR,
  STATE_ADJUSTING,
  STATE_O3_GENERATING,
  STATE_USER_CANCELLED,
  STATE_COMPLETED,
  STATE_OVERPRESSURE,
  STATE_VACUUM_GENERATING,
  STATE_WAITING_THERAPY_TIME,
  STATE_WAITING_EXTERNAL_STUFF,
  STATE_WASHING,
  STATE_DEPRESSURE,
  STATE_WAITING_FOR_SERVICE,
  STATE_CALIBRATE_O3_1,
  STATE_CALIBRATE_O3_2,
  STATE_CALIBRATE_O3_3,
  STATE_SELECT_PRESS_COMP_FACTOR,
  STATE_CALIBRATE_PERIOD,
  STATE_SAVING_PARAMETERS,
  STATE_LOADING_PARAMETERS,
  STATE_CALIBRATION_END,
  FSM_O3_STATE_MAX
}FSM_O3_STATE_ID_E;

/* Application error codes */
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

/* FSM O3 Operation modes */
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

  NO_MODE              = 30,
  SERVICE_MODE         = 100,
}OPERATION_MODE_E;

/* Control boards allowed */
typedef enum
{
  A10068_XX_X = 0, // OZT
  A10026_XX_X = 1, // OZP
  A40170_XX_X = 2, // V3
}CONTROL_BOARD_TYPES_E;

/* O3 sensors allowed */
typedef enum
{
  NO_O3_SENSOR = 0,
  AS03170 = 1
}O3_SENSOR_TYPES_E;

/* Transformers allowed */
typedef enum
{
  A9462_03 = 0, // OZT
  A9462_04 = 1, // OZP
  A9462_09 = 2  // V3
}TRANSFORMER_TYPES_E;

/* O3 tubes allowed */
typedef enum
{
  A10021_02 = 0, // OZT
  A10055_01 = 1  // OZP
}O3_TUBE_TYPES_E;

/* Vacuum pumps allowed */
typedef enum
{
  NO_VACUUM_PUMP = 0, // OZT
  _52402054 = 1,      // OZP
  CC02111 = 2         // V3
}VACUMM_PUMP_TYPES_E;

/* LED strips allowed */
typedef enum
{
  NO_LED_STRIP = 0,   // OZT - OZP
  A40236_XX_X = 1     // V3
}LED_STRIP_TYPES_E;

/* O3 generation modes */
typedef enum
{
  O3_GENERATION_BASED_ON_TUBE_CALIBRATION = 0,
  O3_GENERATION_BASED_ON_O3_PHOTOSENSOR = 1,
  O3_GENERATION_BASED_ON_MAX
}GEN_MODE_E;

/* Communication channels allowed */
typedef enum
{
  SERIAL = 0,
  I2C = 1
}COMM_CHANNEL_TYPES_E;

/* Vacuum logical states */
typedef enum
{
  VACUUM_STATE_STOP = 0,
  VACUUM_STATE_RUNNING = 1,
  VACUUM_STATE_PAUSE = 2
}VACUUM_LOGICAL_STATE_E;

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

/* User configuration */
typedef struct
{
    uint8 userGenerationMode;
    uint8 rfu_0;
    uint8 rfu_1;
} USR_CONFIG_T;

#endif /* FSM_O3_TYPES_H_ */
