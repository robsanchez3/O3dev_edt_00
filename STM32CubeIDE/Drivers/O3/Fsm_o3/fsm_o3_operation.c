/*
Next: (17/10/2025)

Crear el entrono necesario para calibración (OZT, OZP, V3, V3S)
Buscar una manrera secreta de emtran en configuración de fabrica
Plantear la opción de hacer euq esta configuración dependa de un pen drive
salvado de parametros de calibración
compatibilidad e comunicaciones serie o I2C


*/
#include "fsm_o3_operation.h"
#include "stdlib.h"
//#include "../OZTDUart.h"
//#include "OZTDUart.h"
#include "../Dependencies/Com_o3.h"
#include "../Dependencies/Delay_o3.h"
//#include "cmsis_os.h" //  osDelay
#include <stdarg.h>

/* Global variables */
int32 GLB_Time;                   /* Global time variable for the mode execution */
int16 StartUpLog[10];             /* Array to store start up log values          */
CALIBRATION_REF_T CalibrationRef; /* Calibration temporal references             */


// delete definitively when properly tested (21/21/26)
//uint8 IncreaseFlowPrecisionCounter = 0;   /* Since flow PID is slower (due to flow precision increase) it is            */
                                          /* necessary to wait more time to ensure flow target value in some situations */

/* Externals */
extern FSM_O3_EVENT_T     GLB_FsmEvents;
extern FSM_O3_OPERATION_T GLB_fsm_o3;

/* Macros */

#define IS_HW_O3_SENSOR()                    (!(GLB_fsm_o3.HwConfig.O3Sensor == NO_O3_SENSOR))
#define IS_NO_HW_O3_SENSOR()                 (GLB_fsm_o3.HwConfig.O3Sensor == NO_O3_SENSOR)

#define IS_ADJUSTING()                       ((GLB_fsm_o3.CurrentState->State_ID == STATE_ADJUSTING) ? 1: 0)
#define IS_ADJUSTING_EXPIRED()               ((TIMER_State(&GLB_fsm_o3.AdjustTimer) == TIMER_STATE_EXPIRED) ? 1 : 0)
#define IS_CONTINUOUS_END()                  ((GLB_fsm_o3.Option == CONTINUOUS_MODE) && (GLB_fsm_o3.RemainingHours == 0) && (GLB_fsm_o3.RemainingMinutes == 0) && (GLB_fsm_o3.RemainingSeconds == 0))
#define IS_MANUAL_END()                      ((GLB_fsm_o3.Option == MANUAL_MODE) && (GLB_fsm_o3.RemainingHours == 0) && (GLB_fsm_o3.RemainingMinutes == 0) && (GLB_fsm_o3.RemainingSeconds == 0))
#define IS_VACUUM_TIME_END()                 ((GLB_fsm_o3.Option == VACUUM_TIME_MODE) && (GLB_fsm_o3.RemainingHours == 0) && (GLB_fsm_o3.RemainingMinutes == 0) && (GLB_fsm_o3.RemainingSeconds == 0))
#define IS_INSUFFLATION_R_END()              ((GLB_fsm_o3.Option == INSUFFLATION_R_MODE) && (GLB_fsm_o3.InsufflationState == INSUFFLATION_STATE_ON) && (GLB_fsm_o3.CurrentOutputVolume >= GLB_fsm_o3.ConfiguredVolume))
#define IS_INSUFFLATION_V_END()              ((GLB_fsm_o3.Option == INSUFFLATION_V_MODE) && (GLB_fsm_o3.RemainingHours == 0) && (GLB_fsm_o3.RemainingMinutes == 0) && (GLB_fsm_o3.RemainingSeconds == 0))
#define IS_CLOSED_BAG_END_BY_PRESSURE()      ((GLB_fsm_o3.Option == CLOSED_BAG_MODE) && (GLB_fsm_o3.CurrentOutputVolume >= (GLB_fsm_o3.ConfiguredVolume * 1000)))
#define IS_CLOSED_BAG_END_BY_TIME()          ((GLB_fsm_o3.Option == CLOSED_BAG_MODE) && (GLB_fsm_o3.RemainingHours == 0) && (GLB_fsm_o3.RemainingMinutes == 0) && (GLB_fsm_o3.RemainingSeconds == 0))
#define IS_OPEN_BAG_END()                    ((GLB_fsm_o3.Option == OPEN_BAG_MODE) && (GLB_fsm_o3.RemainingHours == 0) && (GLB_fsm_o3.RemainingMinutes == 0) && (GLB_fsm_o3.RemainingSeconds == 0))
#define IS_SALINE_MIXING_END()               ((GLB_fsm_o3.Option == SALINE_MODE) && (GLB_fsm_o3.SalineCycle == SALINE_MIXING_CYCLE) && (GLB_fsm_o3.RemainingHours == 0) && (GLB_fsm_o3.RemainingMinutes == 0) && (GLB_fsm_o3.RemainingSeconds == 0))
#define IS_SALINE_REINFUSION_END()           ((GLB_fsm_o3.Option == SALINE_MODE) && (GLB_fsm_o3.SalineCycle == SALINE_REINFUSION_CYCLE) && (GLB_fsm_o3.RemainingHours == 0) && (GLB_fsm_o3.RemainingMinutes == 0) && (GLB_fsm_o3.RemainingSeconds == 0))
#define IS_DOSE_END()                        ((GLB_fsm_o3.Option == DOSE_MODE) && (GLB_fsm_o3.CurrentTotalDose >= GLB_fsm_o3.ConfiguredDose))
#define IS_INSUFFLATION_R_ON()               ((GLB_fsm_o3.Option == INSUFFLATION_R_MODE) && (GLB_fsm_o3.InsufflationState == INSUFFLATION_STATE_ON))
#define IS_INSUFFLATION_V_ON()               ((GLB_fsm_o3.Option == INSUFFLATION_V_MODE) && (GLB_fsm_o3.InsufflationState == INSUFFLATION_STATE_ON))
#define IS_MANUAL_PUSHED()                   ((GLB_fsm_o3.Option == MANUAL_MODE) && (GLB_fsm_o3.KeyEnterPushed == PUSHED))
#define IS_MANUAL_NOT_PUSHED()               ((GLB_fsm_o3.Option == MANUAL_MODE) && (GLB_fsm_o3.KeyEnterPushed == NOT_PUSHED))
#define IS_NOT_MANUAL_NOR_INSUFFLATION()    !((GLB_fsm_o3.Option == MANUAL_MODE) || (GLB_fsm_o3.Option == INSUFFLATION_R_MODE) || (GLB_fsm_o3.Option == INSUFFLATION_V_MODE))
#define IS_GENERATING()                     (((GLB_fsm_o3.CurrentState->State_ID == STATE_O3_GENERATING) && (GLB_fsm_o3.Option != MANUAL_MODE)) || \
		                                     ((GLB_fsm_o3.CurrentState->State_ID == STATE_O3_GENERATING) && (GLB_fsm_o3.Option == MANUAL_MODE) && (GLB_fsm_o3.KeyEnterPushed == PUSHED)))
//#define IS_GENERATING()                    ((GLB_fsm_o3.CurrentState->State_ID == STATE_O3_GENERATING) ? 1 : 0)
#define IS_NOT_MANUAL_NOR_DENTAL_NOR_INSUFFLATION() !((GLB_fsm_o3.Option == MANUAL_MODE) || (GLB_fsm_o3.Option == DENTAL_MODE) || (GLB_fsm_o3.Option == INSUFFLATION_R_MODE) || (GLB_fsm_o3.Option == INSUFFLATION_V_MODE))

#define IS_SYRINGE_MANUAL_PUSHED()           ((GLB_fsm_o3.Option == MANUAL_MODE) && (GLB_fsm_o3.KeyEnterPushed == PUSHED))
#define IS_SYRINGE_MANUAL_NOT_PUSHED()       ((GLB_fsm_o3.Option == SYRINGE_MANUAL_MODE) && (GLB_fsm_o3.KeyEnterPushed == NOT_PUSHED))

#define IS_INSUFLATION_R_MODE()               (GLB_fsm_o3.Option == INSUFFLATION_R_MODE)
#define IS_INSUFLATION_V_MODE()               (GLB_fsm_o3.Option == INSUFFLATION_V_MODE)
#define IS_INSUFLATION_R_MODE_ON()           ((GLB_fsm_o3.Option == INSUFFLATION_R_MODE) && (GLB_fsm_o3.InsufflationState == INSUFFLATION_STATE_ON))
#define IS_INSUFLATION_MODE_PAUSED()         ((GLB_fsm_o3.Option == INSUFFLATION_R_MODE) && (GLB_fsm_o3.InsufflationState == INSUFFLATION_STATE_PAUSED))

#define IS_DOSE_MODE()	           	 	      (GLB_fsm_o3.Option == DOSE_MODE)
#define IS_SYRINGE_AUTO_MODE()	    		  (GLB_fsm_o3.Option == SYRINGE_AUTO_MODE)
#define IS_SYRINGE_MANUAL_MODE()    		  (GLB_fsm_o3.Option == SYRINGE_MANUAL_MODE)
#define IS_CONTINUOUS_MODE()	   		      (GLB_fsm_o3.Option == CONTINUOUS_MODE)
#define IS_SALINE_MODE()	       		      (GLB_fsm_o3.Option == SALINE_MODE)
#define IS_MANUAL_MODE()	   		          (GLB_fsm_o3.Option == MANUAL_MODE)
#define IS_DENTAL_MODE()	   		          (GLB_fsm_o3.Option == DENTAL_MODE)
#define IS_DENTAL_SUPPLYING()                ((GLB_fsm_o3.Option == DENTAL_MODE) && (GLB_fsm_o3.DentalSupplying == TRUE))
#define IS_VACUUM_TIME_MODE()			      (GLB_fsm_o3.Option == VACUUM_TIME_MODE)
#define IS_VACUUM_PRESSURE_MODE()		      (GLB_fsm_o3.Option == VACUUM_PRESSURE_MODE)
#define IS_OPEN_BAG_MODE()	     	          (GLB_fsm_o3.Option == OPEN_BAG_MODE)
#define IS_CLOSED_BAG_MODE()		          (GLB_fsm_o3.Option == CLOSED_BAG_MODE)
#define IS_CLOSED_BAG_VACUUM_TIME_END()      ((GLB_fsm_o3.Option == CLOSED_BAG_MODE) && (GLB_fsm_o3.RemainingHours == 0) && (GLB_fsm_o3.RemainingMinutes == 0) && (GLB_fsm_o3.RemainingSeconds == 0))

#define IS_CLOSED_BAG_VACUUM()               ((GLB_fsm_o3.Option == CLOSED_BAG_MODE) && (GLB_fsm_o3.CurrentState->State_ID == STATE_VACUUM_GENERATING))
#define IS_CLOSED_BAG_ADJUSTING()            ((GLB_fsm_o3.Option == CLOSED_BAG_MODE) && (GLB_fsm_o3.CurrentState->State_ID == STATE_ADJUSTING))
#define IS_CLOSED_BAG_GENERATING()           ((GLB_fsm_o3.Option == CLOSED_BAG_MODE) && (GLB_fsm_o3.CurrentState->State_ID == STATE_O3_GENERATING))
#define IS_CLOSED_BAG_WAITING_THERAPY_TIME() ((GLB_fsm_o3.Option == CLOSED_BAG_MODE) && (GLB_fsm_o3.CurrentState->State_ID == STATE_WAITING_THERAPY_TIME))
#define IS_CLOSED_BAG_THERAPY_TIME_END()     ((GLB_fsm_o3.Option == CLOSED_BAG_MODE) && (GLB_fsm_o3.RemainingHours == 0) && (GLB_fsm_o3.RemainingMinutes == 0) && (GLB_fsm_o3.RemainingSeconds == 0))
#define IS_CLOSED_BAG_INITIAL_VACUUM()       ((GLB_fsm_o3.Option == CLOSED_BAG_MODE) && (GLB_fsm_o3.CurrentOutputVolume == 0))
#define IS_CLOSED_BAG_FINAL_VACUUM()         ((GLB_fsm_o3.Option == CLOSED_BAG_MODE) && (GLB_fsm_o3.CurrentOutputVolume != 0))

#define IS_OPEN_BAG_VACUUM()                 ((GLB_fsm_o3.Option == OPEN_BAG_MODE) && (GLB_fsm_o3.CurrentState->State_ID == STATE_VACUUM_GENERATING))
#define IS_OPEN_BAG_VACUUM_TIME_END()        ((GLB_fsm_o3.Option == OPEN_BAG_MODE) && (GLB_fsm_o3.RemainingHours == 0) && (GLB_fsm_o3.RemainingMinutes == 0) && (GLB_fsm_o3.RemainingSeconds == 0))

#define IS_SALINE_MIXING_CYCLE()             ((GLB_fsm_o3.Option == SALINE_MODE) && (GLB_fsm_o3.SalineCycle == SALINE_MIXING_CYCLE))
#define IS_SALINE_REINFUSION_CYCLE()         ((GLB_fsm_o3.Option == SALINE_MODE) && (GLB_fsm_o3.SalineCycle == SALINE_REINFUSION_CYCLE))

#define IS_DENTAL_TIMEOUT()                  ((GLB_fsm_o3.Option == DENTAL_MODE) && (GLB_fsm_o3.RemainingHours == 0) && (GLB_fsm_o3.RemainingMinutes == 0) && (GLB_fsm_o3.RemainingSeconds == 0))

#define IS_NORMAL_PROCESSING()		         (TIMER_State(&GLB_fsm_o3.WaitForServiceTimer) == TIMER_STATE_STOP)
#define IS_NOT_ERROR_STATE()                  (GLB_fsm_o3.CurrentState->State_ID != STATE_ERROR)

#define IS_STARTING() 				         ( (GLB_fsm_o3.CurrentState->State_ID == STATE_INIT) || (GLB_fsm_o3.CurrentState->State_ID == STATE_INIT_CHECK_1) || (GLB_fsm_o3.CurrentState->State_ID == STATE_INIT_CHECK_2) || (GLB_fsm_o3.CurrentState->State_ID == STATE_INIT_CHECK_3) )

#define IS_GENERATOR_CLEANED()			     (TIMER_State(&GLB_fsm_o3.CleanGeneratorTimer) != TIMER_STATE_STARTED)

#define IS_MANUAL_RESTARTING_GAP()		     ( (TIMER_State(&GLB_fsm_o3.AuxTimer) == TIMER_STATE_STARTED) && (GLB_fsm_o3.Option == MANUAL_MODE) )

#define IS_NOT_WAITING_FOR_SERVICE_WINDOW()         (TIMER_State(&GLB_fsm_o3.WaitForServiceTimer) == TIMER_STATE_STOP)
#define IS_WAITING_FOR_SERVICE_WINDOW_EXPIRED()     (TIMER_State(&GLB_fsm_o3.WaitForServiceTimer) == TIMER_STATE_EXPIRED)
#define IS_RUNNING_AND_BASED_ON_TUBE_CALIBRATION()  ( ((GLB_fsm_o3.CurrentState->State_ID) != STATE_CALIBRATE_O3_1) && (GLB_fsm_o3.GenerationMode == O3_GENERATION_BASED_ON_TUBE_CALIBRATION) )

/* Functions */
void DelegateDummy(void)
{
}

int8 DelegateDummy2(void)
{
	return 0;
}

int8 DelegateDummy3(uint16 a, int32 b)
{
	return 0;
}

int8 DelegateDummy4(uint16 a, int32 *b)
{
	return 0;
}

void SoftwareReset(void)
{
}

void Beep(void)
{
}

extern uint32 GLB_TickCounter;
extern uint32 GLB_FSM_ProcessEvent_Count;

