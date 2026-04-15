#ifndef FSM_O3_H_
#define FSM_O3_H_

#include "../Typedef.h"
#include "fsm_o3_types.h"

/* Syringe control data */
#define SYRINGE_PATTERN_NUM                         5
#define DEF_SYRINGE_STOP_5ML                     4000
#define DEF_SYRINGE_STOP_10ML                    6150
#define DEF_SYRINGE_STOP_20ML                   10828
#define DEF_SYRINGE_STOP_50ML                   23404
#define DEF_SYRINGE_STOP_100ML                  45000

/* Return values for state methods */
typedef enum
{
  FSM_O3_RETVAL_SUCCESS=0,
  FSM_O3_RETVAL_ERROR=-1,
  FSM_O3_RETVAL_BAD_PARAMETER=-2,
  FSM_O3_RETVAL_MAX=-3
}FSM_O3_RETVAL_E;


/* Debug level flags */
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

/* Global variables defined in fsm_o3.c */
extern uint32 GLB_FSM_ProcessEvent_Count;
extern uint32 GLB_TickCounter;
extern uint32 GLB_SecondCounter;
extern uint16 GLB_SyringeStopPattern[];

void fsm_o3_main(void);
void fsm_o3_timeHandler(void);
void deb_printf(int8 deb_level, const char *fmt, ...);

#ifdef __cplusplus
}
#endif


#endif /* FSM_O3_H_ */


