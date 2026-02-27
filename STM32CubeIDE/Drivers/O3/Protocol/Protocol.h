/*!
 * \file        Protocol.h
 * \brief       This module implements the Master side of the communication protocol between control
 * and interface boards for Ozone systems.
 *
 * \author      Fernando Alcojor & Roberto Sanchez.
 * \version
 * \htmlonly
 *              <A HREF="path-to-tag">TAG</A>
 * \endhtmlonly
 * \date        20/03/2014.
 * \remarks
 * \attention
 * \warning
 * \copyright   SEDECAL S.A.
 * \defgroup    Protocol
 * @{
 * @brief       This module implements the Serial communication protocol master side for communicating
 *              with the slave side.
 *              It let's you send and receive commands, get the last response to a command and Enable/Disable the CRC check in the protocol.
 *              It also implements some internal functions.
 */


#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdio.h>
#include <string.h>
#include "../Typedef.h"
//#include "../Utils/Utils.h"
#include "../Timer/Timer.h"

/**
 * Maximum size for command messages.
 */
#define PROTOCOL_MAX_MESSAGE_SIZE       30
/**
 * Size of buffer for CRC compound
 */
#define PROTOCOL_CRC_SIZE               3
/**
 * Maximum size for integer to hexadecimal conversion buffer.
 */
#define MAX_HEX_VALUE_SIZE              5
/**
 * Maximum for number field of command structure.
 */
#define MAX_NUMBER_IN_COMMAND           0x5A


/**
 * Identifier length
 */
#define IDENTIFIER_LENGTH               2
/**
 * Maximum number of bytes in command response.
 */
#define MAX_RESPONSE_LENGTH             80


#define MAX_RESPONSE_DATA_FIELDS        50

/**
 * Maximum number of retries for a command.
 */
#define NUM_MAX_RETRIES                 3

/**
 * Response string offset for command code
 */
#define RESPONSE_CMD_OFFSET             3

/**
 * Command string offset for CRC status.
 */
#define CRC_STATE_OFFSET                5
/**
 * Response string offset for system error code
 */
#define FROMTAIL_ERRORCODE_OFFSET       4
/**
 * Response string offset for data information
 */
#define FROMTAIL_DATA_OFFSET


/**
 * Response string offset for ACK or NAK
 */
#define RESPONSE_ACK_NAK_OFFSET         1


/**
 * valve and fan states.
 */
#define ON      1
#define OFF     0
/**
 *
 * Command strings
 */
#define STRING_CMD_HEADER      "{"
#define STRING_CMD_SEPARATOR    ","
#define STRING_CMD_TAIL        "\r"

#define STRING_CMD_TEST_COM    "*"
/**
 * Valve commands
 * @{
 */
#define STRING_CMD_SET_VALVE   "EVS"
#define STRING_CMD_GET_VALVE   "EVG"
/**@}*/
/**
 * Fan commands
 * @{
 */
#define STRING_CMD_SET_FAN     "AFS"
#define STRING_CMD_GET_FAN     "AFG"
/**@}*/

/**
 * Vacuum commands
 * @{
 */
#define STRING_CMD_SET_VACUUM   "VS"
#define STRING_CMD_GET_VACUUM   "VG"
/**@}*/

/**
 * Proportional ElectroValve commands
 * @{
 */
#define STRING_CMD_SET_PROP_VALVE       "EPS"
#define STRING_CMD_GET_PROP_VALVE       "EPG"
/**@}*/

/**
 * SENSOR commands
 * @{
 */
#define STRING_CMD_SET_SENSOR_SLOPE     "SSS"
#define STRING_CMD_GET_SENSOR_SLOPE     "SGS"
#define STRING_CMD_SET_SENSOR_OFFSET    "SSO"
#define STRING_CMD_GET_SENSOR_OFFSET    "SGO"
#define STRING_CMD_GET_SENSOR           "SG"
#define STRING_CMD_GET_ALL_SENSORS      "SGA"
/**@}*/

/**
 * Modulator commands
 * @{
 */