#if (1)
void deb_printf(int8 deb_level, const char *fmt, ...)
{
	va_list args;

//	if( (signed char)GLB_fsm_o3.DebLevel >= (signed char)deb_level )
	if( GLB_fsm_o3.DebLevel & deb_level )
	{
        if(!(GLB_fsm_o3.DebLevel & D_LEV_HIDE_TIME_STAMP))
        {
            printf("%010ld . %010ld: ", GLB_FSM_ProcessEvent_Count, GLB_TickCounter);
        }
        va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
}
#else
void deb_printf(int8 deb_level, const char *fmt, ...)
{
	va_list args;

	//GLB_fsm_o3.DebLevel = D_LEV_6;
    if( (signed char)GLB_fsm_o3.DebLevel == (signed char)deb_level )
	{
        printf("%010ld; %010ld: ", GLB_FSM_ProcessEvent_Count, GLB_TickCounter);

		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
}
#endif

void WelcomeMessage(void)
{
	if( GLB_fsm_o3.DebLevel & D_LEV_SEND_WELCOME ){
		COM_O3_PutString((uint8 *)"SEDECAL 03 - Operation mode FSM\r");
		COM_O3_PutString((uint8 *)SW_VERSION);
		COM_O3_PutString((uint8 *)" ");
		COM_O3_PutString((uint8 *)DATE_STAMP);
		COM_O3_PutString((uint8 *)" ");
		COM_O3_PutString((uint8 *)TIME_STAMP);
		COM_O3_PutString((uint8 *)"\r");
	}
}

void ResolveGenerationMode(void)
{
	deb_printf(D_LEV_4, "O3 sensor available: %d\n", GLB_fsm_o3.HwConfig.O3Sensor);   // SWV debug
	deb_printf(D_LEV_4, "User generation mode: %d\n", GLB_fsm_o3.UsrConfig.userGenerationMode);   // SWV debug

	if(GLB_fsm_o3.UsrConfig.userGenerationMode == O3_GENERATION_BASED_ON_TUBE_CALIBRATION)
	{
		GLB_fsm_o3.GenerationMode = O3_GENERATION_BASED_ON_TUBE_CALIBRATION;
	}
	else
	{
		GLB_fsm_o3.GenerationMode = IS_HW_O3_SENSOR() ? O3_GENERATION_BASED_ON_O3_PHOTOSENSOR : O3_GENERATION_BASED_ON_TUBE_CALIBRATION;
//		GLB_fsm_o3.GenerationMode = (GLB_fsm_o3.HwConfig.O3Sensor == NO_O3_SENSOR) ? O3_GENERATION_BASED_ON_TUBE_CALIBRATION : O3_GENERATION_BASED_ON_O3_PHOTOSENSOR;
	}

	deb_printf(D_LEV_4, "Resulting generation mode: %d\n", GLB_fsm_o3.GenerationMode);   // SWV debug
}

void ChangeState(uint8 NewState)
{
	GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[NewState];
/*
 *  Once this function is called, the next FSM event must be the init event for the next state.
 *  Then, the animation timer is reset to prevent that the animation event corresponding to the
 *  current state happens during the current FSM_ProcessEvents() loop.
 */
	TIMER_Start(&GLB_fsm_o3.AnimationTimer, ANIMATION_PERIOD_MS);
}

/*
 * When send command fails, the function set the FSM error state and calls the current state's Error() function instead of wait
 * until next FSM state evaluation.
 * Any further ManageCommand() calls will return the same error without trying to send any command until the error is cleared.
 * This unusual implementation keep the rest of the code simpler, avoiding multiple error checks after each ManageCommand() call.
 */
#if 1
int8 ManageCommand(PROTOCOL_COMMAND_T * Command, PROTOCOL_RESPONSE_T * Response)
{
	/* Please, refer to upper note*/
	if (GLB_fsm_o3.ManageCommandError != (int8) PROTOCOL_RET_SUCCESS)
	{
		return GLB_fsm_o3.ManageCommandError;
	}

//	if(GLB_fsm_o3.CurrentState != &GLB_fsm_o3.States[STATE_ERROR])
	if( IS_NOT_ERROR_STATE() )
	{
		TIMER_Start(&GLB_fsm_o3.KeepAliveTimer, COMMUNICATION_TEST_PERIOD_MS); /* Reset periodic timer to reduce traffic */
	}

	GLB_fsm_o3.ManageCommandError = Protocol_SendCommand(Command);

	if (GLB_fsm_o3.ManageCommandError != (int8) PROTOCOL_RET_SUCCESS)
	{
//		GLB_fsm_o3.ErrorState = CTRL_BOARD_COMS_ERROR;
//		GLB_fsm_o3.CurrentState->Error();
		GotoError(CTRL_BOARD_COMS_ERROR);
	}
	else
	{
		Protocol_GetLastResponse(Response);
		GLB_fsm_o3.ErrorState = Response->Status;

		if (Response->Ack_Nak == CMD_RESPONSE_NAK)
		{
//			GLB_fsm_o3.ErrorState = CTRL_BOARD_COMS_ERROR;
//			GLB_fsm_o3.CurrentState->Error();
			GotoError(CTRL_BOARD_COMS_ERROR);
		}
		else if( (GLB_fsm_o3.ErrorState != NO_ERROR) && ((GLB_fsm_o3.ErrorState & WARNING_MASK) == 0) )
		{
//			GLB_fsm_o3.CurrentState->Error();
			GotoError(GLB_fsm_o3.ErrorState);
		}
	}
	return GLB_fsm_o3.ManageCommandError;
}
#endif

void ClearErrors(void)
{
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

  Command.Command = CMD_CLEAR_ALL_ERROR;
  do
  {
    if (ManageCommand(&Command,&Response) == PROTOCOL_RET_COMMAND_NOT_SENT)
      break;
  }while(Response.Status != CTRL_BOARD_STATUS_OK);
}

void SendReset(void)
{
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

  Command.Command = CMD_RESET;
  ManageCommand(&Command,&Response);
}

void SendStopCommand(void)
{
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

  SwitchOutputValves(OUTPUT_TO_CATALYSER);

  Command.Command = CMD_STOP_OZONE;
  ManageCommand(&Command,&Response);

  Command.Command = CMD_STOP_FLOW;
  ManageCommand(&Command,&Response);

  Command.Command = CMD_SET_FAN;
  Command.Identifier[0] = FAN_0;
  Command.Value = FAN_OFF;
  ManageCommand(&Command,&Response);
}

void SendGenerationCommands(int16 GenerationMode, uint8 FlowRunningState)
{
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

  /* Restore error management on control board */
  RestoreErrors();

  /* Debug purposes */
  Command.Command = CMD_GET_SENSOR_OFFSET;
  snprintf(Command.Identifier, 5, "%X", ID_SENSOR_OZONE);
  ManageCommand(&Command,&Response);

  Command.Command = CMD_GET_SENSOR;
  snprintf(Command.Identifier, 5, "%X", ID_SENSOR_PRESSURE_1);
  ManageCommand(&Command,&Response);
  GLB_fsm_o3.PressAtm = Response.Data[0];

  Command.Command = CMD_SET_FLOW_TARGET;
  Command.Value = GLB_fsm_o3.ConfiguredFlow * 8;
  ManageCommand(&Command,&Response);

/* AVOID flow zero. PWM flow is slower since resolution is 16 bits. */
  if(FlowRunningState == FLOW_NOT_STARTED_YET)
  {
    Command.Command = CMD_START_FLOW;
    ManageCommand(&Command,&Response);
  }

  Command.Command = CMD_SET_FAN;
  Command.Identifier[0] = FAN_0;
  Command.Value = FAN_ON;
  ManageCommand(&Command,&Response);

  Command.Command = CMD_SET_OZONE_TARGET;
  Command.Value = GLB_fsm_o3.ConfiguredO3Concentration * 8;
  ManageCommand(&Command,&Response);

  /* Delayed O3 start to avoid O3 peak due to low speed PID adjust */
//  if(!((GLB_fsm_o3.Option != SYRINGE_MODE) && (GLB_fsm_o3.ConfiguredFlow < MAX_FLOW_LEVEL_WITHOUT_O3_ON_GENERATION_START)))
  if(1) // /* Delayed O3 start to avoid O3 peak due to low speed PID adjust */ /* Always */
  {
    Command.Command = CMD_START_OZONE;
    Command.Value = GenerationMode;
    ManageCommand(&Command,&Response);
  }

//  GLB_fsm_o3.GeneratorCleaned = 0;
}

void SwitchOutputValves(OUTPUT_POSITION_E OutputPosition)
{
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

  Command.Command = CMD_SET_VALVE;
  Command.Identifier[0] = VALVE_OUTPUT;

  if (OutputPosition == OUTPUT_TO_BOTH)
  {
    Command.Value = VALVE_OPEN;
    ManageCommand(&Command,&Response);
    Command.Identifier[0] = VALVE_CATALYSER;
    ManageCommand(&Command,&Response);
  }
  else
  {
    Command.Value = (OutputPosition == OUTPUT_TO_CATALYSER) ? VALVE_CLOSE : VALVE_OPEN;
    ManageCommand(&Command,&Response);
    Command.Identifier[0] = VALVE_CATALYSER;
    Command.Value ^= 0x01;  /* Negate the value sent in the previous command as valves act in inverse */
    ManageCommand(&Command,&Response);
  }
}
void InitialCheck_1(void) 
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};
	int16 Flow = 0;
	uint8 i = 0;
	static uint8 Delay = 0;

	if(Delay == 0)
	{
		printf("InitialCheck_1 (start)\n");

		/* Ensure output to catalyser */
		SwitchOutputValves(OUTPUT_TO_CATALYSER);

		/* Flow generation */
		Command.Command = CMD_SET_FLOW_TARGET;
		Command.Value = FLOW_START_UP_TEST_FLOW;
		ManageCommand(&Command,&Response);

		Command.Command = CMD_START_FLOW;
		ManageCommand(&Command,&Response);

		memset(StartUpLog, 0, sizeof(StartUpLog));
	}

	if(++Delay >= 5)
	{
		printf("InitialCheck_1 (measuring)\n");

		/* Measure flow */
		for(i=0; i<16; i++)
		{
			Command.Command = CMD_GET_SENSOR;
			//UTILS_uint8ToHexString(ID_SENSOR_FLOW,Command.Identifier,FALSE);
			snprintf(Command.Identifier, 5, "%X", ID_SENSOR_FLOW);
			ManageCommand(&Command,&Response);
			Flow += (int16) Response.Data[0]; /* l/h * 8 */
		}

		Flow = Flow >> 7; /* >> 4 (get media), >> 3 (get l/h)*/

		if(Flow < MIN_FLOW_VALUE)
		{
			GLB_fsm_o3.MaxAvailableFlow = MIN_FLOW_VALUE;
		}
		else if(Flow > MAX_FLOW_VALUE)
		{
			GLB_fsm_o3.MaxAvailableFlow = MAX_FLOW_VALUE;
		}
		else
		{
			GLB_fsm_o3.MaxAvailableFlow = (uint16) Flow;
		}
		/*
		 *  Since DEFAULT_FLOW_VALUE is the flow used to fill syringe
		 *  error will be reported bellow this flow level (although
		 *  it should be possible to run other therapies with low input
		 *  flow levels).
		 */
		if(Flow < DEFAULT_FLOW_VALUE)
		{
//			GLB_fsm_o3.ErrorState = FLOW_ERROR;
			GotoError(FLOW_ERROR);
		}
		else
		{
//			GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_INIT_CHECK_2];
			ChangeState(STATE_INIT_CHECK_2);
		}
		printf("InitialCheck_1 (end)\n");
	}
}

/* Debug function to read O3 sensor LED and leave the trace on the bus */
void DebugO3SensorTune(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	Command.Command = CMD_GET_SENSOR;
	Command.Identifier[0] = ID_SENSOR_OZONE + 0x30;
	ManageCommand(&Command,&Response);
	Command.Identifier[0] = 0x37;
	ManageCommand(&Command,&Response);
}

void InitialCheck_2_Ini(void)
{
	InitialCheck_2_FSM(1); // initialize and run
}

void InitialCheck_2(void)
{
	InitialCheck_2_FSM(0); // just run
}

#define STEP_2_LOOP_ITERATION 5

void InitialCheck_2_FSM(uint8 init)
{
	static int8 skip =  0;
	static int8 step =  0;
	static int8 delay = 0;
	static int8 count = 0;
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	if(init)
	{
		skip =  0;
		step =  0;
		delay = 0;
		count = 0;
		printf("InitialCheck_Step2 (start)\n");
	}

	switch(step)
	{
	case 0: // start flow
		Command.Command = CMD_SET_FLOW_TARGET;
		Command.Value = FLOW_START_UP_TEST_CURRENT;
		ManageCommand(&Command,&Response);

		Command.Command = CMD_START_FLOW;
		ManageCommand(&Command,&Response);
		step++;
		delay = 0;
		break;
	case 1: //delay before tune
		if( IS_NO_HW_O3_SENSOR() || IS_RUNNING_AND_BASED_ON_TUBE_CALIBRATION() )
		{
			skip = 1;
		}
		else
		{
			if( delay < (count ? 5 : 10) ) // double delay first time
			{
				delay++;
				Command.Command = CMD_TEST_COM;
				ManageCommand(&Command,&Response);
			}
			else
			{
				delay = 0;
				step++; // end delay
			}
			//	printf("InitialCheck_Step2 (unified), delay %d\n", delay);
		}
		break;
	case 2: // launch tune
		Command.Value = (count < STEP_2_LOOP_ITERATION - 1) ? TUNE_O3_SENSOR_LED : TUNE_O3_SENSOR_LED_AND_START_LED_PID;
		Command.Command = CMD_START_O3_SENSOR_CAL;
		ManageCommand(&Command,&Response);
		step++;
		printf("InitialCheck_Step2 (tune started: %d)\n", count);
		break;
	case 3: // wait for tune end
		Command.Command = CMD_STATE_O3_SENSOR_CAL;
		ManageCommand(&Command,&Response);
		if(Response.Data[0] == 0)
		{
			step = 1; // tune ended, go to delay before next tune
			count++;
			DebugO3SensorTune();
		}
		//	printf("InitialCheck_Step2 (unified), tune waiting %d\n", count);
		break;
	default:
		//	printf("InitialCheck_Step2 (unified), default case\n");
		break;
	}

	/* NOTE: When no sensor used, it is necessary to skip this procedure in the second iteration.       */
	/*       This function is called as Entry function during STATE_CALIBRATE_O3_STEP_1 state, since    */
	/*       it changes GLB_fsm_o3.CurrentState value the relation between GLB_fsm_o3.LastState and     */
	/*       GLB_fsm_o3.CurrentState (see FSM_ProcessEvents()) is corrupted and the system  fsm         */
	/*       sequence is broken.                                                                        */
	/*       To avoid this skip is updated when step = 1.                                               */
	/*       By the way, flow is started when step = 0.                                                 */

	if( (count >= STEP_2_LOOP_ITERATION) ||	skip )
	{
		skip =  0;
		step =  0;
		delay = 0;
		count = 0;

		switch(GLB_fsm_o3.CurrentState->State_ID)
		{
		case STATE_INIT_CHECK_2:
//			GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_INIT_CHECK_3];
			ChangeState(STATE_INIT_CHECK_3);
			break;
		case STATE_CALIBRATE_O3_1:
//			GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_CALIBRATE_O3_2];
			ChangeState(STATE_CALIBRATE_O3_2);
			break;
		default:
//			GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_ERROR];
//			GLB_fsm_o3.ErrorState = UNEXPECTED_ERROR;
			GotoError(UNEXPECTED_ERROR);
		}
		printf("InitialCheck_Step2 (end)\n");
	}
}

#define MAX_ERROR_IN_CURRENT_START_UP_TEST_BY_SENSOR  16 * 8
#define MAX_ERROR_IN_CURRENT_START_UP_TEST_BY_PULSES  150

void InitialCheck_3(void)
{
	uint8 i = 0;
	int16 Flow = 0;
	int32 TestMeasure = 0;
    int32 TestCurrent = 0;
	int16 RefMeasure = 0;
	int16 PropValveState = 0;
	static uint8 Delay = 0;
    static uint8 Attempt = 0;
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	if(!Delay)
	{
		printf("InitialCheck_3 (start)\n");
	    TestCurrentStart();
	}

	if(++Delay >= 17)
	{
		printf("InitialCheck_3 (reading)\n");

		/* Measure current */
		for(i = 0; i < 32; i++)
		{
			Command.Command = CMD_GET_SENSOR;
			snprintf(Command.Identifier, 5, "%X", GLB_fsm_o3.GenerationMode ? ID_SENSOR_OZONE : ID_SENSOR_CURRENT);
			ManageCommand(&Command,&Response);
			TestMeasure += Response.Data[0];

			snprintf(Command.Identifier, 5, "%X", ID_SENSOR_CURRENT);
			ManageCommand(&Command,&Response);
			TestCurrent += Response.Data[0];
		}
		TestMeasure = TestMeasure >> 5;
		TestCurrent = TestCurrent >> 5;

		StartUpLog[START_UP_LOG_TEST] = TestMeasure;

		/* Leave debug info in the bus and generate start up result information */  //TODO: formalize start up result information when proceed
		Command.Command = CMD_GET_SENSOR_OFFSET;
		ManageCommand(&Command,&Response);
		StartUpLog[START_UP_LOG_SENSOR_OFFSET] = Response.Data[0];

		Command.Command = CMD_GET_SENSOR;
		snprintf(Command.Identifier, 5, "%X", ID_SENSOR_OZONE_DIRECT);
		ManageCommand(&Command,&Response);
		StartUpLog[START_UP_LOG_O3_DIRECT] = Response.Data[0];

		snprintf(Command.Identifier, 5, "%X", ID_SENSOR_PRESSURE_1);
		ManageCommand(&Command,&Response);
		StartUpLog[START_UP_LOG_PRESSURE] = Response.Data[0];

		snprintf(Command.Identifier, 5, "%X", ID_SENSOR_TEMPERATURE_0);
		ManageCommand(&Command,&Response);
		StartUpLog[START_UP_LOG_TEMPERATURE] = Response.Data[0];

		/* Read calibration pressure just for debug */
		Command.Command = CMD_GET_TABLE_POSITION;
		Command.Identifier[0] = 'P';        /* Parameters Table */
		Command.Position[0] = CALIBRATION_PRESSURE_POSITION;
		ManageCommand(&Command,&Response);

		/* Read reference current */
		Command.Position[0] = START_UP_TEST_CURRENT_TABLE_POSITION;
		ManageCommand(&Command,&Response);

		RefMeasure = GLB_fsm_o3.GenerationMode ? O3_CALIBRATION_POINT_3 : Response.Data[0];
		StartUpLog[START_UP_LOG_TEST_REF] = Response.Data[0];

		/* Get proportional valve state to check first silicon tube fold (tube between input block and 'T' of sensor pressure 2) */
		Command.Command = CMD_GET_PROPORTIONAL_VALVE;
		snprintf(Command.Identifier, 5, "%X", PROP_VALVE_INPUT);
		ManageCommand(&Command,&Response);
		PropValveState = Response.Data[0];

		Command.Command = CMD_GET_SENSOR;
		snprintf(Command.Identifier, 5, "%X", ID_SENSOR_FLOW);
		ManageCommand(&Command,&Response);
		Flow += (int16) Response.Data[0]; /* l/h * 8 */

        /* Ensure modulation dead time is 8 after test (changed if no O3 sensor mode is used) */
        Command.Command = CMD_SET_MODULATOR_DEADTIME;
        Command.Value = 8;
		ManageCommand(&Command,&Response);

		printf("RefMeasure: %d - TestMeasure: %d - TestCurrent: %d\n", RefMeasure, (int16)TestMeasure, (int16)TestCurrent);

		/* Analyze data */
        if(TestCurrent < 100)
		{
//			GLB_fsm_o3.ErrorState = CTRL_BOARD_TRANSFORMER_ERROR;
			GotoError(CTRL_BOARD_TRANSFORMER_ERROR);
		}
		else if( (PropValveState < PROP_VALVE_CLOSED_VALUE) && (Flow > 0) )
		{
//			GLB_fsm_o3.ErrorState = CTRL_BOARD_FLOW_ERROR;
			GotoError(CTRL_BOARD_FLOW_ERROR);
		}
		else if(abs(RefMeasure - (int16)TestMeasure) > (GLB_fsm_o3.GenerationMode ? MAX_ERROR_IN_CURRENT_START_UP_TEST_BY_SENSOR : MAX_ERROR_IN_CURRENT_START_UP_TEST_BY_PULSES))
		{
			if(Attempt < 2)
			{
				Delay = 0; /* force the whole procedure AND to detect new attempt (when stop command should be sent)*/
				Attempt++;
//				GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_INIT_CHECK_2];
				ChangeState(STATE_INIT_CHECK_2);
				printf("Calibration error during start up, retrying (%d)\n", Attempt);
			}
			else
			{
//				GLB_fsm_o3.ErrorState = CAL_ERROR;
				GotoError(CAL_ERROR);
				GLB_fsm_o3.CalibrationErrorDuringStartUp = 1;
			}
		}
		else
		{
			GotoRepose();
			printf("InitialCheck_3 (end)\n");
		}

        /* if there is no another attempt */
       	if (Delay != 0)
        {
        	SendStopCommand();
        }
	}
}

void GotoInitialCheck(void)
{
	ResolveGenerationMode();
	TIMER_Stop(&GLB_fsm_o3.DataReadTimer);
	TIMER_Start(&GLB_fsm_o3.KeepAliveTimer, COMMUNICATION_TEST_PERIOD_MS);
//	GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_INIT_CHECK_1];
	ChangeState(STATE_INIT_CHECK_1);
}

