#ifndef FSM_O3_H_
#define FSM_O3_H_

#include "../Typedef.h"

 /* FSM states identifiers */
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

/* Return values for state methods */
typedef enum
{
  FSM_O3_RETVAL_SUCCESS=0,
  FSM_O3_RETVAL_ERROR=-1,
  FSM_O3_RETVAL_BAD_PARAMETER=-2,
  FSM_O3_RETVAL_MAX=-3
}FSM_O3_RETVAL_E;


/* State object model structure */
typedef struct
{
    void (* Entry)(void);
    void (* Enter)(void);
    void (* Cancel)(void);
    void (* Animate)(void);
    void (* Error)(void);
    void (* Exit)(void);
    void (* GenericEvent)(void);
    FSM_O3_STATE_ID_E State_ID;
    void *Owner;
}FSM_O3_STATE_T;


#ifdef __cplusplus
extern "C" {
#endif

void fsm_o3_main(void);
void fsm_o3_timeHandler(void);

#ifdef __cplusplus
}
#endif


#endif /* FSM_O3_H_ */