#define STRING_CMD_SET_MODULATOR_BASE_WIDTH    "MSBW"
#define STRING_CMD_SET_MODULATOR_BASE_PERIOD   "MSBP"
#define STRING_CMD_SET_MODULATOR_WIDTH         "MSW"
#define STRING_CMD_SET_MODULATOR_PERIOD        "MSP"
#define STRING_CMD_SET_MODULATOR_DEADTIME      "MSDT"
#define STRING_CMD_GET_MODULATOR_BASE_WIDTH    "MGBW"
#define STRING_CMD_GET_MODULATOR_BASE_PERIOD   "MGBP"
#define STRING_CMD_GET_MODULATOR_WIDTH         "MGW"
#define STRING_CMD_GET_MODULATOR_PERIOD        "MGP"
#define STRING_CMD_MODULATOR_BASE_START        "MBSTR"
#define STRING_CMD_MODULATOR_BASE_STOP         "MBSTP"
#define STRING_CMD_MODULATOR_START             "MSTR"
#define STRING_CMD_MODULATOR_STOP              "MSTP"
/**@}*/

/**
 * Ozone commands
 * @{
 */
#define STRING_CMD_SET_OZONE_TARGET            "O3S"
#define STRING_CMD_GET_OZONE_TARGET            "O3G"
#define STRING_CMD_GET_OZONE_STATE             "O3STT"
#define STRING_CMD_START_OZONE                 "O3STR"
#define STRING_CMD_STOP_OZONE                  "O3STP"
/**
 * Ozone sensor calibration commands
 * @{
 */
#define STRING_CMD_START_O3_SENSOR_CAL         "O3SSTR"
#define STRING_CMD_STATE_O3_SENSOR_CAL         "O3SSTT"
#define STRING_CMD_STOP_O3_SENSOR_CAL          "O3SSTP"
/**@}*/

/**
 * Flow commands
 * @{
 */
#define STRING_CMD_SET_FLOW_TARGET             "FS"
#define STRING_CMD_GET_FLOW_TARGET             "FG"
#define STRING_CMD_GET_FLOW_STATE              "FSTT"
#define STRING_CMD_START_FLOW                  "FSTR"
#define STRING_CMD_STOP_FLOW                   "FSTP"
/**@}*/

/**
 * PID commands
 * @{
 */
#define STRING_CMD_SET_PID_P_PARAM             "PSPP"
#define STRING_CMD_SET_PID_P_GAIN              "PSPG"
#define STRING_CMD_SET_PID_I_PARAM             "PSIP"
#define STRING_CMD_SET_PID_I_GAIN              "PSIG"
#define STRING_CMD_SET_PID_D_PARAM             "PSDP"
#define STRING_CMD_SET_PID_D_GAIN              "PSDG"
#define STRING_CMD_GET_PID_P_PARAM             "PGPP"
#define STRING_CMD_GET_PID_P_GAIN              "PGPG"
#define STRING_CMD_GET_PID_I_PARAM             "PGIP"
#define STRING_CMD_GET_PID_I_GAIN              "PGIG"
#define STRING_CMD_GET_PID_D_PARAM             "PGDP"
#define STRING_CMD_GET_PID_D_GAIN              "PGDG"

#define STRING_CMD_SET_PID_INTEGRAL_MAX        "PSIMAX"
#define STRING_CMD_SET_PID_INTEGRAL_MIN        "PSIMIN"
#define STRING_CMD_GET_PID_INTEGRAL_MAX        "PGIMAX"
#define STRING_CMD_GET_PID_INTEGRAL_MIN        "PGIMIN"

#define STRING_CMD_RESET_PID                   "PRST"

#define STRING_CMD_RESET                       "RST"

/**@}*/

/**
 * Table commands
 * @{
 */
#define STRING_CMD_SET_TABLE_POSITION          "TS"
#define STRING_CMD_GET_TABLE_POSITION          "TG"
#define STRING_CMD_CHECK_TABLE_CRC             "TCRC"
/**@}*/