void GotoService(void)
{
	printf("Goto service state...\n");

	/* Skip error management on control board */
	SkipErrors();

	SendStopCommand();
	TIMER_Stop(&GLB_fsm_o3.WaitForServiceTimer);
	GLB_fsm_o3.Option = SERVICE_MODE;
//	GLB_fsm_o3.Starting = 0;//  remove when properly tested
//	GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_WAITING_FOR_SERVICE];
	ChangeState(STATE_WAITING_FOR_SERVICE);
}

#if 0
void GotoRepose(void)
{
	printf("Goto repose state...\n");
//	GLB_fsm_o3.Starting = 0;// remove when properly tested
	GLB_fsm_o3.TemperatureMonitoring = 0;

	SendStopCommand();
//	ClearErrors();
	StopManualGap();
	TIMER_Stop(&GLB_fsm_o3.DataReadTimer);
	TIMER_Start(&GLB_fsm_o3.KeepAliveTimer, COMMUNICATION_TEST_PERIOD_MS);


#if 0  // remove when properly tested
	if (GLB_fsm_o3.Option == SERVICE_MODE)  //  review necessity
	{
		GotoService();
	}
	else
	{
		GLB_fsm_o3.Option = NO_MODE;
		GLB_fsm_o3.StableOzone = FALSE;
		GLB_fsm_o3.StableFlow = FALSE;
		GLB_fsm_o3.MaxAllowedFlow = MAX_FLOW_VALUE;
		GLB_fsm_o3.MaxAllowedTime = MAX_TIME_VALUE;
		GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_WAITING_FOR_PROTOCOL];
	}
#endif
	GLB_fsm_o3.Option = NO_MODE;
	GLB_fsm_o3.StableOzone = FALSE;
	GLB_fsm_o3.StableFlow = FALSE;
	GLB_fsm_o3.MaxAllowedFlow = MAX_FLOW_VALUE;
	GLB_fsm_o3.MaxAllowedTime = MAX_TIME_VALUE;
//	GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_WAITING_FOR_PROTOCOL];
	ChangeState(STATE_WAITING_FOR_PROTOCOL);
}
#endif
void GotoRepose(void)
{
	deb_printf(D_LEV_4, "Goto repose state request...\n");
	ChangeState(STATE_WAITING_FOR_PROTOCOL);
}
void WaitingForProtocol(void)
{
//	GLB_fsm_o3.Starting = 0;// remove when properly tested
	GLB_fsm_o3.TemperatureMonitoring = 0;

	SendStopCommand();
//	ClearErrors();
	StopManualGap();
	TIMER_Stop(&GLB_fsm_o3.DataReadTimer);
	TIMER_Start(&GLB_fsm_o3.KeepAliveTimer, COMMUNICATION_TEST_PERIOD_MS);


#if 0  // remove when properly tested
	if (GLB_fsm_o3.Option == SERVICE_MODE)  //  review necessity
	{
		GotoService();
	}
	else
	{
		GLB_fsm_o3.Option = NO_MODE;
		GLB_fsm_o3.StableOzone = FALSE;
		GLB_fsm_o3.StableFlow = FALSE;
		GLB_fsm_o3.MaxAllowedFlow = MAX_FLOW_VALUE;
		GLB_fsm_o3.MaxAllowedTime = MAX_TIME_VALUE;
		GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_WAITING_FOR_PROTOCOL];
	}
#endif
	GLB_fsm_o3.Option = NO_MODE;
	GLB_fsm_o3.StableOzone = FALSE;
	GLB_fsm_o3.StableFlow = FALSE;
	GLB_fsm_o3.MaxAllowedFlow = MAX_FLOW_VALUE;
	GLB_fsm_o3.MaxAllowedTime = MAX_TIME_VALUE;
}

void GotoCalibrateFlow(void)
{
  int16 Offset;
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

  Command.Command = CMD_GET_SENSOR;
  snprintf(Command.Identifier, 5, "%X", ID_SENSOR_PRESSURE_1);
  ManageCommand(&Command,&Response);
  Offset = Response.Data[0];
  snprintf(Command.Identifier, 5, "%X", ID_SENSOR_PRESSURE_0);
  ManageCommand(&Command,&Response);
  Offset = Offset * (Offset - Response.Data[0]);

  /* Store temporally Offset value on R1 to write calibration data at once when Slope
   * be available, avoiding partial calibration if cancel is used during process */
  CalibrationRef.R1 = Offset;

  SwitchOutputValves(OUTPUT_TO_EXTERIOR);

  /* Configure flow to first calibration point*/
  Command.Command = CMD_SET_PROPORTIONAL_VALVE;
  snprintf(Command.Identifier, 5, "%X", PROP_VALVE_INPUT);
  Command.Value = 700; // starting value for next reference value, it will be override by GUI
  ManageCommand(&Command,&Response);
}

void GotoCalibrateO3(void)
{
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

  SwitchOutputValves(OUTPUT_TO_EXTERIOR);

  /* Set sensor slope to 1*/
  Command.Command = CMD_SET_SENSOR_SLOPE;
  snprintf(Command.Identifier, 5, "%X", ID_SENSOR_OZONE);
  Command.Value = 256;
  Command.Position[0] = SECTION_0;
  ManageCommand(&Command,&Response);

  /* Set Sensor offset to 0*/
  Command.Command = CMD_SET_SENSOR_OFFSET;
  Command.Value = 0;
  ManageCommand(&Command,&Response);

//  GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_CALIBRATE_O3_1];
  ChangeState(STATE_CALIBRATE_O3_1);
}

void StartForO3Calibration(void)
{
	uint16 Width;
	uint16 BasePeriod;
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	/* Get sensor value for zero O3 */
	Command.Command = CMD_GET_SENSOR;
	snprintf(Command.Identifier, 5, "%X", ID_SENSOR_OZONE_DIRECT);
	ManageCommand(&Command,&Response);
	CalibrationRef.R1 = Response.Data[0];

	/* Modify flow target on the fly (ensure that the flow for
	 * this procedure is independent from the procedures before) */
	Command.Command = CMD_SET_FLOW_TARGET;
	Command.Value = FLOW_O3_CALIBRATION_OZP;
	ManageCommand(&Command,&Response);

	/* Get system parameters & prepare data to start generator */
	Command.Command = CMD_GET_TABLE_POSITION;
	Command.Identifier[0] = 'P';        /* Parameters Table */

	Command.Position[0] = WIDTH_INI_VALUE_TABLE_POSITION;
	ManageCommand(&Command,&Response);
/*  Width = Response.Data[0];*/
	Width = 1024 - 350; // starting value, it will be override by GUI

	Command.Position[0] = BASE_PERIOD_INI_VALUE_TABLE_POSITION;
	ManageCommand(&Command,&Response);
	BasePeriod = Response.Data[0];

	SwitchOutputValves(OUTPUT_TO_EXTERIOR);

	SendGeneratorStartSequence(BasePeriod, Width, 8);
}

void GotoCalibratePeriod(void)
{
	StartResonanceFrequencySearch();
//	GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_CALIBRATE_PERIOD];
	ChangeState(STATE_CALIBRATE_PERIOD);
}

void GotoAdjustPressCompFactor(void)
{
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

  SwitchOutputValves(OUTPUT_TO_EXTERIOR);

  /* Configure flow target to default flow value */
  Command.Command = CMD_SET_FLOW_TARGET;
  Command.Value = DEFAULT_FLOW_VALUE * 8;
  ManageCommand(&Command,&Response);

  Command.Command = CMD_START_FLOW;
  ManageCommand(&Command,&Response);

  /* Force generation conditions for 40 ug/Nml without knowing effective pulses for 40 ug/Nml */
  Command.Command = CMD_SET_OZONE_TARGET;
  Command.Value = OZONE_START_UP_TEST_CURRENT;
  ManageCommand(&Command,&Response);

  Command.Command = CMD_START_OZONE;
  ManageCommand(&Command,&Response);

  Command.Command = CMD_STOP_OZONE;
  ManageCommand(&Command,&Response);

  /* Modulator base start */
  Command.Command = CMD_MODULATOR_BASE_START;
  ManageCommand(&Command,&Response);

  /* Modulator start */
  Command.Command = CMD_MODULATOR_START;
  ManageCommand(&Command,&Response);
#if 0
  /* Wait until flow stabilization */
  for(i=0; i<10; i++)
  {
    Command.Command = CMD_TEST_COM;
    ManageCommand(&Command,&Response);
    Delay_o3_ms(1000);
  }
#endif
//  GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_SELECT_PRESS_COMP_FACTOR];
  ChangeState(STATE_SELECT_PRESS_COMP_FACTOR);
}

void GotoSaveParameters(void)
{
//  GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_SAVING_PARAMETERS];
	ChangeState(STATE_SAVING_PARAMETERS);
}

void GotoLoadParameters(void)
{
//  GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_LOADING_PARAMETERS];
	ChangeState(STATE_LOADING_PARAMETERS);
}


void GotoError(uint16 ErrorState)
{
    deb_printf(D_LEV_2, "Error code %d...\n", ErrorState);

	if(IS_SYRINGE_AUTO_MODE()  && (GLB_fsm_o3.ErrorState == FLOW_ERROR)){
		deb_printf(D_LEV_4, "Flow error during syringe (do not process )...\n");
	}
	else
	{
		if (ErrorState != NO_ERROR)
		{
			Beep();
			StopManualGap();
			TIMER_Stop(&GLB_fsm_o3.DataReadTimer);
			TIMER_Stop(&GLB_fsm_o3.KeepAliveTimer);

			GLB_fsm_o3.ErrorState = ErrorState;

			if(DepressureNeededOnError(GLB_fsm_o3.CurrentState->State_ID, ErrorState))
			{
				GotoDepressure(STATE_ERROR);
				deb_printf(D_LEV_4, "Release pressure and go to error state: %d\n", ErrorState);
			}
			else
			{
				ChangeState(STATE_ERROR);
				deb_printf(D_LEV_4, "Go to error state: %d\n", ErrorState);
			}
		}
	}
}
#if 0
void GotoError(void)
{
	if(IS_SYRINGE_AUTO_MODE()  && (GLB_fsm_o3.ErrorState == FLOW_ERROR)){
		printf("Flow error during syringe (do not process )...\n");
	}
	else
	{
		if (GLB_fsm_o3.ErrorState != NO_ERROR)
		{
			printf("Goto error %d (depressure and go to error state)\n", GLB_fsm_o3.ErrorState);

			Beep();
			StopManualGap();
			TIMER_Stop(&GLB_fsm_o3.DataReadTimer);
			TIMER_Stop(&GLB_fsm_o3.KeepAliveTimer);
			GLB_fsm_o3.DepressureReturnState = STATE_ERROR;
			GotoDepressure();
		}
	}
}
#endif
/*
 * Cancel Flow error at start up forces to test calibration error.
 * Cancel calibration error at start up goes to main menu. The doctor have to know that may be the concentration
 * is not precise, but for continuous and manual therapies sometimes is better less concentration than nothing. Syringe
 * is forbidden due to precision.
 * Any other error during start up forces reset.
 */
void ExitError(void)
{
	/* Mandatory line, see ManageCommand() funtion */
	GLB_fsm_o3.ManageCommandError = (int8) PROTOCOL_RET_SUCCESS;

	deb_printf(D_LEV_4, "Exit error state %d\n", GLB_fsm_o3.ErrorState);

	ClearErrors();

	if( IS_STARTING() )
	{
		switch(GLB_fsm_o3.ErrorState)
		{
		case FLOW_ERROR:        SendReset();
								TIMER_Start(&GLB_fsm_o3.KeepAliveTimer, COMMUNICATION_TEST_PERIOD_MS);
								ChangeState(STATE_INIT_CHECK_2);
		                        break;
		case TRANSFORMER_ERROR:
		case CAL_ERROR:         GotoRepose();
		                        break;
		default:                SoftwareReset();
		}
	}
	else
	{
		GotoRepose();
	}
}
#if 0
void GotoWashing(uint8 ReturnState)
{
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

  GLB_fsm_o3.FreezeRemainingTime = 1;

  SwitchOutputValves(OUTPUT_TO_EXTERIOR);

  Command.Command = CMD_SET_FLOW_TARGET;
  Command.Value = WASHING_FLOW * 8;
  ManageCommand(&Command,&Response);

  Command.Command = CMD_START_FLOW;
  ManageCommand(&Command,&Response);

  GLB_fsm_o3.WashingReturnState = ReturnState;
  GLB_fsm_o3.WashingConfiguredTime = DEFAULT_WHASHING_TIME;
  GLB_fsm_o3.WashingSeconds = GLB_fsm_o3.WashingConfiguredTime;
//  GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_WASHING];
  ChangeState(STATE_WASHING);
}
#endif

void GotoWashing(uint8 ReturnState)
{
	deb_printf(D_LEV_4, "Goto washing from %d state...\n", GLB_fsm_o3.CurrentState->State_ID);
	GLB_fsm_o3.WashingReturnState = ReturnState;
	ChangeState(STATE_WASHING);
}

void WashingIni(void)
{
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

  GLB_fsm_o3.FreezeRemainingTime = 1;

  SwitchOutputValves(OUTPUT_TO_EXTERIOR);

  Command.Command = CMD_SET_FLOW_TARGET;
  Command.Value = WASHING_FLOW * 8;
  ManageCommand(&Command,&Response);

  Command.Command = CMD_START_FLOW;
  ManageCommand(&Command,&Response);

  GLB_fsm_o3.WashingConfiguredTime = DEFAULT_WHASHING_TIME;
  GLB_fsm_o3.WashingSeconds = GLB_fsm_o3.WashingConfiguredTime;
}

void WashingExtesion(void)
{
  GLB_fsm_o3.WashingSeconds += 10;
  if(GLB_fsm_o3.WashingSeconds > 59)
  {
    GLB_fsm_o3.WashingSeconds = 59;
  }
  GLB_fsm_o3.WashingConfiguredTime = GLB_fsm_o3.WashingSeconds;
}

uint32 GetCumulatedVolume(void)
{
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

  Command.Command = CMD_GET_SENSOR;
  snprintf(Command.Identifier, 5, "%X", ID_SENSOR_VOLUME);

  if (ManageCommand(&Command,&Response) == PROTOCOL_RET_SUCCESS)
  {
    return Response.Data[0];
  }
  else
  {
    return 0;
  }
}

void SetFanDependingOnTemperature(uint16 Temperature)
{
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

  Command.Command = CMD_GET_SENSOR;
//  UTILS_uint8ToHexString(ID_SENSOR_TEMPERATURE_0,Command.Identifier,FALSE);
  snprintf(Command.Identifier, 5, "%X", ID_SENSOR_TEMPERATURE_0);
  ManageCommand(&Command,&Response);

  Command.Command = CMD_SET_FAN;
  Command.Identifier[0] = FAN_0;

  if(Response.Data[0] >= Temperature)
  {
    Command.Value = FAN_ON;
    ManageCommand(&Command,&Response);
  }
  else if(Response.Data[0] < (Temperature - FAN_HYSTERESIS))
  {
    Command.Value = FAN_OFF;
    ManageCommand(&Command,&Response);
  }
}

void SetMaxAllowedFlow(void)
{
  /* Set max allowed flow according to next rule
   *
   *   (F)
   *    |      .
   * 50 +* * * *
   *    |      .  *
   *    |      .     *
   *    |      .        *
   *    |      .           *
   *    |      .              *
   * 30 +........................*..
   *    |      .                 .
   *    |______._________________.____ (C)
   *          60                80
   */
  if ( (110 - GLB_fsm_o3.ConfiguredO3Concentration) < GLB_fsm_o3.MaxAllowedFlow )
  {
    GLB_fsm_o3.MaxAllowedFlow = 110 - GLB_fsm_o3.ConfiguredO3Concentration;
    /* If we are in Syringe mode, flow is not configurable but defaulted to 30l/h*/
    /* Time is neither configured because syringe filling completion is detected by pressure value */
  }

  if(GLB_fsm_o3.MaxAvailableFlow < GLB_fsm_o3.MaxAllowedFlow)
  {
    GLB_fsm_o3.MaxAllowedFlow = GLB_fsm_o3.MaxAvailableFlow;
  }
}

void SetMaxAllowedTime(void)
{
  /* Set max allowed time according to next rule
   *
   *   (T)
   *    |      .
   * 20 +* * * *
   *    |      .  *
   *    |      .     *
   *    |      .        *
   *    |      .           *
   *    |      .              *
   *  5 +........................*..
   *    |      .                 .
   *    |______._________________.____ (C)
   *          30                80
   */
  uint16 Tmp16;

  if(GLB_fsm_o3.ConfiguredO3Concentration > 30)
  {
    Tmp16  = (80 - GLB_fsm_o3.ConfiguredO3Concentration) * 15;
    Tmp16  = Tmp16 / 50;
    Tmp16 += 5;
    GLB_fsm_o3.MaxAllowedTime = (uint8) Tmp16;
  }
}

#if 0
void GotoDepressure(void)
{
  GLB_fsm_o3.DepressureSeconds = 3;
  GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_DEPRESSURE];
}
#endif

uint8 DepressureNeededOnError(uint8 CurrentState, uint16 ErrorState)
{
	switch(CurrentState)
	{
	case STATE_WAITING_FOR_PROTOCOL:
	case STATE_TUNING_O3_SENSOR:
	case STATE_ADJUSTING:
	case STATE_WAITING_EXTERNAL_STUFF:
	case STATE_WAITING_FOR_SERVICE:
	case STATE_CALIBRATE_PERIOD:
	case STATE_SAVING_PARAMETERS:
	case STATE_LOADING_PARAMETERS:
		return 0;
	case STATE_O3_GENERATING:
		if (ErrorState == LEAKAGE_ERROR) return 0;
		else return 1;
	default:
		return 1;
	}
}

void GotoDepressure(uint8 ReturnState)
{
	GLB_fsm_o3.DepressureSeconds = 3;
	GLB_fsm_o3.DepressureReturnState = ReturnState;
	GLB_fsm_o3.DepressureErrorReturnState = GLB_fsm_o3.ErrorState;
	SwitchOutputValves(OUTPUT_TO_BOTH);
//  GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_DEPRESSURE];
	ChangeState(STATE_DEPRESSURE);
}

void UpdateDepressure(void)
{
  if(!GLB_fsm_o3.DepressureSeconds)
  {
    SwitchOutputValves(OUTPUT_TO_EXTERIOR);
    GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[GLB_fsm_o3.DepressureReturnState];
    GLB_fsm_o3.ErrorState = GLB_fsm_o3.DepressureErrorReturnState;
	ChangeState(GLB_fsm_o3.DepressureReturnState);
  }
}

