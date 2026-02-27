/*
 * Protocol.c
 *
 *  Created on:
 *      Author:
 */

//#include <Dependencies/Com_o3.h>
#include "../Dependencies/Com_o3.h"
//#include <Protocol/Protocol.h>
#include "Protocol.h"
//#include "../OZTDUart.h"                 // TODO......  improve path
#include "../Fsm_o3/fsm_o3_operation.h"  // Necessary for FSM_O3_OPERATION_T needed for debugging

/**
 * @brief String to store the last received command response.
 */
static int8 LastResponseString[MAX_RESPONSE_LENGTH];


/**
 * @brief State of the CRC checking.
 */
static CRC_STATE_E CrcState = CRC_ACTIVATED;
/**
 * @brief Timer object for transmission timeout control
 */
static TIMER_CTX_T TimeOutTimer;


extern uint32 GLB_TickCounter;  // TODO study if it is posiible to remove this dependence

void Protocol_SetCRCStatus(CRC_STATE_E Status)
{
  CrcState = Status;
}

CRC_STATE_E Protocol_GetCRCStatus(void)
{
  return CrcState;
}

static void ClearLastResponseString(void)
{
  memset(LastResponseString,0,MAX_RESPONSE_LENGTH);
}

int8 Protocol_ComputeCRC(int8 * Message, int8 * CrcString)
{
  uint8 i = 0;
  uint16 Crc = 0;
  if ( (Message == NULL) || (CrcString == NULL) )
      return PROTOCOL_RET_ERROR_BAD_PARAMETER;

  do
  {
    Crc ^= Message[i];
  }while(Message[i++] != 0);
  sprintf(CrcString, "%02X",Crc);
  return PROTOCOL_RET_SUCCESS;
}

/**
 * @brief CheckCRC
 *
 * @param \b [in] \b CommandString
 *
 * @return \li TRUE If the command CRC is the same as the calculated
 *         \li FALSE If not.
 */
uint8 CheckCRC(int8 *CommandString)
{
  int8 *PointerToCRCInCommandString = strchr(CommandString,'\r') -2;
  int8 *CRC_Calculation_Pointer = CommandString;
  int8 CalculatedCrcString[PROTOCOL_CRC_SIZE];
  int8 Crc=0;

  do
  {
    Crc ^= *CRC_Calculation_Pointer;
  }while(++CRC_Calculation_Pointer < PointerToCRCInCommandString);

  snprintf(CalculatedCrcString, 5, "%02X", (uint8)Crc);

  if ( (CalculatedCrcString[0] == PointerToCRCInCommandString[0]) && (CalculatedCrcString[1] == PointerToCRCInCommandString[1]) )
    return TRUE;
  else
    return FALSE;
}