/**
 * Key simulation commands
 * @{
*/
#define STRING_CMD_REMOTE_CONTROL_KEY_CANCEL            "RCCNL"
#define STRING_CMD_REMOTE_CONTROL_KEY_ENTER             "RCENT"
#define STRING_CMD_REMOTE_CONTROL_KEY_ENTER_RELASED     "RCENTR"
#define STRING_CMD_REMOTE_CONTROL_SLIDER                "RCSLD"
#define STRING_CMD_REMOTE_CONTROL_GET_STATE             "RCGET"
#define STRING_CMD_REMOTE_CONTROL_RET_STATE             "RCRET"
/**@}*/




/**
 * SENSOR Section definitions
 * @{
 */
#define SECTION_0                            0
#define SECTION_1                            1
#define SECTION_2                            2
/**@}*/


/**
 * Table constants
 * @{
 */
#define TABLE_INITIALIZATION_STATE                   0
#define BASE_PERIOD_MAX_VALUE_TABLE_POSITION         3
#define BASE_PERIOD_MIN_VALUE_TABLE_POSITION         4
#define BASE_WIDTH_INI_TABLE_POSITION                5
#define BASE_PERIOD_INI_VALUE_TABLE_POSITION         6
#define PERIOD_INI_VALUE_TABLE_POSITION              8
#define WIDTH_INI_VALUE_TABLE_POSITION               7
#define PRESS_0_GAIN_SEC1_TABLE_POSITION             66
#define PRESS_0_GAIN_SEC2_TABLE_POSITION             67
#define PRESS_0_GAIN_SEC3_TABLE_POSITION             68
#define PRESS_1_GAIN_SEC1_TABLE_POSITION             69
#define PRESS_1_GAIN_SEC2_TABLE_POSITION             70
#define PRESS_1_GAIN_SEC3_TABLE_POSITION             71
#define PRESS_0_OFFSET_SEC1_TABLE_POSITION           90 /* 87 */
#define PRESS_0_OFFSET_SEC2_TABLE_POSITION           91 /* 88 */
#define PRESS_0_OFFSET_SEC3_TABLE_POSITION           92 /* 89 */
#define PRESS_1_OFFSET_SEC1_TABLE_POSITION           93 /* 90 */
#define PRESS_1_OFFSET_SEC2_TABLE_POSITION           94 /* 91 */
#define PRESS_1_OFFSET_SEC3_TABLE_POSITION           95 /* 92 */

#define FLOW_GAIN_SEC1_TABLE_POSITION                72
#define FLOW_GAIN_SEC2_TABLE_POSITION                73
#define FLOW_GAIN_SEC3_TABLE_POSITION                74
#define FLOW_OFFSET_SEC1_TABLE_POSITION              96  /* 93  */
#define FLOW_OFFSET_SEC2_TABLE_POSITION              97  /* 94  */
#define FLOW_OFFSET_SEC3_TABLE_POSITION              98  /* 95  */
#define FLOW_LIMIT_SEC1_TABLE_POSITION               120 /* 114 */
#define FLOW_LIMIT_SEC2_TABLE_POSITION               121 /* 115 */
#define FLOW_LIMIT_SEC3_TABLE_POSITION               122 /* 116 */

#define O3_SENSOR_GAIN_SEC1_TABLE_POSITION           78

#define OZONE_GAIN_SEC1_TABLE_POSITION               180 /* 171 */
#define OZONE_GAIN_SEC2_TABLE_POSITION               181 /* 172 */
#define OZONE_GAIN_SEC3_TABLE_POSITION               182 /* 173 */
#define OZONE_OFFSET_SEC1_TABLE_POSITION             183 /* 174 */
#define OZONE_OFFSET_SEC2_TABLE_POSITION             184 /* 175 */
#define OZONE_OFFSET_SEC3_TABLE_POSITION             185 /* 176 */
#define OZONE_LIMIT_SEC1_TABLE_POSITION              186 /* 177 */
#define OZONE_LIMIT_SEC2_TABLE_POSITION              187 /* 178 */
#define OZONE_LIMIT_SEC3_TABLE_POSITION              188 /* 179 */
#define START_UP_TEST_CURRENT_TABLE_POSITION         189 /* 180 */

#define PULSES_FOR_O3_CALIBRATION_SEC_1_POSITION     190 /* 181 */
#define PULSES_FOR_O3_CALIBRATION_SEC_2_POSITION     191 /* 182 */
#define PULSES_FOR_O3_CALIBRATION_SEC_3_POSITION     192 /* 183 */