void UpdateWashing(void)
{
    if (!GLB_fsm_o3.WashingSeconds)
    {
        SendStopCommand();
//        GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[GLB_fsm_o3.WashingReturnState];
    	ChangeState(GLB_fsm_o3.WashingReturnState);
    }
}

void CancelWashing(void)
{
    SendStopCommand();
//    GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[GLB_fsm_o3.WashingReturnState];
	ChangeState(GLB_fsm_o3.WashingReturnState);
}

int SquareRoot(int32 Dato)
{
// Quick calculation of the square root for integers. (By Carlos Fern�ndez Yoldi)
	int32 Oper = Dato;
	int32 Result = 0;
	int32 Medio = 0x40000000;

	if(Dato <= 0)
	{
		return 0;
	}

	while (Medio > Oper) Medio >>= 2;

	while (Medio != 0)
	{
		if (Oper >= Result + Medio)
		{
			Oper -= Result + Medio;
			Result += (Medio << 1);
		}
		Result >>= 1;
		Medio >>= 2;
	}
	return Result;
}

int16 ComputeFlowSlope(int16 P1, int16 P2, int16 Offset, int16 CurrentFlow)
{
	int32 Tmp;
	int32 Flow;

	Tmp = (int32) ( P1 - P2 );
	Tmp = Tmp * P2;
	Tmp = Tmp + Offset;
	Tmp = SquareRoot(Tmp);

	Flow = (int32) (CurrentFlow * 48); /* (6 * 8) */
	Flow = Flow << 14;
	Flow = Flow / 100;

	Tmp = Tmp ? (Flow / Tmp) : Tmp;  /* 240 * 2^14 */ // avoid division by zero

//  printf("Computed flow slope: %ld\n", Tmp);
//  printf("Computed flow offset: %d\n", Offset);
	return (int16) Tmp;
}

uint16 ComputeDeadTime(uint16 Period)
{
    return ( (Period >> 1) - (Period >> 3) );  /* (Period * 48) / 128 -> validated by CFY 17/04/23 */
}

void SendGeneratorStartSequence(uint16 ModBasePeriod, uint16 ModWith, uint16 DeadTime)
{
    PROTOCOL_COMMAND_T  Command  = {0};
    PROTOCOL_RESPONSE_T Response = {0};


    Command.Command = CMD_SET_FAN ;
    Command.Identifier[0] = FAN_0;
    Command.Value = FAN_ON;
    ManageCommand(&Command,&Response);

    /* Modulator stop */
    Command.Command = CMD_MODULATOR_STOP;
    ManageCommand(&Command,&Response);

    /* Configure modulator base period */
    Command.Command = CMD_SET_MODULATOR_BASE_PERIOD;
    Command.Value = ModBasePeriod;
    ManageCommand(&Command,&Response);

    /* Configure modulator width */
    Command.Command = CMD_SET_MODULATOR_WIDTH;
    Command.Value = ModWith;
    ManageCommand(&Command,&Response);

    /* Configure modulator dead time */
    if(DeadTime < 8)
    {
        DeadTime = 8;
    }
    if(DeadTime > 255)
    {
        DeadTime = 255;
    }
    Command.Command = CMD_SET_MODULATOR_DEADTIME;
    Command.Value = DeadTime;
    ManageCommand(&Command,&Response);

    /* Modulator start */
    Command.Command = CMD_MODULATOR_START;
    ManageCommand(&Command,&Response);
}

void TestCurrentStart(void)
{
    uint16 ModWidth ;
    uint16 BasePeriod;
    PROTOCOL_COMMAND_T  Command  = {0};
    PROTOCOL_RESPONSE_T Response = {0};

    /* Recover system parameters */
    Command.Command = CMD_GET_TABLE_POSITION;
    Command.Identifier[0] = 'P';
    Command.Position[0] = BASE_PERIOD_INI_VALUE_TABLE_POSITION;
    ManageCommand(&Command,&Response);
    BasePeriod = Response.Data[0];

    Command.Position[0] = PULSES_FOR_O3_CALIBRATION_SEC_1_POSITION;
    ManageCommand(&Command,&Response);
    ModWidth = 1024 - Response.Data[0];

    SwitchOutputValves(OUTPUT_TO_CATALYSER);
	Delay_o3_ms(500);

    if( (GLB_fsm_o3.GenerationMode == O3_GENERATION_BASED_ON_O3_PHOTOSENSOR) )
    {
    	SendGeneratorStartSequence(BasePeriod, ModWidth, 8);
    }
    else
    {
      SendGeneratorStartSequence(BasePeriod, 0, ComputeDeadTime(BasePeriod)); /* 16%  ( current -> 1EA ) */
//    SendGeneratorStartSequence(BasePeriod, 0, 8);                           /* 100% ( current -> 50A ) */
//    SendGeneratorStartSequence(BasePeriod, 0, BasePeriod >> 2);             /* 32%  ( current -> 3C8 ) */
    }
}

#if 0
void SendGeneratorStartSecuence(uint16 ModBasePeriod, uint16 ModBaseWith, uint16 ModPeriod, uint16 ModWith)
{
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

//  GLB_fsm_o3.ManageCommandError = PROTOCOL_RET_SUCCESS; /* Changed on 22/12/2025  remove when properly tested*/

  Command.Command = CMD_SET_FAN;
  Command.Identifier[0] = FAN_0;
  Command.Value = FAN_ON;
  ManageCommand(&Command,&Response);

  /* Configure modulator base period */
  Command.Command = CMD_SET_MODULATOR_BASE_PERIOD;
  Command.Value = ModBasePeriod;
  ManageCommand(&Command,&Response);

  /* Configure modulator base width */
  Command.Command = CMD_SET_MODULATOR_BASE_WIDTH;
  Command.Value = ModBaseWith;
  ManageCommand(&Command,&Response);

  /* Configure modulator period */
  Command.Command = CMD_SET_MODULATOR_PERIOD;
  Command.Value = ModPeriod;
  ManageCommand(&Command,&Response);

  /* Configure modulator width */
  Command.Command = CMD_SET_MODULATOR_WIDTH;
  Command.Value = ModWith;
  ManageCommand(&Command,&Response);

  /* Modulator base start */
  Command.Command = CMD_MODULATOR_BASE_START;
  ManageCommand(&Command,&Response);

  /* Modulator start */
  Command.Command = CMD_MODULATOR_START;
  ManageCommand(&Command,&Response);
}
#endif

void SaveParameters(void)
{
	uint16 i = 0;
	uint8 error = 0;
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	Command.Command = CMD_GET_TABLE_POSITION;
	Command.Identifier[0] = 'P';        /* Parameters Table */

	if( GLB_fsm_o3.StartStorageSave() == 0 )
	{
		for(i = 0; i < MAX_PARAMETER_WORDS; i++)
		{
			Command.Position[0] = i;

			if (ManageCommand(&Command,&Response) != PROTOCOL_RET_COMMAND_NOT_SENT)
			{
				GLB_fsm_o3.WriteStorageLine(i, Response.Data[0]);
			}
			else
			{
				error = 1;
				break;
			}
		}
	}
	else
	{
		error = 1;
	}

	if(!error)
	{
		GLB_fsm_o3.StoptStorage();
//		GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_CALIBRATION_END];
		ChangeState(STATE_CALIBRATION_END);
		printf("Parameters saved in flash memory\n");   // SWV debug
	}
	else
	{
//		GLB_fsm_o3.ErrorState = SAVE_PARAMETERS_ERROR;
//		GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_ERROR];
		GotoError(SAVE_PARAMETERS_ERROR);
		printf("Error saving parameters in flash memory\n");   // SWV debug
	}

}

void LoadParameters(void)
{
	uint16 i = 0;
	uint8 error = 0;
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	if( GLB_fsm_o3.StartStorageLoad() == 0 )
	{
		Command.Command = CMD_SET_TABLE_POSITION;
		Command.Identifier[0] = 'P'; /* Parameters Table */

		/* Mark parameters table as not init */
		Command.Position[0] = TABLE_INITIALIZATION_STATE;
		Command.Value = 0;

		if (ManageCommand(&Command,&Response) != PROTOCOL_RET_COMMAND_NOT_SENT)
		{
			for(i = 1; i < MAX_PARAMETER_WORDS; i++)
			{
				if (GLB_fsm_o3.ReadStorageLine(i, (int32 *) &Response.Data[0]) == 0)
				{
					Command.Position[0] = i;
					Command.Value = Response.Data[0];

					if (ManageCommand(&Command,&Response) == PROTOCOL_RET_COMMAND_NOT_SENT)
					{
						printf("Error writing parameter %d to system\n", i);   // SWV debug
						error = 1;
						break;
					}
				}
				else
				{
					printf("Error reading parameter %d from storage\n", i);   // SWV debug
					error = 1;
					break;
				}
			}
		}
		else
		{
			printf("Error marking parameters table as not initialized\n");   // SWV debug
			error = 1;
		}
		GLB_fsm_o3.StoptStorage();
	}
	else
	{
		error = 1;
	}

	if(!error)
	{
		/* Mark parameters table as init */
		Command.Position[0] = TABLE_INITIALIZATION_STATE;
		Command.Value = PARAM_CALIBRATED_VALUES;
		ManageCommand(&Command,&Response);

		SendReset();
//		GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_CALIBRATION_END];
		ChangeState(STATE_CALIBRATION_END);
		printf("Parameters loaded from flash memory\n");   // SWV debug
	}
	else
	{
//		GLB_fsm_o3.ErrorState = LOAD_PARAMETERS_ERROR;
//		GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_ERROR];
		GotoError(LOAD_PARAMETERS_ERROR);
		printf("Error saving parameters in flash memory\n");   // SWV debug
	}
}

void Initialize(void)
{
  WelcomeMessage();

  Delay_o3_ms(1000);
  SendReset();
  Delay_o3_ms(1000);
  COM_O3_Init();  // clear reset response from UART buffer

  ClearErrors();
  TIMER_Start(&GLB_fsm_o3.WaitForServiceTimer, SERVICE_ENTRY_TIME_WINDOW);
  TIMER_Start(&GLB_fsm_o3.AnimationTimer, ANIMATION_PERIOD_MS);
  Beep();
}

int16 GetInstantAbsolutePressure(uint16 PressAtm)
{
  /* Use only the last pressure sample */
  if(GLB_fsm_o3.SensorDataIndex == 0)
  {
    return (GLB_fsm_o3.SensorData[ID_SENSOR_PRESSURE_1][NUM_SENSOR_SAMPLES-1]  - PressAtm);
  }
  else
  {
    return (GLB_fsm_o3.SensorData[ID_SENSOR_PRESSURE_1][GLB_fsm_o3.SensorDataIndex-1]  - PressAtm);
  }
}

void ManageFsmEvents(void)
{
//  Manage events
	if(GLB_FsmEvents.Enter)   { deb_printf(D_LEV_2, "Enter event processed\n");   GLB_fsm_o3.CurrentState->Enter(); }
	if(GLB_FsmEvents.Cancel)  { deb_printf(D_LEV_2, "Cancel event processed\n");  GLB_fsm_o3.CurrentState->Cancel(); }
	if(GLB_FsmEvents.Generic) { deb_printf(D_LEV_2, "Generic event processed\n"); GLB_fsm_o3.CurrentState->GenericEvent(); }
//  Clean events
	GLB_FsmEvents.Enter = 0;
	GLB_FsmEvents.Cancel = 0;
	GLB_FsmEvents.Generic = 0;
}

void KeepAlive(void)
{
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

  Command.Command = CMD_TEST_COM;
  ManageCommand(&Command,&Response);
}

#define READ_SENSORS_AT_ONCE

void SensorProcess(void)
{
	uint8 i = 0;
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	deb_printf(D_LEV_4,"Sensor process...\n");   // SWV debug

#ifdef READ_SENSORS_AT_ONCE
	Command.Command = CMD_GET_ALL_SENSORS;
	ManageCommand(&Command,&Response);

	/* loop over all sensors and store read data */
	for (i = 0; i < ID_SENSOR_MAX; i++)
	{
		GLB_fsm_o3.SensorData[i][GLB_fsm_o3.SensorDataIndex] = Response.Data[i];
	}

#else
	{
		Command.Command = CMD_GET_SENSOR;

		/* loop over all ID_SENSOR_E values to read all sensors */
		for (i = 0; i < ID_SENSOR_MAX; i++)
		{
			//    UTILS_uint8ToHexString(i,Command.Identifier,FALSE);
			snprintf(Command.Identifier, 5, "%X", i);
			if (ManageCommand(&Command,&Response) == PROTOCOL_RET_SUCCESS)
			{
				/* Store read data */
				GLB_fsm_o3.SensorData[i][GLB_fsm_o3.SensorDataIndex] = Response.Data[0];
			}
			else
			{
				/* If last command wasn't sent, there was an error so stop sending */
				break;
			}
		}
#endif
	if(GLB_fsm_o3.GenerationMode == O3_GENERATION_BASED_ON_TUBE_CALIBRATION)
	{
		GLB_fsm_o3.SensorData[ID_SENSOR_OZONE][GLB_fsm_o3.SensorDataIndex] =  GLB_fsm_o3.ConfiguredO3Concentration * 8;
	}

	if(++GLB_fsm_o3.SensorDataIndex == NUM_SENSOR_SAMPLES)
	{
		GLB_fsm_o3.SensorDataIndex = 0;
		GLB_fsm_o3.SensorDataFilled = TRUE;
	}
}

#if 0
	void SensorMeanComputation(void)
	{
	  uint8 i=0;
	  uint32 FlowSum=0;
	  uint32 OzoneSum=0;

	  for (i=0;i<NUM_SENSOR_SAMPLES;i++)
	  {
	    FlowSum  += GLB_fsm_o3.SensorData[ID_SENSOR_FLOW][i];
	    OzoneSum += GLB_fsm_o3.SensorData[ID_SENSOR_OZONE][i];
	  }
	  GLB_fsm_o3.CurrentMeanFlow  = FlowSum >> 4;
	  GLB_fsm_o3.CurrentMeanOzone = OzoneSum >> 4;
	}

#endif

void SensorMeanComputation(void)
{
	uint8  i = 0;
	uint32 FlowSum = 0;
	uint32 OzoneSum = 0;
	uint8  totalSamples = GLB_fsm_o3.SensorDataFilled ? NUM_SENSOR_SAMPLES : GLB_fsm_o3.SensorDataIndex;

	for (i = 0; i < totalSamples; i++)
	{
		FlowSum  += GLB_fsm_o3.SensorData[ID_SENSOR_FLOW][i];
		OzoneSum += GLB_fsm_o3.SensorData[ID_SENSOR_OZONE][i];
	}
	GLB_fsm_o3.CurrentMeanFlow  = FlowSum / totalSamples;
	GLB_fsm_o3.CurrentMeanOzone = OzoneSum / totalSamples;
}

void CheckFlowStability(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	Command.Command = CMD_STATE_FLOW;
	ManageCommand(&Command,&Response);

	if(Response.Data[0] == O3_FLOW_REACHED)
	{
		GLB_fsm_o3.StableFlow = TRUE;
	}
	else
	{
		GLB_fsm_o3.StableFlow = FALSE;
	}
#if 0 // delete definitively when properly tested (21/21/26)
	if(IncreaseFlowPrecisionCounter)
	{
		IncreaseFlowPrecisionCounter--;
		GLB_fsm_o3.StableFlow = FALSE;
	}
#endif

	//  if(TIMER_State(&GLB_fsm_o3.AuxTimer) == TIMER_STATE_STARTED)
	if( IS_MANUAL_RESTARTING_GAP() )
	{
		SkipAdjusting();
		GLB_fsm_o3.StableFlow = TRUE;
	}
}

void CheckOzoneStability(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};
	static uint8 NoStableCounter;

	Command.Command = CMD_STATE_OZONE;
	ManageCommand(&Command,&Response);

	GLB_fsm_o3.StableOzone = FALSE;

	if( IS_SYRINGE_AUTO_MODE() )
	{
		if(Response.Data[0] == O3_CONCENTRATION_REACHED_ACCURATE)
		{
			GLB_fsm_o3.StableOzone = TRUE;
			NoStableCounter = 0;
		}
	}
	else if((Response.Data[0] == O3_CONCENTRATION_REACHED) || (Response.Data[0] == O3_CONCENTRATION_REACHED_ACCURATE))
	{
		GLB_fsm_o3.StableOzone = TRUE;
		NoStableCounter = 0;
	}

//	if(GLB_fsm_o3.CurrentState->State_ID == STATE_O3_GENERATING)
	if( IS_GENERATING() )
	{
		if(GLB_fsm_o3.StableOzone == FALSE)
		{
			if(++NoStableCounter <  O3_ERROR_DETECTION_ITERATIONS)
			{
				GLB_fsm_o3.StableOzone = TRUE;
			}
		}
		if( IS_SYRINGE_AUTO_MODE() )
		{
			GLB_fsm_o3.StableOzone = TRUE;
		}
	}

	//if(TIMER_State(&GLB_fsm_o3.AuxTimer) == TIMER_STATE_STARTED)
	if( IS_MANUAL_RESTARTING_GAP() )
	{
		SkipAdjusting();
		GLB_fsm_o3.StableOzone = TRUE;
	}
}

void UpdateProcess(void)
{
    if( GLB_fsm_o3.StableFlow && GLB_fsm_o3.StableOzone )
    {
      if( IS_ADJUSTING() )
      {
        TIMER_Stop(&GLB_fsm_o3.AdjustTimer);
        GLB_fsm_o3.PressAdjust = GetInstantAbsolutePressure(GLB_fsm_o3.PressAtm);
        ResetCumulativeValues();
//        GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_O3_GENERATING];
		ChangeState(STATE_O3_GENERATING);

    	if( IS_MANUAL_NOT_PUSHED() )
    	{
    		KeyOnRelease();
    	}
      }
      else if( IS_GENERATING() )
      {
        GLB_fsm_o3.CurrentState->Animate(); // Speed up critical decisions in relation with measurements
      }

    }
    else if( IS_ADJUSTING_EXPIRED() || IS_GENERATING() ) /* Look for error situation if no stable measurements */
    {
      SendStopCommand();
      if(!GLB_fsm_o3.StableFlow)
      {
        GLB_fsm_o3.ErrorState = FLOW_ERROR;
        GotoError(FLOW_ERROR);
      }
      else if (!GLB_fsm_o3.StableOzone)
      {
        GLB_fsm_o3.ErrorState = OZONE_ERROR;
        GotoError(OZONE_ERROR);
      }
//      GLB_fsm_o3.CurrentState->Error();
//      GotoError();
    }
}