int8 Protocol_CompoundMessage(PROTOCOL_COMMAND_T * Command,int8 * Message)
{
  int8 Crc[PROTOCOL_CRC_SIZE]={0,0,0};
  int8 HexValue[MAX_HEX_VALUE_SIZE]={0,0,0,0,0};

   if (Command == NULL)
     return PROTOCOL_RET_ERROR_BAD_PARAMETER;

   strncpy(Message, STRING_CMD_HEADER,strlen(STRING_CMD_HEADER) + 1);
   switch (Command->Command)
   {
     case CMD_TEST_COM:
       strncat(Message,STRING_CMD_TEST_COM,strlen(STRING_CMD_TEST_COM) + 1);
       break;
     case CMD_CRC_SET:
       strncat(Message,STRING_CMD_CRC_SET,sizeof(STRING_CMD_CRC_SET));
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,1);
       break;
     case CMD_SET_VALVE:
       strncat(Message,STRING_CMD_SET_VALVE,sizeof(STRING_CMD_SET_VALVE));
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       deb_printf(D_LEV_4, "Set valve %d to %d...\n", Command->Identifier[0] - 48, Command->Value);
       break;
     case CMD_GET_VALVE:
       strncat(Message,STRING_CMD_GET_VALVE,sizeof(STRING_CMD_GET_VALVE));
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,1);
       break;
     case CMD_SET_FAN:
       strncat(Message,STRING_CMD_SET_FAN,sizeof(STRING_CMD_SET_FAN));
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_GET_FAN:
       strncat(Message,STRING_CMD_GET_FAN,sizeof(STRING_CMD_GET_FAN));
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,1);
       break;
     case CMD_SET_VACCUM:
       strncat(Message,STRING_CMD_SET_VACUUM,strlen(STRING_CMD_SET_VACUUM) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_GET_VACCUM:
       strncat(Message,STRING_CMD_GET_VACUUM,strlen(STRING_CMD_GET_VACUUM) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,1);
       break;
     case CMD_SET_SENSOR_SLOPE:
       strncat(Message,STRING_CMD_SET_SENSOR_SLOPE,sizeof(STRING_CMD_SET_SENSOR_SLOPE));
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%X", (uint8)Command->Position[0]);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_GET_SENSOR_SLOPE:
       strncat(Message,STRING_CMD_GET_SENSOR_SLOPE,sizeof(STRING_CMD_GET_SENSOR_SLOPE));
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,1);
       snprintf(HexValue, 5, "%X", (uint8)Command->Position[0]);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       break;
     case CMD_SET_SENSOR_OFFSET:
       strncat(Message,STRING_CMD_SET_SENSOR_OFFSET,sizeof(STRING_CMD_SET_SENSOR_OFFSET));
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%X", (uint8)Command->Position[0]);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_GET_SENSOR_OFFSET:
       strncat(Message,STRING_CMD_GET_SENSOR_OFFSET,sizeof(STRING_CMD_GET_SENSOR_OFFSET));
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,1);
       snprintf(HexValue, 5, "%X", (uint8)Command->Position[0]);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       break;
     case CMD_GET_SENSOR:
       strncat(Message,STRING_CMD_GET_SENSOR,strlen(STRING_CMD_GET_SENSOR) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,1);
       break;
     case CMD_GET_ALL_SENSORS:
       strncat(Message,STRING_CMD_GET_ALL_SENSORS,strlen(STRING_CMD_GET_ALL_SENSORS) + 1);
       break;
     case CMD_SET_MODULATOR_BASE_WIDTH:
       strncat(Message,STRING_CMD_SET_MODULATOR_BASE_WIDTH,strlen(STRING_CMD_SET_MODULATOR_BASE_WIDTH) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_SET_MODULATOR_BASE_PERIOD:
       strncat(Message,STRING_CMD_SET_MODULATOR_BASE_PERIOD,strlen(STRING_CMD_SET_MODULATOR_BASE_PERIOD) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_SET_MODULATOR_WIDTH:
       strncat(Message,STRING_CMD_SET_MODULATOR_WIDTH,sizeof(STRING_CMD_SET_MODULATOR_WIDTH));
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_SET_MODULATOR_PERIOD:
       strncat(Message,STRING_CMD_SET_MODULATOR_PERIOD,sizeof(STRING_CMD_SET_MODULATOR_PERIOD));
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_SET_MODULATOR_DEADTIME:
       strncat(Message,STRING_CMD_SET_MODULATOR_DEADTIME,sizeof(STRING_CMD_SET_MODULATOR_DEADTIME));
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%02X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_GET_MODULATOR_BASE_WIDTH:
       strncat(Message,STRING_CMD_GET_MODULATOR_BASE_WIDTH,strlen(STRING_CMD_GET_MODULATOR_BASE_WIDTH) + 1);
       break;
     case CMD_GET_MODULATOR_BASE_PERIOD:
       strncat(Message,STRING_CMD_GET_MODULATOR_BASE_PERIOD,strlen(STRING_CMD_GET_MODULATOR_BASE_PERIOD) + 1);
       break;
     case CMD_GET_MODULATOR_WIDTH:
       strncat(Message,STRING_CMD_GET_MODULATOR_WIDTH,sizeof(STRING_CMD_GET_MODULATOR_WIDTH));
       break;
     case CMD_GET_MODULATOR_PERIOD:
       strncat(Message,STRING_CMD_GET_MODULATOR_PERIOD,sizeof(STRING_CMD_GET_MODULATOR_PERIOD));
       break;
     case CMD_MODULATOR_BASE_START:
       strncat(Message,STRING_CMD_MODULATOR_BASE_START,strlen(STRING_CMD_MODULATOR_BASE_START) + 1);
       break;
     case CMD_MODULATOR_BASE_STOP:
       strncat(Message,STRING_CMD_MODULATOR_BASE_STOP,strlen(STRING_CMD_MODULATOR_BASE_STOP) + 1);
       break;
     case CMD_MODULATOR_START:
       strncat(Message,STRING_CMD_MODULATOR_START,strlen(STRING_CMD_MODULATOR_START) + 1);
       break;
     case CMD_MODULATOR_STOP:
       strncat(Message,STRING_CMD_MODULATOR_STOP,strlen(STRING_CMD_MODULATOR_STOP) + 1);
       break;
     case CMD_START_O3_SENSOR_CAL:
       strncat(Message,STRING_CMD_START_O3_SENSOR_CAL,strlen(STRING_CMD_START_O3_SENSOR_CAL) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_SET_OZONE_TARGET:
       strncat(Message,STRING_CMD_SET_OZONE_TARGET,sizeof(STRING_CMD_SET_OZONE_TARGET));
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_GET_OZONE_TARGET:
       strncat(Message,STRING_CMD_GET_OZONE_TARGET,strlen(STRING_CMD_GET_OZONE_TARGET) + 1);
       break;
     case CMD_START_OZONE:
//       strncat(Message,STRING_CMD_START_OZONE,strlen(STRING_CMD_START_OZONE) + 1);



       strncat(Message,STRING_CMD_START_OZONE,sizeof(STRING_CMD_START_OZONE));
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);




       break;
     case CMD_STOP_OZONE:
       strncat(Message,STRING_CMD_STOP_OZONE,strlen(STRING_CMD_STOP_OZONE) + 1);
       break;
     case CMD_STATE_OZONE:
       strncat(Message,STRING_CMD_GET_OZONE_STATE,strlen(STRING_CMD_GET_OZONE_STATE) + 1);
       break;
     case CMD_SET_FLOW_TARGET:
       strncat(Message,STRING_CMD_SET_FLOW_TARGET,strlen(STRING_CMD_SET_FLOW_TARGET) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_GET_FLOW_TARGET:
       strncat(Message,STRING_CMD_GET_FLOW_TARGET,strlen(STRING_CMD_GET_FLOW_TARGET) + 1);
       break;
     case CMD_START_FLOW:
       strncat(Message,STRING_CMD_START_FLOW,strlen(STRING_CMD_START_FLOW) + 1);
       break;
     case CMD_STOP_FLOW:
       strncat(Message,STRING_CMD_STOP_FLOW,strlen(STRING_CMD_STOP_FLOW) + 1);
       break;
     case CMD_STATE_FLOW:
       strncat(Message,STRING_CMD_GET_FLOW_STATE,strlen(STRING_CMD_GET_FLOW_STATE) + 1);
       break;
     case CMD_STATE_O3_SENSOR_CAL:
       strncat(Message,STRING_CMD_STATE_O3_SENSOR_CAL,strlen(STRING_CMD_STATE_O3_SENSOR_CAL) + 1);
       break;
     case CMD_SET_PID_P_PARAM:
       strncat(Message,STRING_CMD_SET_PID_P_PARAM,strlen(STRING_CMD_SET_PID_P_PARAM) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,IDENTIFIER_LENGTH);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_SET_PID_I_PARAM:
       strncat(Message,STRING_CMD_SET_PID_I_PARAM,strlen(STRING_CMD_SET_PID_I_PARAM) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,IDENTIFIER_LENGTH);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_SET_PID_D_PARAM:
       strncat(Message,STRING_CMD_SET_PID_D_PARAM,strlen(STRING_CMD_SET_PID_D_PARAM) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,IDENTIFIER_LENGTH);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_GET_PID_P_PARAM:
       strncat(Message,STRING_CMD_GET_PID_P_PARAM,strlen(STRING_CMD_GET_PID_P_PARAM) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,IDENTIFIER_LENGTH);
       break;
     case CMD_GET_PID_I_PARAM:
       strncat(Message,STRING_CMD_GET_PID_I_PARAM,strlen(STRING_CMD_GET_PID_I_PARAM) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,IDENTIFIER_LENGTH);
       break;
     case CMD_GET_PID_D_PARAM:
       strncat(Message,STRING_CMD_GET_PID_D_PARAM,strlen(STRING_CMD_GET_PID_D_PARAM) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,IDENTIFIER_LENGTH);
       break;
     case CMD_SET_PID_P_GAIN:
       strncat(Message,STRING_CMD_SET_PID_P_GAIN,strlen(STRING_CMD_SET_PID_P_GAIN) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,IDENTIFIER_LENGTH);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_SET_PID_I_GAIN:
       strncat(Message,STRING_CMD_SET_PID_I_GAIN,strlen(STRING_CMD_SET_PID_I_GAIN) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,IDENTIFIER_LENGTH);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_SET_PID_D_GAIN:
       strncat(Message,STRING_CMD_SET_PID_D_GAIN,strlen(STRING_CMD_SET_PID_D_GAIN) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,IDENTIFIER_LENGTH);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_GET_PID_P_GAIN:
       strncat(Message,STRING_CMD_GET_PID_P_GAIN,strlen(STRING_CMD_GET_PID_P_GAIN) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,IDENTIFIER_LENGTH);
       break;
     case CMD_GET_PID_I_GAIN:
       strncat(Message,STRING_CMD_GET_PID_I_GAIN,strlen(STRING_CMD_GET_PID_I_GAIN) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,IDENTIFIER_LENGTH);
       break;
     case CMD_GET_PID_D_GAIN:
       strncat(Message,STRING_CMD_GET_PID_D_GAIN,strlen(STRING_CMD_GET_PID_D_GAIN) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,IDENTIFIER_LENGTH);
       break;
     case CMD_RESET_PID:
       strncat(Message,STRING_CMD_RESET_PID,strlen(STRING_CMD_RESET_PID) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,IDENTIFIER_LENGTH);
       break;
     case CMD_SET_TABLE_POSITION:
       strncat(Message,STRING_CMD_SET_TABLE_POSITION,strlen(STRING_CMD_SET_TABLE_POSITION) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,IDENTIFIER_LENGTH);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Position[0]);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Position[1]);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_GET_TABLE_POSITION:
       strncat(Message,STRING_CMD_GET_TABLE_POSITION,strlen(STRING_CMD_GET_TABLE_POSITION) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,IDENTIFIER_LENGTH);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Position[0]);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Position[1]);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_CHECK_TABLE_CRC:
       strncat(Message,STRING_CMD_CHECK_TABLE_CRC,strlen(STRING_CMD_CHECK_TABLE_CRC) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,1);
       break;
     case CMD_CLEAR_FIRST_ERROR:
       strncat(Message,STRING_CMD_CLEAR_FIRST_ERROR_QUEUE,strlen(STRING_CMD_CLEAR_FIRST_ERROR_QUEUE) + 1);
       break;
     case CMD_CLEAR_ALL_ERROR:
       strncat(Message,STRING_CMD_CLEAR_ALL_ERROR,strlen(STRING_CMD_CLEAR_ALL_ERROR) + 1);
       break;
     case CMD_SKIP_ERROR:
       strncat(Message,STRING_CMD_SKIP_ERROR,strlen(STRING_CMD_SKIP_ERROR) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,1);
       break;
     case CMD_RESET:
       strncat(Message,STRING_CMD_RESET,strlen(STRING_CMD_RESET) + 1);
       break;
     case CMD_GET_SW_VERSION:
       strncat(Message,STRING_CMD_GET_SW_VERSION,strlen(STRING_CMD_GET_SW_VERSION) + 1);
       break;
     case CMD_WATCHDOG_SET:
       strncat(Message,STRING_CMD_SET_WATCHDOG,strlen(STRING_CMD_SET_WATCHDOG) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_REMOTE_CONTROL_RET_STATE:
       strncat(Message,STRING_CMD_REMOTE_CONTROL_RET_STATE,strlen(STRING_CMD_REMOTE_CONTROL_RET_STATE) + 1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_SET_PROPORTIONAL_VALVE:
       strncat(Message,STRING_CMD_SET_PROP_VALVE,sizeof(STRING_CMD_SET_PROP_VALVE));
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       strncat(Message,Command->Identifier,1);
       strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
       snprintf(HexValue, 5, "%04X", (uint16)Command->Value);
       strncat(Message,HexValue,MAX_HEX_VALUE_SIZE);
       break;
     case CMD_GET_PROPORTIONAL_VALVE:
       strncat(Message,STRING_CMD_GET_PROP_VALVE,sizeof(STRING_CMD_GET_PROP_VALVE));
       break;
     default:
       return PROTOCOL_RET_BAD_COMMAND;
       break;
   }

   strncat(Message,STRING_CMD_SEPARATOR,strlen(STRING_CMD_SEPARATOR) + 1);
   Protocol_ComputeCRC(Message,Crc);

   strncat(Message,Crc,PROTOCOL_CRC_SIZE);
   strncat(Message,STRING_CMD_TAIL,strlen(STRING_CMD_TAIL) + 1);
   return PROTOCOL_RET_SUCCESS;
}