#define PRESSURE_COMPENSATION_FACTOR_POSITION        193
#define CALIBRATION_PRESSURE_POSITION                194
#define FLOW_COMP_FOR_PULSES_POSITION                195

#define PULSES_FOR_1_MG_POSITION                     196 /* 183 */

/**@}*/

/**
 * CRC commands
 * @{
 */
#define STRING_CMD_CRC_SET                     "CRC"
/**@}*/

/**
 * Watchdog management commands
 */
#define STRING_CMD_SET_WATCHDOG                "WS"
/**@}*/


/**
 * Error management commands
 * @{
 */
#define STRING_CMD_CLEAR_FIRST_ERROR_QUEUE    "ERCLRF"
#define STRING_CMD_CLEAR_ALL_ERROR            "ERCLR"
#define STRING_CMD_SKIP_ERROR                 "ERSKIP"
/**@}*/

/**
 * Software version command
 */
#define STRING_CMD_GET_SW_VERSION                         "VER"
/**
 * ACK/NAK commands
 * @{
 */
#define STRING_CMD_RESPONSE_ACK                 "@"
#define STRING_CMD_RESPONSE_NAK                 "#"

/**
 * ACK/NAK codes
 * @{
 */
#define CODE_ACK                 '@'
#define CODE_NAK                 '#'

/**
 * Warning mask to distinguish error and warning indications
 * @{
 */
#define WARNING_MASK    0x80
/**@}*/

/**
 * \enum CRC_STATE_E
 * CRC status constants
 */
typedef enum
{
  CRC_DEACTIVATED=1,/*!< CRC_DEACTIVATED  //!< CRC_DEACTIVATED */
  CRC_ACTIVATED=2   /*!< CRC_ACTIVATED    //!< CRC_ACTIVATED */
}CRC_STATE_E;

/**
 * \enum PROTOCOL_RETURN_E
 * Return values for Protocol functions
 */
typedef enum
{
  PROTOCOL_RET_SUCCESS=0,
  PROTOCOL_RET_ERROR_BAD_PARAMETER=-1,
  PROTOCOL_RET_BAD_COMMAND=-2,
  PROTOCOL_RET_COMMAND_NOT_SENT=-3,
  PROTOCOL_RET_TIMEOUT=-4
}PROTOCOL_RETURN_E;

/**
 * \enum PROTOCOL_NAK_ERROR_E
 * Protocol NAK Causes
 *
 */
typedef enum
{
  PROTOCOL_NAK_NO_ERROR = '0',
  PROTOCOL_NAK_ERROR_NO_COMMAND,
  PROTOCOL_NAK_ERROR_UNKNOWN_COMMAND,
  PROTOCOL_NAK_ERROR_KNOWN_COMMAND,
  PROTOCOL_NAK_ERROR_CRC,
  PROTOCOL_NAK_ERROR_START_BYTE,
  PROTOCOL_NAK_ERROR_NO_DELIMITER,
  PROTOCOL_NAK_ERROR_WRONG_PARAMETER_VALUE,
  PROTOCOL_NAK_ERROR_REPORTED_FROM_LOWER_LAYER,
  PROTOCOL_NAK_ERROR_SENSOR_NOT_CALIBRATED,
  PROTOCOL_NAK_PARSE_ERROR_MAX
} PROTOCOL_NAK_ERROR_E;


/**
 * \enum PROTOCOL_COMMANDS_E
 * Protocol Command identifiers
 */