/* (24/2/2016)                                                                  *
 * The programmer of this patch is not agree with this mode of device behavior. *
 * O3 is supplied to output before flow and O3 levels where stable.             *
 *                                                                              *
 * TODO: CONSIDER AVOIDING THIS PRACTISE AND RETURNING TO MANUAL WITHOUT        *
 * STOPPING GENERATION DURING KeyOnRelease().                                   *
 */
void SkipAdjusting(void)
{
  uint8 i = 0;

  for (i=0;i<NUM_SENSOR_SAMPLES;i++)
  {
    GLB_fsm_o3.SensorData[ID_SENSOR_FLOW][i]  = ((uint16)(GLB_fsm_o3.ConfiguredFlow)) << 3;
    GLB_fsm_o3.SensorData[ID_SENSOR_OZONE][i] = ((uint16)(GLB_fsm_o3.ConfiguredO3Concentration)) << 3;
  }
}

void LaunchTherapy(void)
{
	deb_printf(D_LEV_4, "Therapy (%d) launched @ %d ug/ml, %d l/h, %d min. Total secs: %lu\n", GLB_fsm_o3.Option, GLB_fsm_o3.ConfiguredO3Concentration, GLB_fsm_o3.ConfiguredFlow, GLB_fsm_o3.ConfiguredTime, GLB_Time);
	deb_printf(D_LEV_4, "                        %lu vol, %d ug, %d pressure\n",  GLB_fsm_o3.ConfiguredVolume,	GLB_fsm_o3.ConfiguredDose, GLB_fsm_o3.ConfiguredPressure);
	deb_printf(D_LEV_4, "                        %d vac secs, %d vac pressure\n", GLB_fsm_o3.ConfiguredVacuumTime, GLB_fsm_o3.ConfiguredVacuumPressure);
	deb_printf(D_LEV_4, "                        %d pressire threshold\n", GLB_fsm_o3.PressThreshold);

	if( IS_DENTAL_MODE() )
	{
		GLB_fsm_o3.PressThreshold = GLB_fsm_o3.ConfiguredPressure + GLB_fsm_o3.PressThreshold;
	}

	if( IS_VACUUM_TIME_MODE() || IS_VACUUM_PRESSURE_MODE() || IS_CLOSED_BAG_MODE() )
	{
		ResetCumulativeValues(); // useful in the closed bag mode to distinguish between the first and second vacuum
		GotoVacuumGeneration();
	}
	else if( IS_DENTAL_MODE() )
	{
		ChangeState(STATE_WAITING_EXTERNAL_STUFF);
	}
	else
	{
		GLB_fsm_o3.SalineCycle = SALINE_MIXING_CYCLE;

		ChangeState(STATE_TUNING_O3_SENSOR);
	}
//	StartGeneration(FLOW_NOT_STARTED_YET);
}

#define _QUICK_START


void FSM_ProcessEvents()
{
//	TestEprom();
	GLB_FSM_ProcessEvent_Count++;

	/* Verify state change event */
	if(GLB_fsm_o3.CurrentState != GLB_fsm_o3.LastState)
	{
		/* Update LastState first, then execute Entry() to get a right
		 * debug log in relation with Send(X) and Rec(X) messages*/
		deb_printf(D_LEV_4, "State machine change: %d\n", GLB_fsm_o3.CurrentState->State_ID);  // SWV debug
		GLB_fsm_o3.LastState = GLB_fsm_o3.CurrentState;
		GLB_fsm_o3.CurrentState->Entry();
	}

	/* Manage FSM events */
	ManageFsmEvents();

	/* Process Animation Timer event */
	if(TIMER_State(&GLB_fsm_o3.AnimationTimer) == TIMER_STATE_EXPIRED)
	{
		TIMER_Start(&GLB_fsm_o3.AnimationTimer, ANIMATION_PERIOD_MS);
		GLB_fsm_o3.CurrentState->Animate();
	}

	/* Service menu or normal processing management */
	if( IS_WAITING_FOR_SERVICE_WINDOW_EXPIRED() )  /* Service menu entry time window expired */
	{
		if( IS_NOT_ERROR_STATE() )
		{
			TIMER_Stop(&GLB_fsm_o3.WaitForServiceTimer);
#ifdef _QUICK_START
			GLB_fsm_o3.MaxAvailableFlow = MAX_FLOW_VALUE;
			ResolveGenerationMode();
			GotoRepose();
#else
			printf("Going to initial check...\n");  // SWV debug
			GotoInitialCheck();
#endif
		}
	}
	else if(IS_NORMAL_PROCESSING()) /* Normal processing when not in service menu */
	{
		DetectFalseFlowSituation();
		DetectHighVoltageDisconnection();

		/* Sensor data reading */
		if(TIMER_State(&GLB_fsm_o3.DataReadTimer) == TIMER_STATE_EXPIRED)
		{
			TIMER_Start(&GLB_fsm_o3.DataReadTimer, HI_SPEED_DATA_READ_PERIOD_MS);
			SensorProcess();
			SensorMeanComputation();
			CheckFlowStability();
			CheckOzoneStability();
			UpdateProcess();
		}

		/* Generation cleaning management */
//		if (TIMER_State(&GLB_fsm_o3.CleanGeneratorTimer) == TIMER_STATE_EXPIRED)
//		{
//			GeneratorCleanStop();
//		}
	}

	/* Keep alive processing */
	if(TIMER_State(&GLB_fsm_o3.KeepAliveTimer) == TIMER_STATE_EXPIRED)
	{
		(GLB_fsm_o3.TemperatureMonitoring) ? SetFanDependingOnTemperature(FAN_ON_TEMPERATURE) : KeepAlive();

		TIMER_Start(&GLB_fsm_o3.KeepAliveTimer, COMMUNICATION_TEST_PERIOD_MS);
	}
}

////////////////////////////////// put therapies together///////////////////////////
uint32 ComputeSalineTime(uint16 Flow)
{
  uint32 Time;
  /*Time = [ Volume (ml) / ((Flow (l/h) * 1000) / 3600) ] * 5 = (Volume (ml) * 36 * 5 ) / (Flow (l/h) * 10) =
    = (Volume (ml) * 36) / (Flow (l/h) * 2) = (Volume (ml) * 18) / Flow (l/h) -> seconds */
  Time = GLB_fsm_o3.ConfiguredVolume;
  Time = Time * 18;
  Time = Time / Flow;
  return Time;
}

#define TEST_CLOSED_BAG

void ResetTiming(void)
{
	if( IS_INSUFLATION_R_MODE() || IS_DOSE_MODE() || IS_VACUUM_PRESSURE_MODE() )
	{
		GLB_fsm_o3.RemainingHours   = (uint8)((GLB_Time / 3600));
		GLB_fsm_o3.RemainingMinutes = (uint8)((GLB_Time % 3600) / 60);
		GLB_fsm_o3.RemainingSeconds = (uint8)((GLB_Time % 3600) % 60);
		GLB_fsm_o3.ConfiguredTime = GLB_fsm_o3.RemainingMinutes;
	}
	else if ( IS_VACUUM_TIME_MODE() || IS_OPEN_BAG_VACUUM() || IS_CLOSED_BAG_VACUUM() )
	{
		GLB_fsm_o3.RemainingHours   = 0;
		GLB_fsm_o3.RemainingMinutes = 0;
		GLB_fsm_o3.RemainingSeconds = GLB_fsm_o3.ConfiguredVacuumTime;
		GLB_Time = GLB_fsm_o3.ConfiguredVacuumTime; // seconds
	}
	else if ( IS_CLOSED_BAG_ADJUSTING() )
	{
		GLB_Time = (GLB_fsm_o3.ConfiguredVolume * 3600) / GLB_fsm_o3.ConfiguredFlow; // seconds
		GLB_fsm_o3.RemainingHours   = 0;
		GLB_fsm_o3.RemainingMinutes = (uint8)((GLB_Time % 3600) / 60) - 1;
		GLB_fsm_o3.RemainingSeconds = 59;

#ifdef	TEST_CLOSED_BAG
		GLB_Time = 10;
		GLB_fsm_o3.RemainingMinutes = 0;
		GLB_fsm_o3.RemainingSeconds = 10;
#endif
	}
	else if ( IS_CLOSED_BAG_WAITING_THERAPY_TIME() )
	{
		GLB_fsm_o3.RemainingHours   = 0;
		GLB_fsm_o3.RemainingMinutes = GLB_fsm_o3.ConfiguredTime - 1;
		GLB_fsm_o3.RemainingSeconds = 59;
		GLB_Time = GLB_fsm_o3.ConfiguredTime * 60; // seconds
	}
	else if ( IS_SALINE_MIXING_CYCLE())
	{
		GLB_Time = ComputeSalineTime(GLB_fsm_o3.ConfiguredFlow);

        GLB_fsm_o3.RemainingHours   = (uint8)((GLB_Time / 3600));
        GLB_fsm_o3.RemainingMinutes = (uint8)((GLB_Time % 3600) / 60);
        GLB_fsm_o3.RemainingSeconds = (uint8)((GLB_Time % 3600) % 60);
	}
	else
	{
		GLB_fsm_o3.RemainingHours   = 0;
		GLB_fsm_o3.RemainingMinutes = GLB_fsm_o3.ConfiguredTime - 1;
		GLB_fsm_o3.RemainingSeconds = 59;
		GLB_Time = GLB_fsm_o3.ConfiguredTime * 60; // seconds
	}
}

void ResetCumulativeValues(void)
{
//	GLB_fsm_o3.FirstManualModeIteraion = 0;
	GLB_fsm_o3.CurrentMeanOzone = 0;
	/*GLB_fsm_o3.CurrentMeanFlow = 0;*/
	GLB_fsm_o3.CurrentOutputVolume = 0;
	GLB_fsm_o3.CurrentTotalDose = 0;
	GLB_fsm_o3.FreezeRemainingTime = 0;
	GLB_fsm_o3.CumulatedOutputVolume = 0;
	GLB_fsm_o3.PressureAlertDone = FALSE;
	GLB_fsm_o3.DentalSupplying = FALSE;

	ResetTiming();
}

void GotoGenerating(void)
{
	int16  CurrentOperatingPressure = 0;
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	deb_printf(D_LEV_2, "Generating (end of adjusting)...\n");

	//ResetCumulativeValues(); /* Moved close to generating state change line to avoid information glitches at the very begging of generation stage*/
	GLB_fsm_o3.ReferenceOutputVolume = GetCumulatedVolume();

	CurrentOperatingPressure = GetInstantAbsolutePressure(GLB_fsm_o3.PressAtm);
	SYRINGE_InitProcess(&GLB_fsm_o3.SyringeCtrl, CurrentOperatingPressure);

	/* Delayed O3 start to avoid O3 peak due to low speed PID adjust */
	//if((GLB_fsm_o3.Option != SYRINGE_MODE) && (GLB_fsm_o3.ConfiguredFlow < MAX_FLOW_LEVEL_WITHOUT_O3_ON_GENERATION_START))
	if(0) /* Delayed O3 start to avoid O3 peak due to low speed PID adjust */ /* Always */
	{
		if( IS_GENERATING() )
		{
			Command.Command = CMD_START_OZONE;
			ManageCommand(&Command,&Response);
			TIMER_Start(&GLB_fsm_o3.HihgVoltDisconectTimer, HIGH_VOLT_DETECTION_TIMEOUT);
		}
	}

	if( IS_INSUFLATION_R_MODE() || IS_INSUFLATION_V_MODE() || IS_MANUAL_NOT_PUSHED() || IS_SYRINGE_MANUAL_MODE() )
	{
		//  KeyOnPush();
		SwitchOutputValves(OUTPUT_TO_CATALYSER);
		GLB_fsm_o3.CumulatedOutputVolume = GLB_fsm_o3.CurrentOutputVolume;
		GLB_fsm_o3.FreezeRemainingTime = 1;
	}
	else if( IS_DENTAL_MODE() )
	{
		GLB_fsm_o3.DentalSupplyDetectionPressure = CurrentOperatingPressure + (CurrentOperatingPressure >> 1); /* Add 50% of current pressure  */

		if(GLB_fsm_o3.DentalSupplyDetectionPressure < MINIMUM_PRESSURE_THRESHOLD_FOR_DENTAL_SUPPLYING_DETECTION)
		{
			GLB_fsm_o3.DentalSupplyDetectionPressure = MINIMUM_PRESSURE_THRESHOLD_FOR_DENTAL_SUPPLYING_DETECTION;
		}
	}
	else
	{
		SwitchOutputValves(OUTPUT_TO_EXTERIOR);
	}
}

void GotoUserCancelled(void)
{
	SendStopCommand();
	StopManualGap();
	TIMER_Stop(&GLB_fsm_o3.DataReadTimer);

	//  if (GLB_fsm_o3.CurrentState->State_ID != STATE_ERROR)
	if( IS_NOT_ERROR_STATE() )
	{
		if( IS_SYRINGE_AUTO_MODE())
		{
//			GLB_fsm_o3.DepressureReturnState = STATE_USER_CANCELLED;
			GotoDepressure(STATE_USER_CANCELLED);
		}
		else if( IS_CONTINUOUS_MODE() )
		{
//			GLB_fsm_o3.WashingReturnState = STATE_USER_CANCELLED;
			GotoWashing(STATE_USER_CANCELLED);
		}
		else
		{
//			GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_USER_CANCELLED];
			ChangeState(STATE_USER_CANCELLED);
		}
	}
}

void GotoEndOperation(void)
{
    deb_printf(D_LEV_2, "End of operation...\n");

	SendStopCommand();
	StopManualGap();
	TIMER_Stop(&GLB_fsm_o3.DataReadTimer);

	if( IS_CONTINUOUS_MODE() )
	{
//		GLB_fsm_o3.WashingReturnState = STATE_COMPLETED;
		GotoWashing(STATE_COMPLETED);
	}
	else if( IS_SYRINGE_AUTO_MODE() )
	{
//		GLB_fsm_o3.DepressureReturnState = STATE_COMPLETED;
		GotoDepressure(STATE_COMPLETED);
	}
	else if( IS_CLOSED_BAG_GENERATING() )
	{
//		GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_WAITING_THERAPY_TIME];
		ChangeState(STATE_WAITING_THERAPY_TIME);
		ResetTiming();
	}
	else if ( IS_CLOSED_BAG_WAITING_THERAPY_TIME() )
	{
		GotoVacuumGeneration();
	}
	else if ( IS_OPEN_BAG_MODE() )
	{
		GotoVacuumGeneration();
	}
	else if( IS_SALINE_MIXING_CYCLE() )
	{
//		GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_WAITING_EXTERNAL_STUFF];
		ChangeState(STATE_WAITING_EXTERNAL_STUFF);
	}
	else
	{
//		GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_COMPLETED];
		ChangeState(STATE_COMPLETED);
	}
}

void GotoOverPressure(void)
{
	deb_printf(D_LEV_4, "Overpressure...\n");

	SendStopCommand();
	StopManualGap();
	TIMER_Stop(&GLB_fsm_o3.DataReadTimer);

	if ( IS_SYRINGE_AUTO_MODE())
	{
//		GLB_fsm_o3.DepressureReturnState = STATE_COMPLETED;
		GotoDepressure(STATE_COMPLETED);
	}
	else
	{
//		GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_OVERPRESSURE];
		ChangeState(STATE_OVERPRESSURE);
	}
}

void StartGenerationInitialSettings(void)
{
	uint16 percentage;
/*	uint32 aux;*/

//	IncreaseFlowPrecisionCounter = 0;   delete definitively when properly tested (21/21/26)
	GLB_fsm_o3.SyringeDetected = 0;
	GLB_fsm_o3.KeyEnterPushed = NOT_PUSHED;
	GLB_fsm_o3.InsufflationState = INSUFFLATION_STATE_PAUSED;
	GLB_fsm_o3.SyringeManualState = SYRINGE_MANUAL_STATE_PAUSED;
/*
	if(GLB_fsm_o3.Option == CONTINUOUS_MODE)
	{
		GLB_Time = GLB_fsm_o3.ConfiguredTime * 60;
	}
	else if( (GLB_fsm_o3.Option == MANUAL_MODE) || (GLB_fsm_o3.Option == DENTAL_MODE) )
	{
		SetMaxAllowedTime();
		GLB_fsm_o3.ConfiguredTime = GLB_fsm_o3.MaxAllowedTime;
		GLB_Time = GLB_fsm_o3.ConfiguredTime * 60;
	}
	else if(GLB_fsm_o3.Option == DOSE_MODE)
	{
		GLB_fsm_o3.ConfiguredFlow = DEFAULT_FLOW_VALUE;
		aux = GLB_fsm_o3.ConfiguredDose / GLB_fsm_o3.ConfiguredO3Concentration; //  milliliters needed
		aux = (aux * 3600u) / ((uint32_t)GLB_fsm_o3.ConfiguredFlow * 1000u);  // seconds needed
		GLB_Time = aux;
	}
*/
	percentage = (GLB_fsm_o3.ConfiguredFlow > 15) ? FLOW_DEVIATION_PERCENTAGE : EXTENDED_FLOW_DEVIATION_PERCENTAGE;
    GLB_fsm_o3.MaxFlowDeviation = (uint16)((uint16)(((uint16)GLB_fsm_o3.ConfiguredFlow << 3) * percentage) ) / 100;

    /* Start periodic sensor data reading environment */
	memset(GLB_fsm_o3.SensorData,0,sizeof(GLB_fsm_o3.SensorData));
	GLB_fsm_o3.SensorDataIndex = 0;
	GLB_fsm_o3.SensorDataFilled = FALSE;
	GLB_fsm_o3.StableFlow = FALSE;
	GLB_fsm_o3.StableOzone = FALSE;
}

void StartGeneration(uint8 FlowRunningState)
{
  ClearErrors();

  if( IS_DENTAL_MODE() )
  {
	  SwitchOutputValves(OUTPUT_TO_EXTERIOR);
  }

  SendGenerationCommands(GLB_fsm_o3.GenerationMode, FlowRunningState);

  ChangeState(STATE_ADJUSTING);

  StartGenerationInitialSettings();

  if(GLB_fsm_o3.ErrorState == NO_ERROR)
  {
	  TIMER_Start(&GLB_fsm_o3.DataReadTimer, HI_SPEED_DATA_READ_PERIOD_MS);
  	  TIMER_Start(&GLB_fsm_o3.AdjustTimer, ADJUST_TIMEOUT_MS);
  }

  TIMER_Start(&GLB_fsm_o3.HihgVoltDisconectTimer, HIGH_VOLT_DETECTION_TIMEOUT);
}