extern FSM_O3_OPERATION_T GLB_fsm_o3;

uint8 FsmStateForDebug(void)
{
	return (GLB_fsm_o3.CurrentState == GLB_fsm_o3.LastState) ? GLB_fsm_o3.CurrentState->State_ID : GLB_fsm_o3.LastState->State_ID;
}

/**
 * @brief Protocol_SendCommand
 *
 * @param [in] Command Structure with necessary information to compound a command message.
 *
 * @return \li PROTOCOL_RET_SUCCESS
 *         \li PROTOCOL_RET_COMMAND_NOT_SENT
 */
int8 Protocol_SendCommand(PROTOCOL_COMMAND_T * Command)
{
  int8 Message[PROTOCOL_MAX_MESSAGE_SIZE];
  int8 RetVal=0;
  uint8 NumRetries = 0;

  TIMER_Init(&TimeOutTimer,&GLB_TickCounter);

  RetVal = Protocol_CompoundMessage(Command,Message);

  if (RetVal != PROTOCOL_RET_SUCCESS)
  {
    return RetVal;
  }
  else
  {
    do
    {
      COM_O3_PutString((uint8_t *)Message);
      deb_printf(D_LEV_0, "%s\n", Message); // SWV debug
      deb_printf(D_LEV_1, "Send(%02d) -> %s\n", (GLB_fsm_o3.CurrentState == GLB_fsm_o3.LastState) ? GLB_fsm_o3.CurrentState->State_ID : GLB_fsm_o3.LastState->State_ID, Message); // SWV debug
      RetVal = Protocol_GetResponse();
    }while ( (RetVal != CMD_RESPONSE_ACK) && (RetVal != CMD_RESPONSE_KEY_SIMULATION) && (++NumRetries < 3) );

    /* If CMD_CRC_SET command is not acknowledged, leave CRC status as it was */
    if ( (Command->Command == CMD_CRC_SET) && (RetVal != CMD_RESPONSE_ACK) )
    {
      CrcState = !CrcState;
    }
    if ( (RetVal == CMD_RESPONSE_ACK) || (RetVal == CMD_RESPONSE_KEY_SIMULATION) )
    {
      return PROTOCOL_RET_SUCCESS;
    }
    else
    {
      return PROTOCOL_RET_COMMAND_NOT_SENT;
    }
  }
}