typedef enum
{
  CMD_TEST_COM=1,
  CMD_SET_VALVE,
  CMD_GET_VALVE,
  CMD_SET_FAN,
  CMD_GET_FAN,
  CMD_SET_VACCUM,
  CMD_GET_VACCUM,
  CMD_SET_SENSOR_SLOPE,
  CMD_GET_SENSOR_SLOPE,
  CMD_SET_SENSOR_OFFSET,
  CMD_GET_SENSOR_OFFSET,
  CMD_GET_SENSOR,
  CMD_GET_ALL_SENSORS,
  CMD_SET_MODULATOR_BASE_WIDTH,
  CMD_SET_MODULATOR_BASE_PERIOD,
  CMD_SET_MODULATOR_WIDTH,
  CMD_SET_MODULATOR_PERIOD,
  CMD_SET_MODULATOR_DEADTIME,
  CMD_GET_MODULATOR_BASE_WIDTH,
  CMD_GET_MODULATOR_BASE_PERIOD,
  CMD_GET_MODULATOR_WIDTH,
  CMD_GET_MODULATOR_PERIOD,
  CMD_MODULATOR_BASE_START,
  CMD_MODULATOR_BASE_STOP,
  CMD_MODULATOR_START,
  CMD_MODULATOR_STOP,
  CMD_START_O3_SENSOR_CAL,
  CMD_STOOP_O3_SENSOR_CAL,
  CMD_STATE_O3_SENSOR_CAL,
  CMD_SET_OZONE_TARGET,
  CMD_GET_OZONE_TARGET,
  CMD_START_OZONE,
  CMD_STOP_OZONE,
  CMD_STATE_OZONE,
  CMD_SET_FLOW_TARGET,
  CMD_GET_FLOW_TARGET,
  CMD_START_FLOW,
  CMD_STOP_FLOW,
  CMD_STATE_FLOW,
  CMD_SET_PID_P_PARAM,
  CMD_SET_PID_I_PARAM,
  CMD_SET_PID_D_PARAM,
  CMD_GET_PID_P_PARAM,
  CMD_GET_PID_I_PARAM,
  CMD_GET_PID_D_PARAM,
  CMD_SET_PID_P_GAIN,
  CMD_SET_PID_I_GAIN,
  CMD_SET_PID_D_GAIN,
  CMD_GET_PID_P_GAIN,
  CMD_GET_PID_I_GAIN,
  CMD_GET_PID_D_GAIN,
  CMD_SET_PID_I_MAX,
  CMD_SET_PID_I_MIN,
  CMD_GET_PID_I_MAX,
  CMD_GET_PID_I_MIN,
  CMD_RESET_PID,
  CMD_SET_TABLE_POSITION,
  CMD_GET_TABLE_POSITION,
  CMD_CHECK_TABLE_CRC,
  CMD_CRC_SET,
  CMD_WATCHDOG_SET,
  CMD_CLEAR_FIRST_ERROR,
  CMD_CLEAR_ALL_ERROR,
  CMD_SKIP_ERROR,
  CMD_RESET,
  CMD_GET_SW_VERSION,
  CMD_REMOTE_CONTROL_KEY_CANCEL,
  CMD_REMOTE_CONTROL_KEY_ENTER,
  CMD_REMOTE_CONTROL_KEY_ENTER_RELEASED,
  CMD_REMOTE_CONTROL_SLIDER,
  CMD_REMOTE_CONTROL_GET_STATE,
  CMD_REMOTE_CONTROL_RET_STATE,
  CMD_SET_PROPORTIONAL_VALVE,
  CMD_GET_PROPORTIONAL_VALVE,

  CMD_MAX
}PROTOCOL_COMMANDS_E;

/**
 * \enum PROTOCOL_CMD_RESPONSE_E
 * Protocol command response types.
 */
typedef enum
{
  CMD_RESPONSE_ACK=0,
  CMD_RESPONSE_NAK,
  CMD_RESPONSE_KEY_SIMULATION,
  CMD_RESPONSE_NONE,
  CMD_RESPONSE_MAX
}PROTOCOL_CMD_RESPONSE_E;

/**
 * \enum PROTOCOL_CONTROL_BOARD_STATUS_E
 * Protocol Slave board possible status.
 */
typedef enum
{
  CTRL_BOARD_STATUS_OK         = 0,
  CTRL_BOARD_TEMP_ERROR        = 1,
  CTRL_BOARD_CURRENT_ERROR     = 2,
  CTRL_BOARD_OZONE_ERROR       = 3,
  CTRL_BOARD_FLOW_ERROR        = 4,
  CTRL_BOARD_PRESS_ERROR       = 5,
  CTRL_BOARD_LEAKAGE_ERROR     = 6,
  CTRL_BOARD_PIN_ERROR         = 7,
  CTRL_BOARD_COMS_ERROR        = 8,
  CTRL_BOARD_CAL_ERROR         = 9,
  CTRL_BOARD_TRANSFORMER_ERROR = 10,
  CTRL_BOARD_CTRL_BOARD_STATUS_MAX
}PROTOCOL_CONTROL_BOARD_STATUS_E;