void UpdateSecondSelectorValue(void)
{
	uint32 aux;

	if ( IS_INSUFLATION_R_MODE() || IS_SALINE_MODE() )
	{
		/*Time = Concentration (ml) / ((Flow (l/h) * 1000) / 3600) = Concentration (ml) * 36 / (Flow (l/h) * 10) -> seconds */
		GLB_Time = (int32)(GLB_fsm_o3.ConfiguredVolume);
		GLB_Time = GLB_Time * 36;
		GLB_Time = GLB_Time / GLB_fsm_o3.ConfiguredFlow;
		GLB_Time = GLB_Time / 10;

		printf("GLB_fsm_o3.ConfiguredFlow: %d\n", GLB_fsm_o3.ConfiguredFlow);
		printf("GLB_fsm_o3.ConfiguredVolume: %lu\n", GLB_fsm_o3.ConfiguredVolume);
		printf("GLB_Time (seconds): %lu\n", GLB_Time);
	}
	else if ( IS_VACUUM_TIME_MODE() || IS_CLOSED_BAG_MODE())
	{
		GLB_Time = GLB_fsm_o3.ConfiguredVacuumTime;
	}
	else if ( IS_VACUUM_PRESSURE_MODE() )
	{
		GLB_Time = 5 * 60;
		printf("GLB_Time (seconds): %lu\n", GLB_Time);
	}
	else if( IS_MANUAL_MODE() || IS_DENTAL_MODE() )
	{
		SetMaxAllowedTime();
		GLB_Time = GLB_fsm_o3.MaxAllowedTime * 60;
	}
	else if( IS_DOSE_MODE() )
	{
		GLB_fsm_o3.ConfiguredFlow = DEFAULT_FLOW_VALUE;
		aux = GLB_fsm_o3.ConfiguredDose / GLB_fsm_o3.ConfiguredO3Concentration; //  milliliters needed
		aux = (aux * 3600u) / ((uint32)GLB_fsm_o3.ConfiguredFlow * 1000u);  // seconds needed
		GLB_Time = aux;
	}
	else if ( IS_INSUFLATION_V_MODE() ) // consider this as default else in the case that more therapies are added
	{
		GLB_Time = GLB_fsm_o3.ConfiguredTime * 60;
		GLB_fsm_o3.ConfiguredVolume = (GLB_fsm_o3.ConfiguredFlow * GLB_Time * 10) / 36; // ml
	}
	else if ( IS_CONTINUOUS_MODE() || IS_OPEN_BAG_MODE() || IS_INSUFLATION_V_MODE() ) // consider this as default else in the case that more therapies are added
	{
		GLB_Time = GLB_fsm_o3.ConfiguredTime * 60;
	}
}

void KeyOnPushForSaline(void)
{
	if ( IS_SALINE_MIXING_CYCLE() )
	{
		GotoEndOperation();
	}
}

void KeyOnPushForOpenBag(void)
{
	if( IS_OPEN_BAG_MODE() )
	{
		GotoEndOperation();
	}
}

void KeyOnPushForClosedBag(void)
{
	if( IS_CLOSED_BAG_MODE() )
	{
		GotoEndOperation();
	}
}

void KeyOnPushForInsufflation(void)
{
  if ( IS_INSUFLATION_R_MODE() || IS_INSUFLATION_V_MODE() )
  {
    if(GLB_fsm_o3.InsufflationState == INSUFFLATION_STATE_ON)
    {
      SwitchOutputValves(OUTPUT_TO_BOTH);
      Delay_o3_ms(150);
      SwitchOutputValves(OUTPUT_TO_CATALYSER);
      GLB_fsm_o3.CumulatedOutputVolume = GLB_fsm_o3.CurrentOutputVolume;
      GLB_fsm_o3.FreezeRemainingTime = 1;
      GLB_fsm_o3.InsufflationState = INSUFFLATION_STATE_PAUSED;
    }
    else
    {
      SwitchOutputValves(OUTPUT_TO_EXTERIOR);
      GLB_fsm_o3.ReferenceOutputVolume = GetCumulatedVolume();
      GLB_fsm_o3.FreezeRemainingTime = 0;
      GLB_fsm_o3.InsufflationState = INSUFFLATION_STATE_ON;
    }
  }
}

void KeyOnPushForManual(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	if( IS_MANUAL_NOT_PUSHED() )
	{
        SendGenerationCommands(GLB_fsm_o3.GenerationMode, FLOW_NOT_STARTED_YET);
		Command.Command = CMD_START_OZONE;
		ManageCommand(&Command,&Response);
		SwitchOutputValves(OUTPUT_TO_EXTERIOR);

		SkipAdjusting();
		StartManualGap();
		GLB_fsm_o3.KeyEnterPushed = PUSHED;
		GLB_fsm_o3.FreezeRemainingTime = 0;
		GLB_fsm_o3.ReferenceOutputVolume = GetCumulatedVolume();
        TIMER_Start(&GLB_fsm_o3.HihgVoltDisconectTimer, HIGH_VOLT_DETECTION_TIMEOUT);
	}
}

void KeyOnPushForSyringeManual(void)
{
	  if ( IS_SYRINGE_MANUAL_MODE() )
	  {
	      SwitchOutputValves(OUTPUT_TO_EXTERIOR);
	      GLB_fsm_o3.ReferenceOutputVolume = GetCumulatedVolume();
	      GLB_fsm_o3.FreezeRemainingTime = 0;
	      GLB_fsm_o3.SyringeManualState = SYRINGE_MANUAL_STATE_ON;
	  }
}

void KeyOnPush(void)
{
  deb_printf(D_LEV_4, "KeyOnPush() execution...\n");

  KeyOnPushForManual();
  KeyOnPushForInsufflation();
  KeyOnPushForClosedBag();
  KeyOnPushForOpenBag();
  KeyOnPushForSyringeManual();
  KeyOnPushForSaline();
}

void KeyOnRelease(void)
{
  deb_printf(D_LEV_4, "KeyOnRelease() execution...\n");

  if ( IS_MANUAL_MODE() )
  {
    SendStopCommand();
  }
  if ( IS_SYRINGE_MANUAL_MODE() )
  {
	  SwitchOutputValves(OUTPUT_TO_BOTH);
	  Delay_o3_ms(150);
	  GLB_fsm_o3.SyringeManualState = SYRINGE_MANUAL_STATE_PAUSED;
  }
  if ( IS_MANUAL_MODE() || IS_SYRINGE_MANUAL_MODE() )
  {
	SwitchOutputValves(OUTPUT_TO_CATALYSER);
	GLB_fsm_o3.KeyEnterPushed = NOT_PUSHED;
	GLB_fsm_o3.FreezeRemainingTime = 1;
	GLB_fsm_o3.CumulatedOutputVolume = GLB_fsm_o3.CurrentOutputVolume;
  }
}

void ReleasePressureDuringGeneration(void)
{
	deb_printf(D_LEV_2, "ReleasePressure...\n");

	GLB_fsm_o3.DepressureSeconds = 2; // force graphic interface to show the depressurizing icon

    SwitchOutputValves(OUTPUT_TO_BOTH);
    Delay_o3_ms(150);
    SwitchOutputValves(OUTPUT_TO_EXTERIOR);
}

void PauseAndReleasePressure(void)
{
	if( IS_INSUFLATION_R_MODE_ON() )
    {
    	KeyOnPushForInsufflation();
    }
    else
    {
        SwitchOutputValves(OUTPUT_TO_BOTH);
        Delay_o3_ms(150);
        SwitchOutputValves(OUTPUT_TO_CATALYSER);
	}
}

void UpdateGeneratingForDental(void)
{
	/* manage pressure alert */
	if(GLB_fsm_o3.CurrentOperatingPressure > DENTAL_PRESSURE_ALERT_THRESHOLD)
	{
		if( !GLB_fsm_o3.PressureAlertDone )
		{
			Beep();
			Beep();
			Beep();
			GLB_fsm_o3.PressureAlertDone = TRUE;
		}
	}
	else
	{
		GLB_fsm_o3.PressureAlertDone = FALSE;
	}

	/* manage configured pressure reached */
	if(GLB_fsm_o3.CurrentOperatingPressure > GLB_fsm_o3.ConfiguredPressure)
	{
		ReleasePressureDuringGeneration();
	}
	else
	{
		/* manage supplying detection*/
		if(GLB_fsm_o3.CurrentOperatingPressure < GLB_fsm_o3.DentalSupplyDetectionPressure)
		{
			if(GLB_fsm_o3.DentalSupplying == TRUE)
			{
				GLB_fsm_o3.CumulatedOutputVolume = GLB_fsm_o3.CurrentOutputVolume;
			}
			GLB_fsm_o3.DentalSupplying = FALSE;
		}
		else
		{
			if(GLB_fsm_o3.DentalSupplying == FALSE)
			{
				Beep();
				GLB_fsm_o3.ReferenceOutputVolume = GetCumulatedVolume();
			}
			GLB_fsm_o3.DentalSupplying = TRUE;
		}
	}
}

void UpdateGeneratingForSyringe(void)
{
	uint16 SyringeVol;
	int8   SyringeDebugValues[20];
	uint16 ElapsedSeconds = (((uint16)(GLB_fsm_o3.ConfiguredTime) - (uint16)(GLB_fsm_o3.RemainingMinutes)) - 1) * 60 + (59 - (uint16)(GLB_fsm_o3.RemainingSeconds));

	/* Detect pressure increase due to piston resistance */
	if ( (GLB_fsm_o3.CurrentOperatingPressure > PISTON_PRESSURE_INITIAL_PEAK) && !GLB_fsm_o3.SyringeDetected )
	{
		GLB_fsm_o3.SyringeDetected = 1;
		deb_printf(D_LEV_4, "Syringe detected...\n"); // SWV Debug
		deb_printf(D_LEV_4, "Atm pressure: %d: \n", GLB_fsm_o3.PressAtm); // SWV Debug
	}

	/* Syringe detection during first seconds after valve opening */
	if ((ElapsedSeconds >= 2) && !GLB_fsm_o3.SyringeDetected)
	{
		SendStopCommand();
		GotoError(LEAKAGE_ERROR);
		deb_printf(D_LEV_4, "No syringe detected (1) ...\n"); // SWV Debug
		deb_printf(D_LEV_4, "Atm pressure: %d: \n", GLB_fsm_o3.PressAtm); // SWV Debug
	}
	else
	{
		/* Syringe filling control */

		/* Syringe removed supervision during filling process */
		 if((GLB_fsm_o3.SyringeDetected) && (abs(GLB_fsm_o3.CurrentOperatingPressure - GLB_fsm_o3.PressAdjust) < FILL_PRESSURE_DEVIATION))
		{
			SendStopCommand();
			GotoError(LEAKAGE_ERROR);
			deb_printf(D_LEV_4, "No syringe detected (2) ...\n"); // SWV Debug
			deb_printf(D_LEV_4, "Atm pressure: %d: \n", GLB_fsm_o3.PressAtm); // SWV Debug
			deb_printf(D_LEV_4, "Operating pressure: %d: \n", GLB_fsm_o3.CurrentOperatingPressure); // SWV Debug
			deb_printf(D_LEV_4, "Adjust pressure: %d: \n", GLB_fsm_o3.PressAdjust); // SWV Debug
		}

		/* Syringe end of filling supervision */
		if(SYRINGE_Process(&GLB_fsm_o3.SyringeCtrl, GLB_fsm_o3.CurrentOperatingPressure))
		{
			/* Syringe filled */
			/* Send debug information */
			snprintf(SyringeDebugValues, 5, "%04X", (uint16)GLB_fsm_o3.SyringeCtrl.FillingTime);
			snprintf(&SyringeDebugValues[4], 5, "%04X", (uint16)GLB_fsm_o3.SyringeCtrl.DetachTime);
			snprintf(&SyringeDebugValues[8], 5, "%04X", (uint16)GLB_fsm_o3.SyringeCtrl.PressStableTime);
			snprintf(&SyringeDebugValues[12], 5, "%04X", (uint16)GLB_fsm_o3.SyringeCtrl.OperatingPressureBefore);
			SyringeDebugValues[16] = 0;
			COM_O3_PutString((uint8 *)SyringeDebugValues);

			/* Determine syringe type */
			switch (SYRINGE_Type(&GLB_fsm_o3.SyringeCtrl))
			{
			case SYRINGE_ID_5ML:  SyringeVol = 5;   break;
			case SYRINGE_ID_10ML: SyringeVol = 10;  break;
			case SYRINGE_ID_20ML: SyringeVol = 20;  break;
			case SYRINGE_ID_50ML: SyringeVol = 50;  break;
			default:              SyringeVol = 100; break;
			}
			GLB_fsm_o3.CurrentTotalDose = (uint32) (SyringeVol * GLB_fsm_o3.ConfiguredO3Concentration);
			GLB_fsm_o3.CurrentOutputVolume = SyringeVol;
			GotoOverPressure();
		}

		/* Avoid over-pressure condition from control board */
//		else if(GLB_fsm_o3.CurrentOperatingPressure > OVERPRESSURE_THRESHOLD_SYRINGE)
		else if(GLB_fsm_o3.CurrentOperatingPressure > GLB_fsm_o3.PressThreshold)
		{
			/* Specific patch to resolve special hard and continuously used syringes */
			SyringeVol = (GLB_fsm_o3.CurrentOutputVolume < 21) ? 5 : 10;
			/* End of patch */

			GLB_fsm_o3.CurrentTotalDose = (uint32) (SyringeVol * GLB_fsm_o3.ConfiguredO3Concentration);
			GLB_fsm_o3.CurrentOutputVolume += SyringeVol;
			GotoOverPressure();
		}
	}
}
#if 0
uint16 GetPressureThreshold()
{
	uint16  PressThreshold;

	switch(GLB_fsm_o3.Option)
	{
	case CONTINUOUS_MODE:
		PressThreshold = OVERPRESSURE_THRESHOLD;
		break;
	case INSUFFLATION_R_MODE:
		PressThreshold = OVERPRESSURE_THRESHOLD_INSUFLATION;
		break;
	case MANUAL_MODE:
		PressThreshold = OVERPRESSURE_THRESHOLD_MANUAL;
		break;
	case DOSE_MODE:
		PressThreshold = OVERPRESSURE_THRESHOLD;
		break;
	case DENTAL_MODE:
		PressThreshold = GLB_fsm_o3.ConfiguredPressure + OVERPRESSURE_STEP_THRESHOLD_DENTAL;
		break;
	default:
		PressThreshold = OVERPRESSURE_THRESHOLD;
		break;
	}
	return PressThreshold;
}
#endif
void UpdateGenerating(void)
{
	int32  VolumeMonitor;

	/* Use only the last pressure sample */
	GLB_fsm_o3.CurrentOperatingPressure = GetInstantAbsolutePressure(GLB_fsm_o3.PressAtm);

	/* Then, read data and calculate the issued volume due now*/
	VolumeMonitor = GetCumulatedVolume() - GLB_fsm_o3.ReferenceOutputVolume + GLB_fsm_o3.CumulatedOutputVolume;

	if(VolumeMonitor >= 0)
	{
		if ( IS_NOT_MANUAL_NOR_DENTAL_NOR_INSUFFLATION() || IS_INSUFFLATION_R_ON() || IS_INSUFFLATION_V_ON() || IS_MANUAL_PUSHED() || IS_DENTAL_SUPPLYING())
		{
			GLB_fsm_o3.CurrentOutputVolume = VolumeMonitor;
		}
	}

	/*Calculate total estimated dose*/
	GLB_fsm_o3.CurrentTotalDose = GLB_fsm_o3.CurrentOutputVolume * (GLB_fsm_o3.CurrentMeanOzone >> 3);

//	if (GLB_fsm_o3.CurrentState->State_ID != STATE_ERROR)
	if( IS_NOT_ERROR_STATE() )
	{
		/* Check operating pressure */
//		if((int16)GLB_fsm_o3.CurrentOperatingPressure > GetPressureThreshold())
		if(GLB_fsm_o3.CurrentOperatingPressure > GLB_fsm_o3.PressThreshold)
		{
			switch(GLB_fsm_o3.Option)
			{
				case SYRINGE_AUTO_MODE:/* No action needed in syringe mode */
										  break;
				case SYRINGE_MANUAL_MODE: ReleasePressureDuringGeneration();
				                          break;
				case INSUFFLATION_R_MODE: PauseAndReleasePressure();
				                          break;
				default:                  GLB_fsm_o3.CumulatedOutputVolume = GLB_fsm_o3.CurrentOutputVolume;
						                  GotoOverPressure();
						                  break;
			}
		}
		/* Update syringe*/
		if( IS_SYRINGE_AUTO_MODE() )
		{
			UpdateGeneratingForSyringe();
		}
		/* Update dental mode*/
		if( IS_DENTAL_MODE() )
		{
			UpdateGeneratingForDental();
		}
		/* Check for end of procedure */
		if ( IS_CONTINUOUS_END() || IS_MANUAL_END() || IS_INSUFFLATION_R_END() || IS_INSUFFLATION_V_END() || IS_DOSE_END() || IS_CLOSED_BAG_END_BY_PRESSURE() || IS_CLOSED_BAG_END_BY_TIME() || IS_OPEN_BAG_END() || IS_SALINE_MIXING_END() || IS_SALINE_REINFUSION_END() || IS_DENTAL_TIMEOUT() ) // TODO: bag end by pressure, just by time or both
		{
			GotoEndOperation();
		}
	}
}



/*
  Bug 1297
  Description

  When input silicon tube between input block and pressure sensor is folded, gas
  flow stops and no flow error is detected.

  Comment 1

  Solved in V0.R0.P5_b version of interface board.

  When control board flow sensor return any flow value and proportional valve
  position is 0 (less than 5) then, there is a flow error.

  When silicon tube is folded the pressure in sensor 1 increases while pressure
  in sensor 2 is atmospheric pressure. Flow PID detects flow increase and then
  tries to compensate closing proportional electro-valve (this situation remains
  until folding releases).

  Comment 2

  Due to changes in V0.R1.P0_a & _b some false positive errors were detected when
  push in manual mode (the system starts without adjusting in this version..."no comment").
  Then some positive errors have to be confirmed in this version.
*/

void DetectFalseFlowSituation(void)
{
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};
  int16 PropValveState;
  static uint8 PositiveErrorCounter = 0;

  if( IS_GENERATING() )
  {
    Command.Command = CMD_GET_PROPORTIONAL_VALVE;
//    UTILS_uint8ToHexString(PROP_VALVE_INPUT,Command.Identifier,FALSE);
    snprintf(Command.Identifier, 5, "%X", PROP_VALVE_INPUT);
    ManageCommand(&Command,&Response);
    PropValveState = Response.Data[0];

    if(PropValveState < PROP_VALVE_CLOSED_VALUE)
    {
      if(PositiveErrorCounter++ > 3)
      {
        SendStopCommand();
//        GLB_fsm_o3.ErrorState = CTRL_BOARD_FLOW_ERROR;
//      GLB_fsm_o3.CurrentState->Error();
		GotoError(CTRL_BOARD_FLOW_ERROR);
      }
    }
    else
    {
      PositiveErrorCounter = 0;
    }
  }
}