int8 Protocol_GetResponse(void)
{
  uint8 i=0;
  int8 RetVal=CMD_RESPONSE_NONE;

  ClearLastResponseString();
/*TIMER_Start(&TimeOutTimer,MS_TO_CLOCK(150));*/

  TIMER_Start(&TimeOutTimer,60000);
  do
  {
	if(COM_O3_DataAvailable())
	{
      COM_O3_GetChar((uint8_t *) &LastResponseString[i]);

      if (LastResponseString[i] == '\r')
      {
        break;
      }
      else if (LastResponseString[i] != 0)
      {
        if (i < MAX_RESPONSE_LENGTH)
        {
          i++;
        }
      }
	}
    if (TIMER_State(&TimeOutTimer) == TIMER_STATE_EXPIRED)
    {
    	deb_printf(D_LEV_0, "Serial time out\n"); // SWV debug
    	deb_printf(D_LEV_1, "Serial time out\n"); // SWV debug
        return PROTOCOL_RET_TIMEOUT;
    }
  }while (1);

  deb_printf(D_LEV_0, "%s\n", LastResponseString); // SWV debug
  deb_printf(D_LEV_1, "Rec (%02d) -> %s\n", (GLB_fsm_o3.CurrentState == GLB_fsm_o3.LastState) ? GLB_fsm_o3.CurrentState->State_ID : GLB_fsm_o3.LastState->State_ID, LastResponseString); // SWV debug

  if ( (CrcState == CRC_ACTIVATED) && !CheckCRC(LastResponseString) )
  {
    RetVal = CMD_RESPONSE_NAK;
  }
  else
  {
    if (LastResponseString[RESPONSE_ACK_NAK_OFFSET] == CODE_ACK)
    {
      RetVal = CMD_RESPONSE_ACK;
    }
    else
    {
      RetVal = CMD_RESPONSE_NAK;
    }
  }
  if ( (RetVal != CMD_RESPONSE_ACK) && (RetVal != CMD_RESPONSE_KEY_SIMULATION) )
  {
    ClearLastResponseString();
  }
  return RetVal;
}