/**
 * \enum PROTOCOL_CONTROL_BOARD_WARNINGS_E
 * Protocol Slave board possible warning status.
 */
typedef enum
{
  CTRL_BOARD_WARNING_NO = 0x80,
  CTRL_BOARD_WARNING_NO_STABY_CURRENT= 0x81
}PROTOCOL_CONTROL_BOARD_WARNINGS_E;

/**
 * \struct PROTOCOL_COMMAND_T
 * Structure that represent a protocol command.
 */
typedef struct
{
    uint8 Command;
    int8  Identifier[2];
    int16 Value;
    uint16 Position[2];
}PROTOCOL_COMMAND_T;

/**
 * \union ACK_NAK_INFO_T
 * Union for managing NAK code.
 */
typedef union
{
    uint8 Command;
    uint8 NakCode;
}ACK_NAK_INFO_T;

/**
 * \struct PROTOCOL_RESPONSE_T
 * Protocol response structure.
 */
typedef struct
{
    uint8 Ack_Nak;
    ACK_NAK_INFO_T Ack_Nak_Info;
    PROTOCOL_CONTROL_BOARD_STATUS_E Status;
    int16 Data[MAX_RESPONSE_DATA_FIELDS];
}PROTOCOL_RESPONSE_T;



/**
 * @brief Protocol_SendCommand
 *
 * @param Command Structure with the command to be sent.
 *
 * @return \li PROTOCOL_RET_SUCCESS if Command was sent and acknowledged successfully.
 *         \li PROTOCOL_BAD_PARAMETER if Command is not a valid command structure or has invalid values.
 */
int8 Protocol_SendCommand(PROTOCOL_COMMAND_T * Command);

/**
 * @brief Protocol_GetResponse
 *
 * @param None
 *
 * @return \li PROTOCOL_RET_SUCCESS If the command is acknowledged
 *         \li PROTOCOL_RET_TIMEOUT If there was no response after NUM_MAX_RETRIES times.
 */
int8 Protocol_GetResponse();



/**
 * @brief Protocol_SetCRCStatus
 *
 * @param \b [in] Status CRC_ACTIVATE or CRC_DEACTIVATE
 *
 * @return no return.
 */

void Protocol_SetCRCStatus(CRC_STATE_E Status);

/**
 * @brief Protocol_GetCRCStatus
 *
 * @return \b [in] Status CRC_ACTIVATE or CRC_DEACTIVATE
 */

CRC_STATE_E Protocol_GetCRCStatus(void);


/**
 * @brief Protocol_ComputeCRC Computes the CRC for the string \b Message.
 *
 * Computes the XOR of all bytes in Message and stores it as hexadecimal ascii characters in CrcString.
 *
 * @param [in] \b Message String to compute CRC from.
 * @param [out] \b CrcString String where CRC is returned
 *
 * @return No return.
 */
int8 Protocol_ComputeCRC(int8 * Message, int8 * CrcString);

/**
 * @brief Protocol_CompoundMessage
 *
 * @param [in] \b Command Structure with the command info.
 * @param [out] \b Message String where compounded message will be stored.
 *
 * @return \li PROTOCOL_RET_ERROR_BAD_PARAMETER if any of the parameters is NULL.
 *         \li PROTOCOL_RET_BAD_COMMAND if the command information is not correct.
 */
int8 Protocol_CompoundMessage(PROTOCOL_COMMAND_T * Command, int8 * Message);


/**
 * @brief GetLastResponse
 *
 * @param \b [out] \b Response Structure where response information will be stored.
 *
 * @return No return
 */
void Protocol_GetLastResponse(PROTOCOL_RESPONSE_T * Response);

/**
 * @}
 */
#endif /* PROTOCOL_H_ */