void DetectHighVoltageDisconnection(void)
{
	if(1)  // TODO:pending to solve
	{
		return;
	}
	if(GLB_fsm_o3.ConfiguredO3Concentration != 0)
	{
		if ( IS_ADJUSTING() || IS_GENERATING() )
		{
			if( (GLB_fsm_o3.ErrorState & CTRL_BOARD_WARNING_NO_STABY_CURRENT) == CTRL_BOARD_WARNING_NO_STABY_CURRENT )
			{
				ClearErrors();
				TIMER_Start(&GLB_fsm_o3.HihgVoltDisconectTimer, HIGH_VOLT_DETECTION_TIMEOUT);
			}
			else if(TIMER_State(&GLB_fsm_o3.HihgVoltDisconectTimer) == TIMER_STATE_EXPIRED)
			{
				printf("call d\n");SendStopCommand();
//				GLB_fsm_o3.ErrorState = CTRL_BOARD_TRANSFORMER_ERROR;
//				GLB_fsm_o3.CurrentState->Error();
				GotoError(CTRL_BOARD_TRANSFORMER_ERROR);
			}
		}
	}
}

void GeneratorCleanStart(int16 CleanFlow, uint32 CleanTime)
{
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

  deb_printf(D_LEV_4, "GeneratorCleanStart(CleanFlow: %d, CleanTime: %lu)\n", CleanFlow, CleanTime);

  SwitchOutputValves(OUTPUT_TO_CATALYSER);

  /* Flow generation */
  Command.Command = CMD_SET_FLOW_TARGET;
  Command.Value = CleanFlow;
  ManageCommand(&Command,&Response);

  Command.Command = CMD_START_FLOW;
  ManageCommand(&Command,&Response);

  /* Wait for clean up */
  TIMER_Start(&GLB_fsm_o3.CleanGeneratorTimer, CleanTime);
}


void O3SensorTune_Ini(void)
{
	O3SensorTune_FSM(1);
}

void O3SensorTune(void)
{
	O3SensorTune_FSM(0);
}

void O3SensorTune_FSM(uint8 init)
{
	static int8 step =  0;
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	if(init)
	{
		step =  0;
	}

	switch(step)
	{
	case 0: // start cleaning
		deb_printf(D_LEV_4, "O3SensorTune (start)\n");

		step++;
		GeneratorCleanStart(GLB_fsm_o3.ConfiguredFlow * 8, 3000);  // TODO: consider to fix the clean flow to get the same clean level independently of the configured flow
		break;
	case 1: // wait for clean and start tune sensor
		deb_printf(D_LEV_4, "O3SensorTune (waiting for clean)\n");

		if( IS_GENERATOR_CLEANED() )
		{
			if(GLB_fsm_o3.GenerationMode == O3_GENERATION_BASED_ON_O3_PHOTOSENSOR)
			{
				step++;
				deb_printf(D_LEV_4, "O3SensorTune (tuning sensor LED start)\n");

				Command.Command = CMD_START_O3_SENSOR_CAL;
				Command.Value = TUNE_O3_SENSOR_LED_AND_START_LED_PID;
				ManageCommand(&Command,&Response);
			}
			else
			{
				step+= 2;
			}
		}
		break;
	case 2: // wait for tune end
		deb_printf(D_LEV_4, "O3SensorTune (tuning sensor LED)\n");

		Command.Command = CMD_STATE_O3_SENSOR_CAL;
		ManageCommand(&Command,&Response);

		if(Response.Data[0] == 0)
		{
			step++;
			DebugO3SensorTune();
		}
		break;
	case 3: // start generation
		deb_printf(D_LEV_4, "O3SensorTune (end)\n");

		StartGeneration(FLOW_ALREADY_STARTED);
		break;
	}
}



/* Vacuum by time and pressure generation */

void EndVacuumPressure(void)
{
	SetVacuumState(VACUUM_STATE_STOP);
//	GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_COMPLETED];
	ChangeState(STATE_COMPLETED);
}

void EndVacuumTime(void)
{
	SetVacuumState(VACUUM_STATE_STOP);

	if( IS_CLOSED_BAG_INITIAL_VACUUM() )
	{
		ChangeState(STATE_TUNING_O3_SENSOR);
	}
//	else if( IS_CLOSED_BAG_FINAL_VACUUM() )
//	{
//		ChangeState(STATE_COMPLETED);
//	}
	else
	{
		ChangeState(STATE_COMPLETED);
	}
}

void CancelVacuum(void)
{
	SetVacuumState(VACUUM_STATE_STOP);
//    GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_USER_CANCELLED];
	ChangeState(STATE_USER_CANCELLED);
}

void PauseVacuum(void)
{
	if(GLB_fsm_o3.VacuumStatus == VACUUM_STATE_RUNNING)
	{
		SetVacuumState(VACUUM_STATE_PAUSE);
		GLB_fsm_o3.FreezeRemainingTime = 1;
	}
	else if(GLB_fsm_o3.VacuumStatus == VACUUM_STATE_PAUSE)
	{
		SetVacuumState(VACUUM_STATE_RUNNING);
		GLB_fsm_o3.FreezeRemainingTime = 0;
	}
}

void SkipVacuum(void)
{
	SetVacuumState(VACUUM_STATE_STOP);

	if( IS_CLOSED_BAG_INITIAL_VACUUM() )
	{
//		GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_TUNING_O3_SENSOR];
		ChangeState(STATE_TUNING_O3_SENSOR);
	}
	else if( IS_CLOSED_BAG_FINAL_VACUUM() || IS_OPEN_BAG_MODE())
	{
//		GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_COMPLETED];
		ChangeState(STATE_COMPLETED);
	}
}

void ModifyVacuum(void)
{
	if ( IS_CLOSED_BAG_MODE() || IS_OPEN_BAG_MODE() )
	{

		SkipVacuum();
	}
	if ( IS_VACUUM_TIME_MODE() || IS_VACUUM_PRESSURE_MODE() )
	{
		PauseVacuum();
	}
}

void UpdateVacuumByPressure(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	Command.Command = CMD_GET_SENSOR;

	snprintf(Command.Identifier, 5, "%X", ID_SENSOR_PRESSURE_1);
	ManageCommand(&Command,&Response);

    if( abs((GLB_fsm_o3.PressAtm) - (Response.Data[0])) > GLB_fsm_o3.ConfiguredVacuumPressure)
	{
		EndVacuumPressure();
	}
}

void UpdateVacuumBytime(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	Command.Command = CMD_GET_SENSOR;
//	UTILS_uint8ToHexString(ID_SENSOR_PRESSURE_1,Command.Identifier,FALSE);
	snprintf(Command.Identifier, 5, "%X", ID_SENSOR_PRESSURE_1);
	ManageCommand(&Command,&Response);

	if( (Response.Data[0] - (int16)(GLB_fsm_o3.PressAtm)) < LOWPRESSURE_THRESHOLD_VACUUM)
	{
		SetVacuum(VACUUM_STATE_STOP);
		GotoOverPressure();
	}
	else
	{
		/* Check for end of procedure */
		if ( IS_VACUUM_TIME_END() || IS_CLOSED_BAG_VACUUM_TIME_END() || IS_OPEN_BAG_VACUUM_TIME_END() )
		{
			EndVacuumTime();
		}
	}
}

void UpdateVacuum(void)
{
	if( IS_VACUUM_TIME_MODE() || IS_CLOSED_BAG_MODE() || IS_OPEN_BAG_MODE() )
	{
		UpdateVacuumBytime();
	}
	if( IS_VACUUM_PRESSURE_MODE() )
	{
		UpdateVacuumByPressure();
	}
}
void SetVacuum(int8 State)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	Command.Command = CMD_SET_VACCUM;
	Command.Identifier[0] = VACUUM_0;
	Command.Value = State;
	ManageCommand(&Command,&Response);
}

void SetVacuumState(int8 State)
{
	GLB_fsm_o3.VacuumStatus = State;
	SwitchOutputValves((State == VACUUM_STATE_RUNNING) ? OUTPUT_TO_EXTERIOR : OUTPUT_TO_CATALYSER);
	SetVacuum((State == VACUUM_STATE_RUNNING) ? VACUUM_ON : VACUUM_OFF);
}

void StartVacuumGeneration(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	SwitchOutputValves(OUTPUT_TO_CATALYSER);
	Delay_o3_ms(200);/*TODO: measure real 200 ms*/

	Command.Command = CMD_GET_SENSOR;
	snprintf(Command.Identifier, 5, "%X", ID_SENSOR_PRESSURE_1);
	ManageCommand(&Command,&Response);
	GLB_fsm_o3.PressAtm = Response.Data[0];

	SetVacuumState(VACUUM_STATE_RUNNING);

//	GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_VACUUM_GENERATING];
	ChangeState(STATE_VACUUM_GENERATING);
}

void GotoVacuumGeneration(void)
{
	StartVacuumGeneration();
/*	ResetCumulativeValues();*/
	ResetTiming();
}

void UpdateTherapyTime(void)
{
	if ( IS_CLOSED_BAG_THERAPY_TIME_END() )
	{
//		GLB_fsm_o3.CurrentState = &(GLB_fsm_o3.States[STATE_COMPLETED]);
		GotoVacuumGeneration();
	}
}

void CancelTherapyTime(void)
{
//    GLB_fsm_o3.CurrentState = &(GLB_fsm_o3.States[STATE_USER_CANCELLED]);
	GotoVacuumGeneration();
}

void GotoStuffConnected(void)
{
	if( IS_SALINE_MODE () )
	{
		GLB_fsm_o3.SalineCycle = SALINE_REINFUSION_CYCLE;
//		GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_TUNING_O3_SENSOR];
		ChangeState(STATE_TUNING_O3_SENSOR);
	}
	if( IS_DENTAL_MODE () )
	{
//		GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_TUNING_O3_SENSOR];
		ChangeState(STATE_TUNING_O3_SENSOR);
	}
}

OPERATION_MODE_E ConvertTherapyOption(OPERATION_MODE_E Option)
{
	switch (Option)
	{
		case INSUFFLATION_MODE: return INSUFFLATION_R_MODE;
		case BAG_MODE:          return CLOSED_BAG_MODE;
		case SYRINGE_MODE:      return SYRINGE_AUTO_MODE;
		case VACUUM_MODE:       return VACUUM_TIME_MODE;
		default:                return Option;
	}
}


uint16 TmpPressureForNextStep;      // Try to improve this method to pass pressure
uint16 TmpTemperatureForNextStep;   // Try to improve this method to pass temperature

void CalibrateO3_Ref_1_Ok(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};
	uint16 Pressure;
	uint16 Temperature;

	printf("CalibrateO3_Ref_1_Ok(): %d\n", GLB_fsm_o3.CalibrationValue_0);

	/* Start O3 sensor calibration procedure with reference value 1.                */
	/* This procedure with reference value 1 will be finished outside this function */

	/* Read pressure  */
	Command.Command = CMD_GET_SENSOR;
	snprintf(Command.Identifier, 5, "%X", ID_SENSOR_PRESSURE_1);
	ManageCommand(&Command,&Response);
	Pressure = Response.Data[0];

	/* Read temperature  */
	snprintf(Command.Identifier, 5, "%X", ID_SENSOR_TEMPERATURE_0);
	ManageCommand(&Command,&Response);
	Temperature = Response.Data[0];

	/* Get sensor value for reference O3 value */
	snprintf(Command.Identifier, 5, "%X", ID_SENSOR_OZONE_DIRECT);
	ManageCommand(&Command,&Response);
	CalibrationRef.R2 = Response.Data[0];

	Command.Command = CMD_STOP_OZONE;
	ManageCommand(&Command,&Response);
//	GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_CALIBRATE_O3_3];
	ChangeState(STATE_CALIBRATE_O3_3);

	TmpPressureForNextStep = Pressure;       // Try to improve this method to pass pressure
	TmpTemperatureForNextStep = Temperature; // Try to improve this method to pass temperature
}

void O3SensorCalibrationEnd(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};
	uint16 Pressure = TmpPressureForNextStep;       // Try to improve this method to pass pressure
	uint16 Temperature = TmpTemperatureForNextStep; // Try to improve this method to pass temperature
	int32  AuxVal;

	printf("O3SensorCalibrationEnd(): %d\n", GLB_fsm_o3.CalibrationValue_0);

	/* Continue O3 sensor calibration procedure with reference value 1 */

	if( IS_HW_O3_SENSOR() )
	{

		for(uint8 i = 0; i < 100; i++)
		{
			/* Send keep alive */
			Command.Command = CMD_TEST_COM;
			ManageCommand(&Command,&Response);
			Delay_o3_ms(50);

			if(GLB_FsmEvents.Cancel)
			{
				return; // patch to allow 'cancel' process out of state machine
			}
		}

		/* Get sensor value for zero O3 */
		Command.Command = CMD_GET_SENSOR;
		snprintf(Command.Identifier, 5, "%X", ID_SENSOR_OZONE_DIRECT);
		ManageCommand(&Command,&Response);

		/* Calculate media. Experimental better results */
		CalibrationRef.R1 = (CalibrationRef.R1 + Response.Data[0]) >> 1;
		Command.Command = CMD_SET_TABLE_POSITION;
		Command.Identifier[0] = 'P';
		Command.Position[0] = O3_SENSOR_GAIN_SEC1_TABLE_POSITION;
#if 0       /* 76 ug*/
		/*AuxVal = 5370789 * Pressure;                                               */
		/*AuxVal = 2685394 * Pressure;                     Avoid overload, step 1    */
		AuxVal = 1342697L * (signed long)Pressure;      /* Avoid sing change, step 1 */
#endif
#if 0       /*64 ug*/
		/*AuxVal = 4522770 * Pressure;                                               */
		/*AuxVal = 2261385 * Pressure;                     Avoid overload, step 1    */
		/*AuxVal = 1130693 * Pressure;                     Avoid sing change, step 1 */
		AuxVal = 1130693L * (signed long)Pressure;      /* Avoid sing change, step 1 */
#endif
#if 1       /*60 ug*/
		/*AuxVal = 4240097 * Pressure;                                               */
		/*AuxVal = 2120048 * Pressure;                     Avoid overload, step 1    */
		/*AuxVal = 1060024 * Pressure;                     Avoid sing change, step 1 */
		AuxVal = 1060024L * (signed long)Pressure;      /* Avoid sing change, step 1 */
		AuxVal = AuxVal / ((34963L + (signed long)Temperature) * (int32)(CalibrationRef.R2 - CalibrationRef.R1));
		/*AuxVal = AuxVal << 1;                            Avoid overload, step 2    */
		AuxVal = AuxVal << 2;                           /* Avoid sing change, step 1 */
		Command.Value = (int16) AuxVal;
		ManageCommand(&Command,&Response);
#endif
	}
	/* O3 sensor calibration procedure with reference value 1 finished */

	/* Store reference 1 for calibration by pulses */
	Command.Command = CMD_SET_TABLE_POSITION;
	Command.Identifier[0] = 'P';
	Command.Position[0] = PULSES_FOR_O3_CALIBRATION_SEC_1_POSITION;
	Command.Value = (int16)GLB_fsm_o3.CalibrationValue_0;
	ManageCommand(&Command,&Response);
////	Delay_o3_ms(200); /*  Evaluate if necessary, only for V3*/

	/* Prepare reference value calibration step */

	/* Modulator start */
	Command.Command = CMD_MODULATOR_START;
	ManageCommand(&Command,&Response);

	Command.Command = CMD_SET_MODULATOR_WIDTH;
	Command.Value = 1024 - 50; // starting value for next reference value, it will be override by GUI
	ManageCommand(&Command,&Response);
}

void CalibrateO3_Ref_1_Value(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	printf("CalibrateO3_Ref_1_Value(): %d\n", GLB_fsm_o3.CalibrationValue_0);

	Command.Command = CMD_SET_MODULATOR_WIDTH;
	Command.Value = 1024 - (int16)GLB_fsm_o3.CalibrationValue_0;
	ManageCommand(&Command,&Response);
}

void CalibrateO3_Ref_2_Ok(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	printf("CalibrateO3_Ref_2_Ok()...\n");

	/* Calibration by pulses */

	Command.Command = CMD_SET_TABLE_POSITION;
	Command.Identifier[0] = 'P';
	Command.Position[0] = PULSES_FOR_O3_CALIBRATION_SEC_2_POSITION;
	Command.Value = (int16)GLB_fsm_o3.CalibrationValue_1;
	ManageCommand(&Command,&Response);
///	Delay_o3_ms(200); /*  Evaluate if necessary, only for V3*/
}

void CalibrateO3_Ref_2_Value(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	printf("CalibrateO3_Ref_2_Value(): %d\n", GLB_fsm_o3.CalibrationValue_1);

	Command.Command = CMD_SET_MODULATOR_WIDTH;
	Command.Value = 1024 - (int16)GLB_fsm_o3.CalibrationValue_1;
	ManageCommand(&Command,&Response);

}

void CalibrateO3_Ref_3_Ok(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	printf("CalibrateO3_Ref_3_Ok()...\n");

	/* Calibration by pulses */

	Command.Command = CMD_SET_TABLE_POSITION;
	Command.Identifier[0] = 'P';
	Command.Position[0] = PULSES_FOR_O3_CALIBRATION_SEC_3_POSITION;
	Command.Value = (int16)GLB_fsm_o3.CalibrationValue_2;
	ManageCommand(&Command,&Response);
///	Delay_o3_ms(200);  /* only for V3 evaluate if necessary*/

	Command.Position[0] = PULSES_FOR_1_MG_POSITION;
	Command.Value = (int16)GLB_fsm_o3.CalibrationValue_2;
	ManageCommand(&Command,&Response);
//	Delay_o3_ms(200);  /* only for V3 evaluate if necessary*/

	SendReset();
	GotoService();
}

void CalibrateO3_Ref_3_Value(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	printf("CalibrateO3_Ref_3_Value(): %d\n", GLB_fsm_o3.CalibrationValue_2);

	Command.Command = CMD_SET_MODULATOR_WIDTH;
	Command.Value = 1024 - (int16)GLB_fsm_o3.CalibrationValue_2;
	ManageCommand(&Command,&Response);
}

void CalibrateFlowStep1_Ok(void)
{
	printf("CalibrateFlowStep1_Ok()...\n");
}