static void ResolveStatus(uint8 ReceivedStatus, PROTOCOL_RESPONSE_T* Response)
{
  if( ((ReceivedStatus & WARNING_MASK) == 0) && (ReceivedStatus > CTRL_BOARD_CAL_ERROR) )
  {
    Response->Status = CTRL_BOARD_COMS_ERROR;
  }
  else
  {
    Response->Status = ReceivedStatus;
  }
}

void Protocol_GetLastResponse(PROTOCOL_RESPONSE_T * Response)
{
  int8 * pParser = NULL;
  int8* rest = LastResponseString;


  memset(Response,0,sizeof(PROTOCOL_RESPONSE_T));

  if (LastResponseString[0] == '\0')
    Response->Ack_Nak = CMD_RESPONSE_NONE;
  else
  {
    pParser = strtok_r(rest,",",&rest) + 1;
    /*case NAK*/
    if (!strcmp(pParser,STRING_CMD_RESPONSE_NAK))
    {
      Response->Ack_Nak = CMD_RESPONSE_NAK;
      pParser = strtok_r(NULL,",",&rest);
      Response->Ack_Nak_Info.NakCode = *(pParser) - 0x30;
    }
    /* case ACK */
    else if (!strcmp(pParser,STRING_CMD_RESPONSE_ACK))
    {
      Response->Ack_Nak = CMD_RESPONSE_ACK;
      pParser = strtok_r(NULL,",",&rest);

      /* CMD_TEST_COM */
      if (!strcmp(pParser,STRING_CMD_TEST_COM))
      {
        Response->Ack_Nak_Info.Command = CMD_TEST_COM;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_CRC_SET */
      else if  (!strcmp(pParser,STRING_CMD_CRC_SET))
      {
        Response->Ack_Nak_Info.Command = CMD_CRC_SET;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_SET_VALVE */
      else if  (!strcmp(pParser,STRING_CMD_SET_VALVE))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_VALVE;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_GET_VALVE */
      else if  (!strcmp(pParser,STRING_CMD_GET_VALVE))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_VALVE;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);

        pParser = strtok_r(NULL, ",", &rest);
        Response->Data[0] = *(pParser) - 0x30;
      }
      /* CMD_SET_FAN */
      else if  (!strcmp(pParser,STRING_CMD_SET_FAN))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_FAN;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);

      }
      /* CMD_GET_FAN */
      else if  (!strcmp(pParser,STRING_CMD_GET_FAN))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_FAN;
        pParser = strtok_r(NULL, ",",&rest);
         ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",", &rest);
        Response->Data[0] = *(pParser) - 0x30;
      }

      /* CMD_SET_VACUUM */
      else if  (!strcmp(pParser,STRING_CMD_SET_VACUUM))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_VACCUM;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_GET_VACUUM */
      else if  (!strcmp(pParser,STRING_CMD_GET_VACUUM))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_VACCUM;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",", &rest);
        Response->Data[0] = *(pParser) - 0x30;
      }
      /* CMD_SET_SENSOR_SLOPE */
      else if  (!strcmp(pParser,STRING_CMD_SET_SENSOR_SLOPE))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_SENSOR_SLOPE;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /*CMD_GET_SENSOR_SLOPE*/
      else if  (!strcmp(pParser,STRING_CMD_GET_SENSOR_SLOPE))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_SENSOR_SLOPE;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }
      /* CMD_SET_SENSOR_OFFSET */
      else if  (!strcmp(pParser,STRING_CMD_SET_SENSOR_OFFSET))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_SENSOR_OFFSET;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /*CMD_GET_SENSOR_SLOPE*/
      else if  (!strcmp(pParser,STRING_CMD_GET_SENSOR_OFFSET))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_SENSOR_OFFSET;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }
      /*CMD_GET_SENSOR*/
      else if  (!strcmp(pParser,STRING_CMD_GET_SENSOR))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_SENSOR;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);

        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);

        pParser = pParser +1;
        rest = rest +1;
      }
      /*CMD_SET_MODULATOR_BASE_WIDTH*/
      else if  (!strcmp(pParser,STRING_CMD_SET_MODULATOR_BASE_WIDTH))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_MODULATOR_BASE_WIDTH;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /*CMD_SET_MODULATOR_BASE_PERIOD*/
      else if  (!strcmp(pParser,STRING_CMD_SET_MODULATOR_BASE_PERIOD))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_MODULATOR_BASE_PERIOD;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /*CMD_SET_MODULATOR_WIDTH*/
      else if  (!strcmp(pParser,STRING_CMD_SET_MODULATOR_WIDTH))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_MODULATOR_WIDTH;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /*CMD_SET_MODULATOR_BASE_PERIOD*/
      else if  (!strcmp(pParser,STRING_CMD_SET_MODULATOR_PERIOD))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_MODULATOR_PERIOD;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      else if  (!strcmp(pParser,STRING_CMD_SET_MODULATOR_DEADTIME))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_MODULATOR_DEADTIME;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /*CMD_GET_MODULATOR_BASE_WIDTH*/
      else if  (!strcmp(pParser,STRING_CMD_GET_MODULATOR_BASE_WIDTH))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_MODULATOR_BASE_WIDTH;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }
      /*CMD_GET_MODULATOR_BASE_PERIOD*/
      else if  (!strcmp(pParser,STRING_CMD_GET_MODULATOR_BASE_PERIOD))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_MODULATOR_BASE_PERIOD;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }
      /*CMD_GET_MODULATOR_WIDTH*/
      else if  (!strcmp(pParser,STRING_CMD_GET_MODULATOR_WIDTH))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_MODULATOR_WIDTH;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }
      /*CMD_GET_MODULATOR_BASE_PERIOD*/
      else if  (!strcmp(pParser,STRING_CMD_GET_MODULATOR_PERIOD))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_MODULATOR_PERIOD;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }
      /* CMD_MODULATOR_BASE_START */
      else if  (!strcmp(pParser,STRING_CMD_MODULATOR_BASE_START))
      {
        Response->Ack_Nak_Info.Command = CMD_MODULATOR_BASE_START;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_MODULATOR_BASE_STOP */
      else if  (!strcmp(pParser,STRING_CMD_MODULATOR_BASE_STOP))
      {
        Response->Ack_Nak_Info.Command = CMD_MODULATOR_BASE_STOP;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }

      /* CMD_MODULATOR_START */
      else if  (!strcmp(pParser,STRING_CMD_MODULATOR_START))
      {
        Response->Ack_Nak_Info.Command = CMD_MODULATOR_START;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_MODULATOR_STOP */
      else if  (!strcmp(pParser,STRING_CMD_MODULATOR_STOP))
      {
        Response->Ack_Nak_Info.Command = CMD_MODULATOR_STOP;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_START_O3_SENSOR_CAL */
      else if  (!strcmp(pParser,STRING_CMD_START_O3_SENSOR_CAL))
      {
        Response->Ack_Nak_Info.Command = CMD_START_O3_SENSOR_CAL;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_SET_OZONE_TARGET */
      else if  (!strcmp(pParser,STRING_CMD_SET_OZONE_TARGET))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_OZONE_TARGET;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_GET_OZONE_TARGET */
      else if  (!strcmp(pParser,STRING_CMD_GET_OZONE_TARGET))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_OZONE_TARGET;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }
      /* CMD_START_OZONE */
      else if  (!strcmp(pParser,STRING_CMD_START_OZONE))
      {
        Response->Ack_Nak_Info.Command = CMD_START_OZONE;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_STOP_OZONE */
      else if  (!strcmp(pParser,STRING_CMD_STOP_OZONE))
      {
        Response->Ack_Nak_Info.Command = CMD_STOP_OZONE;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_STATE_OZONE */
      else if  (!strcmp(pParser,STRING_CMD_GET_OZONE_STATE))
      {
        Response->Ack_Nak_Info.Command = CMD_STATE_OZONE;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",", &rest);
        Response->Data[0] = *(pParser) - 0x30;
      }
      /* CMD_SET_FLOW_TARGET */
      else if  (!strcmp(pParser,STRING_CMD_SET_FLOW_TARGET))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_FLOW_TARGET;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_GET_FLOW_TARGET */
      else if  (!strcmp(pParser,STRING_CMD_GET_FLOW_TARGET))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_FLOW_TARGET;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);

        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }
      /* CMD_START_FLOW */
      else if  (!strcmp(pParser,STRING_CMD_START_FLOW))
      {
        Response->Ack_Nak_Info.Command = CMD_START_FLOW;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_STOP_FLOW */
      else if  (!strcmp(pParser,STRING_CMD_STOP_FLOW))
      {
        Response->Ack_Nak_Info.Command = CMD_STOP_FLOW;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
    }
      /* CMD_STATE_FLOW */
      else if  (!strcmp(pParser,STRING_CMD_GET_FLOW_STATE))
      {
        Response->Ack_Nak_Info.Command = CMD_STATE_FLOW;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",", &rest);
        Response->Data[0] = *(pParser) - 0x30;
      }
      /* CMD_STATE_O3_SENSOR_CAL */
      else if  (!strcmp(pParser,STRING_CMD_STATE_O3_SENSOR_CAL))
      {
        Response->Ack_Nak_Info.Command = CMD_STATE_O3_SENSOR_CAL;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",", &rest);
        Response->Data[0] = *(pParser) - 0x30;
      }
      /* CMD_SET_PID_P_PARAM */
      else if  (!strcmp(pParser,STRING_CMD_SET_PID_P_PARAM))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_PID_P_PARAM;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_SET_PID_I_PARAM */
      else if  (!strcmp(pParser,STRING_CMD_SET_PID_I_PARAM))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_PID_I_PARAM;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_SET_PID_D_PARAM */
      else if  (!strcmp(pParser,STRING_CMD_SET_PID_D_PARAM))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_PID_D_PARAM;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_GET_PID_P_PARAM */
      else if  (!strcmp(pParser,STRING_CMD_GET_PID_P_PARAM))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_PID_P_PARAM;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }
      /* CMD_GET_PID_I_PARAM */
      else if  (!strcmp(pParser,STRING_CMD_GET_PID_I_PARAM))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_PID_I_PARAM;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }
      /* CMD_GET_PID_D_PARAM */
      else if  (!strcmp(pParser,STRING_CMD_GET_PID_D_PARAM))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_PID_D_PARAM;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }

      /* CMD_SET_PID_P_GAIN */
      else if  (!strcmp(pParser,STRING_CMD_SET_PID_P_GAIN))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_PID_P_GAIN;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_SET_PID_I_GAIN */
      else if  (!strcmp(pParser,STRING_CMD_SET_PID_I_GAIN))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_PID_I_GAIN;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_SET_PID_D_GAIN */
      else if  (!strcmp(pParser,STRING_CMD_SET_PID_D_GAIN))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_PID_D_GAIN;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_GET_PID_P_GAIN */
      else if  (!strcmp(pParser,STRING_CMD_GET_PID_P_GAIN))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_PID_P_GAIN;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }
      /* CMD_GET_PID_I_GAIN */
      else if  (!strcmp(pParser,STRING_CMD_GET_PID_I_GAIN))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_PID_I_GAIN;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }
      /* CMD_GET_PID_D_GAIN */
      else if  (!strcmp(pParser,STRING_CMD_GET_PID_D_GAIN))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_PID_D_GAIN;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }
      /* CMD_RESET_PID */
      else if  (!strcmp(pParser,STRING_CMD_RESET_PID))
      {
        Response->Ack_Nak_Info.Command = CMD_RESET_PID;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_SET_TABLE_POSITION */
      else if  (!strcmp(pParser,STRING_CMD_SET_TABLE_POSITION))
      {
        Response->Ack_Nak_Info.Command = CMD_SET_TABLE_POSITION;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_GET_TABLE_POSITION */
      else if  (!strcmp(pParser,STRING_CMD_GET_TABLE_POSITION))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_TABLE_POSITION;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }
      /* CMD_CHECK_TABLE_CRC */
      else if  (!strcmp(pParser,STRING_CMD_CHECK_TABLE_CRC))
      {
        Response->Ack_Nak_Info.Command = CMD_CHECK_TABLE_CRC;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",", &rest);
        Response->Data[0] = *(pParser) - 0x30;
      }

      /* CMD_WATCHDOG_SET */
      else if  (!strcmp(pParser,STRING_CMD_SET_WATCHDOG))
      {
        Response->Ack_Nak_Info.Command = CMD_WATCHDOG_SET;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_CLEAR_FIRST_ERROR */
      else if  (!strcmp(pParser,STRING_CMD_CLEAR_FIRST_ERROR_QUEUE))
      {
        Response->Ack_Nak_Info.Command = CMD_CLEAR_FIRST_ERROR;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_CLEAR_ALL_ERROR */
      else if  (!strcmp(pParser,STRING_CMD_CLEAR_ALL_ERROR))
      {
        Response->Ack_Nak_Info.Command = CMD_CLEAR_ALL_ERROR;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_SKIP_ERROR */
      else if  (!strcmp(pParser,STRING_CMD_SKIP_ERROR))
      {
        Response->Ack_Nak_Info.Command = CMD_SKIP_ERROR;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_RESET */
      else if  (!strcmp(pParser,STRING_CMD_RESET))
      {
        Response->Ack_Nak_Info.Command = CMD_RESET;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_GET_SW_VERSION */
      else if  (!strcmp(pParser,STRING_CMD_GET_SW_VERSION))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_SW_VERSION;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        memcpy((void *)(Response->Data),pParser,8);
      }
      /* CMD_GET_ALL_SENSORS */
      else if  (!strcmp(pParser,STRING_CMD_GET_ALL_SENSORS))
      {
//    	  uint8 i = 0;
    	  Response->Ack_Nak_Info.Command = CMD_GET_ALL_SENSORS;
    	  pParser = strtok_r(NULL, ",",&rest);
    	  ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);

//    	  while( (pParser = strtok_r(NULL, ",",&rest)) != NULL )
//    	  {
//    		  Response->Data[i++] = (int16)(uint16)strtoul(pParser, NULL, 16);
//    	  }
    	  // Thread-safe (without strtok_r)
    	  // Make a local copy of the string to avoid affecting other threads
    	  char local_buffer[64];
    	  strncpy(local_buffer, rest, sizeof(local_buffer) - 1);
    	  local_buffer[sizeof(local_buffer) - 1] = '\0';

    	  sscanf(local_buffer, "%hx,%hx,%hx,%hx,%hx,%hx,%hx,%hx", &Response->Data[0], &Response->Data[1], &Response->Data[2], &Response->Data[3],
    			                                                  &Response->Data[4], &Response->Data[5], &Response->Data[6], &Response->Data[7]);
      }
      /* CMD_REMOTE_CONTROL_KEY_CANCEL */
      if (!strcmp(pParser,STRING_CMD_REMOTE_CONTROL_KEY_CANCEL))
      {
        Response->Ack_Nak_Info.Command = CMD_REMOTE_CONTROL_KEY_CANCEL;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_REMOTE_CONTROL_KEY_ENTER */
      else if  (!strcmp(pParser,STRING_CMD_REMOTE_CONTROL_KEY_ENTER))
      {
        Response->Ack_Nak_Info.Command = CMD_REMOTE_CONTROL_KEY_ENTER;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_REMOTE_CONTROL_KEY_ENTER_RELEASED */
      else if  (!strcmp(pParser,STRING_CMD_REMOTE_CONTROL_KEY_ENTER_RELASED))
      {
        Response->Ack_Nak_Info.Command = CMD_REMOTE_CONTROL_KEY_ENTER_RELEASED;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_REMOTE_CONTROL_SLIDER */
      else if  (!strcmp(pParser,STRING_CMD_REMOTE_CONTROL_SLIDER))
      {
        Response->Ack_Nak_Info.Command = CMD_REMOTE_CONTROL_SLIDER;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }
      /* CMD_REMOTE_CONTROL_GET_STATE */
      else if  (!strcmp(pParser,STRING_CMD_REMOTE_CONTROL_GET_STATE))
      {
        Response->Ack_Nak_Info.Command = CMD_REMOTE_CONTROL_GET_STATE;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_REMOTE_CONTROL_RET_STATE */
      else if  (!strcmp(pParser,STRING_CMD_REMOTE_CONTROL_RET_STATE))
      {
        Response->Ack_Nak_Info.Command = CMD_REMOTE_CONTROL_RET_STATE;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
      }
      /* CMD_GET_PROPORTIONAL_VALVE */
      else if  (!strcmp(pParser,STRING_CMD_GET_PROP_VALVE))
      {
        Response->Ack_Nak_Info.Command = CMD_GET_PROPORTIONAL_VALVE;
        pParser = strtok_r(NULL, ",",&rest);
        ResolveStatus((uint8)strtoul(pParser, NULL, 16), Response);
        pParser = strtok_r(NULL, ",",&rest);
        Response->Data[0] = (int16)(uint16)strtoul(pParser, NULL, 16);
      }

    }
  }
  ClearLastResponseString();
}