void CalibrateFlowStep1_Value(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	printf("CalibrateFlowStep1_Value(): %d\n", GLB_fsm_o3.CalibrationValue_0);

    Command.Command = CMD_SET_PROPORTIONAL_VALVE;
//    UTILS_uint8ToHexString(PROP_VALVE_INPUT,Command.Identifier,FALSE);
    snprintf(Command.Identifier, 5, "%X", PROP_VALVE_INPUT);
    Command.Value = (int16) GLB_fsm_o3.CalibrationValue_0;;
    ManageCommand(&Command,&Response);
}

void CalibrateFlowStep2(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};
	int16  P1, P2, Slope1, Offset1, UserValue =  GLB_fsm_o3.CalibrationValue_1;

	printf("CalibrateFlowStep2(): %d\n", GLB_fsm_o3.CalibrationValue_1);

	Command.Command = CMD_GET_SENSOR ;
    sprintf((char *)Command.Identifier, "%X", ID_SENSOR_PRESSURE_0 );
    ManageCommand(&Command,&Response);
    P1 = Response.Data[0];

    sprintf((char *)Command.Identifier, "%X", ID_SENSOR_PRESSURE_1 );
    ManageCommand(&Command,&Response);
    P2 = Response.Data[0];

    Offset1 = CalibrationRef.R1; /* Offset was stored temporally on R1 before*/
    Slope1 = ComputeFlowSlope(P1, P2, Offset1, UserValue);

    Command.Command = CMD_SET_TABLE_POSITION;
    Command.Identifier[0] = 'P';
    Command.Position[0] = FLOW_GAIN_SEC1_TABLE_POSITION;
    Command.Value = Slope1;
    ManageCommand(&Command,&Response);

    Command.Position[0] = FLOW_OFFSET_SEC1_TABLE_POSITION;
    Command.Value = Offset1;
    ManageCommand(&Command,&Response);

    Command.Position[0] = FLOW_LIMIT_SEC1_TABLE_POSITION;
    Command.Value = 0x7000 | UserValue; /* Ensure max limit value (just one sector) and store UserValue in the same parameter space */
    ManageCommand(&Command,&Response);

    /* Stop flow */
    Command.Command = CMD_STOP_FLOW;
    ManageCommand(&Command,&Response);

    SendReset();
	GotoService();
}

void CalibratePressureInit(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

    SwitchOutputValves(OUTPUT_TO_EXTERIOR);

    /* Set pressure sensor 0 slope to 1*/
    Command.Command = CMD_SET_SENSOR_SLOPE;
    sprintf((char *)Command.Identifier, "%X", ID_SENSOR_PRESSURE_0);
    Command.Value = 256;
    Command.Position[0] = SECTION_0;
	ManageCommand(&Command,&Response);

    /* Set pressure sensor 0 offset to 0*/
    Command.Command = CMD_SET_SENSOR_OFFSET;
    Command.Value = 0;
	ManageCommand(&Command,&Response);

    /* Set pressure sensor 1 slope to 1*/
    Command.Command = CMD_SET_SENSOR_SLOPE;
    sprintf((char *)Command.Identifier, "%X", ID_SENSOR_PRESSURE_1);
    Command.Value = 256;
    Command.Position[0] = SECTION_0;
	ManageCommand(&Command,&Response);

    /* Set pressure sensor 1 offset to 0*/
    Command.Command = CMD_SET_SENSOR_OFFSET;
    Command.Value = 0;
	ManageCommand(&Command,&Response);
}

void CalibratePressureStep1(void)
{
	printf("CalibratePressureStep1()...\n");
	CalibratePressure(1);
}

void CalibratePressureStep2(void)
{
	printf("CalibratePressureStep2()...\n");
	CalibratePressure(2);
}

void CalibratePressure(uint8 step)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};
	static int16 PresAtmReal, D1atm, D2atm, Var1, Var2;

	if(step == 1)
	{
		Command.Command = CMD_GET_SENSOR;
		sprintf((char *)Command.Identifier, "%X", ID_SENSOR_PRESSURE_0 );
		ManageCommand(&Command,&Response);
		D1atm = Response.Data[0];
		sprintf((char *)Command.Identifier, "%X", ID_SENSOR_PRESSURE_1);
		ManageCommand(&Command,&Response);
		D2atm = Response.Data[0];
		PresAtmReal = GLB_fsm_o3.CalibrationValue_0;
	}
	else if(step == 2)
	{
		Command.Command = CMD_GET_SENSOR;
		sprintf((char *)Command.Identifier, "%X", ID_SENSOR_PRESSURE_0 );
		ManageCommand(&Command,&Response);
		Var1 = Response.Data[0];
		sprintf((char *)Command.Identifier, "%X", ID_SENSOR_PRESSURE_1);
		ManageCommand(&Command,&Response);
		Var2 = Response.Data[0];

		if((Var1 - D1atm) && (Var2 - D2atm)) // avoid division by zero if the pressure was not externally changed
		{
			/* Set pressure sensor 0 slope */
			Command.Command = CMD_SET_TABLE_POSITION;
			Command.Identifier[0] = 'P';
			Command.Position[0] = PRESS_0_GAIN_SEC1_TABLE_POSITION;
			Command.Value = (int16)(128000L / (Var1 - D1atm));
			ManageCommand(&Command,&Response);

			/* Set pressure sensor 0 offset*/
			Command.Position[0] = PRESS_0_OFFSET_SEC1_TABLE_POSITION;
			Command.Value = (int16)((int32)PresAtmReal - (((int32)D1atm * (int32)Command.Value) >> 8));
			ManageCommand(&Command,&Response);

			/* Set pressure sensor 1 slope */
			Command.Position[0] = PRESS_1_GAIN_SEC1_TABLE_POSITION;
			Command.Value = (int16)(128000L / (Var2 - D2atm));
			ManageCommand(&Command,&Response);

			/* Set pressure sensor 1 offset*/
			Command.Position[0] = PRESS_1_OFFSET_SEC1_TABLE_POSITION;
			Command.Value = (int16)((int32)PresAtmReal - (((int32)D2atm * (int32)Command.Value) >> 8));
			ManageCommand(&Command,&Response);

			SendReset();
		}

		GotoService();
	}
}
/*
void ChangeCurrentState(uint16_t NewState)??????????????????????????
{
	GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[NewState];
}
*/
void ResetOption(void)
{
	GLB_fsm_o3.Option = NO_MODE;
}

void refreshGeneratorVersion(void)
{
  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

  Command.Command = CMD_GET_SW_VERSION;
  ManageCommand(&Command,&Response);

  sprintf((char *)GLB_fsm_o3.SharedBuffer,"%s",(int8 *)(Response.Data));
}

int16 getPressureSensor(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	Command.Command = CMD_GET_SENSOR;
//	UTILS_uint8ToHexString(ID_SENSOR_PRESSURE_1,Command.Identifier,FALSE);
	snprintf(Command.Identifier, 5, "%X", ID_SENSOR_PRESSURE_1);
	ManageCommand(&Command,&Response);
	return Response.Data[0];
}

int16 getTemperatureSensor(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	Command.Command = CMD_GET_SENSOR;
//	UTILS_uint8ToHexString(ID_SENSOR_TEMPERATURE_0,Command.Identifier,FALSE);
	snprintf(Command.Identifier, 5, "%X", ID_SENSOR_TEMPERATURE_0);
	ManageCommand(&Command,&Response);
	return (Response.Data[0] / 128);
}

void refreshParameters(void)
{
	uint8 auxBuff[20];
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};
	Command.Command = CMD_GET_TABLE_POSITION;
	Command.Identifier[0] = 'P';        /* Parameters Table */

	Command.Position[0] = CALIBRATION_PRESSURE_POSITION;
	ManageCommand(&Command,&Response);
	sprintf((char *)GLB_fsm_o3.SharedBuffer,"PrCal: %d", Response.Data[0]);

	Command.Position[0] = O3_SENSOR_GAIN_SEC1_TABLE_POSITION;
	ManageCommand(&Command,&Response);
	sprintf((char *)auxBuff,"\nGO3: %d", Response.Data[0]);
	strcat((char *)GLB_fsm_o3.SharedBuffer, (char *)auxBuff);

	Command.Position[0] = BASE_PERIOD_INI_VALUE_TABLE_POSITION;
	ManageCommand(&Command,&Response);
	sprintf((char *)auxBuff,"\nPer: %d", Response.Data[0]);
	strcat((char *)GLB_fsm_o3.SharedBuffer, (char *)auxBuff);

	sprintf((char *)auxBuff,"\nSensor: %d", GLB_fsm_o3.GenerationMode);
	strcat((char *)GLB_fsm_o3.SharedBuffer, (char *)auxBuff);

	Command.Position[0] = FLOW_GAIN_SEC1_TABLE_POSITION;
	ManageCommand(&Command,&Response);
	sprintf((char *)auxBuff,"\nFG: %d", Response.Data[0]);
	strcat((char *)GLB_fsm_o3.SharedBuffer, (char *)auxBuff);

	Command.Position[0] = FLOW_OFFSET_SEC1_TABLE_POSITION;
	ManageCommand(&Command,&Response);
	sprintf((char *)auxBuff,"\nFO: %d", Response.Data[0]);
	strcat((char *)GLB_fsm_o3.SharedBuffer, (char *)auxBuff);

	Command.Position[0] = PULSES_FOR_O3_CALIBRATION_SEC_1_POSITION;
	ManageCommand(&Command,&Response);
	sprintf((char *)auxBuff,"\nPuls: %d", Response.Data[0]);
	strcat((char *)GLB_fsm_o3.SharedBuffer, (char *)auxBuff);

	Command.Position[0] = PULSES_FOR_O3_CALIBRATION_SEC_2_POSITION;
	ManageCommand(&Command,&Response);
	sprintf((char *)auxBuff,", %d", Response.Data[0]);
	strcat((char *)GLB_fsm_o3.SharedBuffer, (char *)auxBuff);

	Command.Position[0] = PULSES_FOR_O3_CALIBRATION_SEC_3_POSITION;
	ManageCommand(&Command,&Response);
	sprintf((char *)auxBuff,", %d", Response.Data[0]);
	strcat((char *)GLB_fsm_o3.SharedBuffer, (char *)auxBuff);

	printf("Parameters refreshed: %s\n", GLB_fsm_o3.SharedBuffer);
}

void refreshStartupInfo()
{
	uint8 auxBuff[30];
//	  sprintf((char *)GLB_fsm_o3.SharedBuffer,"%s","Start up info should be included here...");

	sprintf((char *)GLB_fsm_o3.SharedBuffer,"Generation mode: %s\n\n", GLB_fsm_o3.GenerationMode ? "Sensor" : "Math" );

	sprintf((char *)auxBuff,"Test result: %d\n", StartUpLog[START_UP_LOG_TEST]);
	strcat((char *)GLB_fsm_o3.SharedBuffer, (char *)auxBuff);
	sprintf((char *)auxBuff,"Test reference: %d\n", StartUpLog[START_UP_LOG_TEST_REF]);
	strcat((char *)GLB_fsm_o3.SharedBuffer, (char *)auxBuff);
	sprintf((char *)auxBuff,"Sensor ofset: %d\n", StartUpLog[START_UP_LOG_SENSOR_OFFSET]);
	strcat((char *)GLB_fsm_o3.SharedBuffer, (char *)auxBuff);
	sprintf((char *)auxBuff,"O3 direct: %d\n", StartUpLog[START_UP_LOG_O3_DIRECT]);
	strcat((char *)GLB_fsm_o3.SharedBuffer, (char *)auxBuff);
	sprintf((char *)auxBuff,"Pressure: %d mbar\n", StartUpLog[START_UP_LOG_PRESSURE]);
	strcat((char *)GLB_fsm_o3.SharedBuffer, (char *)auxBuff);
	sprintf((char *)auxBuff,"Temperature: %d °C\n", StartUpLog[START_UP_LOG_TEMPERATURE]);
	strcat((char *)GLB_fsm_o3.SharedBuffer, (char *)auxBuff);

	sprintf((char *)auxBuff,"Msg len: %d", (int)strlen((char *)GLB_fsm_o3.SharedBuffer));
	strcat((char *)GLB_fsm_o3.SharedBuffer, (char *)auxBuff);

}

/* V3 calibration */
/* V3 calibration */
/* V3 calibration */
/* V3 calibration */
void StartResonanceFrequencySearch(void)
{
    uint16 Period;
    uint16 DeadTime;
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

    /* Get system parameters & prepare data to start generator */
    Command.Command = CMD_GET_TABLE_POSITION;
    Command.Identifier[0] = 'P';        /* Parameters Table */
    Command.Position[0] = PERIOD_INI_VALUE_TABLE_POSITION; /* This memory position stores initial modulator base period value for resonance frequency search on OZD */
	ManageCommand(&Command,&Response);
    Period = Response.Data[0] | 0x01;

#if 0
    /* bug 9791 debug */
    /* make resonance frequency search process shorter, just for debug purposes */
    Period = 0xCC | 0x01;
#endif

    DeadTime = ComputeDeadTime(Period);

    GLB_fsm_o3.MaxCurrent = 0;
    GLB_fsm_o3.LastModBasePeriod = Period;
    GLB_fsm_o3.MaxModBasePeriod = Period + 128;

    SendGeneratorStartSequence(Period, 0, DeadTime);
}

int16 MeasureCurrent(void)
{
    uint8 i;
    int32 Current = 0;
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

    Command.Command = CMD_GET_SENSOR;

    /* measure current */
    for( i = 0; i < 4; i++ )
    {
    	Delay_o3_ms(20);
        sprintf((char *)Command.Identifier, "%X", ID_SENSOR_CURRENT);
    	ManageCommand(&Command,&Response);
        Current += Response.Data[0];
    }
    Current = Current >> 2;

    return (int16) Current;
}

void UpdateResonanceFrequencySearch(void)
{
    uint8  i, SearchError = 1;
    int16  Pressure;
    uint16 DeadTime;
    int32  Current = 0;
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

    /* measure pressure */
    Command.Command = CMD_GET_SENSOR;
    sprintf((char *)Command.Identifier, "%X", ID_SENSOR_PRESSURE_1);
	ManageCommand(&Command,&Response);
    Pressure = Response.Data[0];

    /* measure current */
    Current = MeasureCurrent();

    if(GLB_fsm_o3.MaxCurrent < Current)
    {
    	GLB_fsm_o3.MaxCurrent = Current;
    }

    if(Current < 1800) /* no over current, continue searching... */
    {
        if( GLB_fsm_o3.MaxCurrent <= (Current + 50) ) /* no current peak found, continue searching... */
        {
            if(GLB_fsm_o3.LastModBasePeriod < GLB_fsm_o3.MaxModBasePeriod) /* enough period range, continue searching... */
            {
                SearchError = 0;
                GLB_fsm_o3.LastModBasePeriod += 2;
                GLB_fsm_o3.LastModBasePeriod |= 0x01;
                DeadTime = ComputeDeadTime(GLB_fsm_o3.LastModBasePeriod);
                SendGeneratorStartSequence(GLB_fsm_o3.LastModBasePeriod, 0, DeadTime);
            }
        }
        else /* current peak found, search ends successfully */
        {
            SearchError = 0;

            Command.Command = CMD_SET_TABLE_POSITION;
            Command.Identifier[0] = 'P';
            Command.Position[0] = BASE_PERIOD_INI_VALUE_TABLE_POSITION;
            Command.Value = GLB_fsm_o3.LastModBasePeriod;
        	ManageCommand(&Command,&Response);

            Command.Position[0] = BASE_WIDTH_INI_TABLE_POSITION;
            Command.Value = GLB_fsm_o3.LastModBasePeriod >> 1;
        	ManageCommand(&Command,&Response);

            Command.Position[0] = CALIBRATION_PRESSURE_POSITION;
            Command.Value = Pressure;
        	ManageCommand(&Command,&Response);

            Command.Position[0] = PRESSURE_COMPENSATION_FACTOR_POSITION;
            Command.Value = 0;
        	ManageCommand(&Command,&Response);

        	TestCurrentStart();

            /* just delay */
            for(i=0; i<1; i++)
            {
              Delay_o3_ms(1000);
              Command.Command = CMD_TEST_COM;
              ManageCommand(&Command,&Response);
            }

            /* no sensor mode */
            Command.Command = CMD_SET_TABLE_POSITION;
            Command.Identifier[0] = 'P';
            Command.Position[0] = START_UP_TEST_CURRENT_TABLE_POSITION;
            Command.Value = MeasureCurrent();
        	ManageCommand(&Command,&Response);

    		printf("Resonance frequency search success. Current: %ld\n", Current);   // SWV debug
    		SendReset();
//    		GLB_fsm_o3.CurrentState = &GLB_fsm_o3.States[STATE_CALIBRATION_END];
    		ChangeState(STATE_CALIBRATION_END);
        }
    }

    if(SearchError)
    {
//		GLB_fsm_o3.ErrorState = CTRL_BOARD_TRANSFORMER_ERROR;
		GotoError(CTRL_BOARD_TRANSFORMER_ERROR);
    }

    printf("UpdateResonanceFrequencySearch. Current: % ld - Period %d - Error: %d\n", Current, GLB_fsm_o3.LastModBasePeriod, SearchError);
}

void CalibratePeriodUpdate(void)
{
	UpdateResonanceFrequencySearch();
}

void SkipErrors(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	Command.Command = CMD_SKIP_ERROR;
	Command.Identifier[0] = '1';
	ManageCommand(&Command,&Response);
}

void RestoreErrors(void)
{
	PROTOCOL_COMMAND_T  Command  = {0};
	PROTOCOL_RESPONSE_T Response = {0};

	Command.Command = CMD_SKIP_ERROR;
	Command.Identifier[0] = '0';
	ManageCommand(&Command,&Response);
}

void StopManualGap(void)
{
	TIMER_Stop(&GLB_fsm_o3.AuxTimer);    // patch for manual therapy
}

void StartManualGap(void)
{
	TIMER_Start(&GLB_fsm_o3.AuxTimer, 20000);   // patch for manual therapy
}

void TestIni(void)
{

	printf("TestIni()...*************************************************************\n");


  PROTOCOL_COMMAND_T  Command  = {0};
  PROTOCOL_RESPONSE_T Response = {0};

  GLB_fsm_o3.FreezeRemainingTime = 1;

  SwitchOutputValves(OUTPUT_TO_EXTERIOR);

  Command.Command = CMD_SET_FLOW_TARGET;
  Command.Value = WASHING_FLOW * 8;
  ManageCommand(&Command,&Response);

  Command.Command = CMD_START_FLOW;
  ManageCommand(&Command,&Response);

  GLB_fsm_o3.WashingConfiguredTime = DEFAULT_WHASHING_TIME;
  GLB_fsm_o3.WashingSeconds = GLB_fsm_o3.WashingConfiguredTime;

}

