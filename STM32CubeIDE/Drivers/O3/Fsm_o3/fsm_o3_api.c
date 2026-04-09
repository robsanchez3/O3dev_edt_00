#include "fsm_o3_api.h"

/* ---- Events (GUI -> FSM) ---- */

void fsm_o3_sendEnter(void)
{
    GLB_FsmEvents.Enter = 1;
}

void fsm_o3_sendCancel(void)
{
    GLB_FsmEvents.Cancel = 1;
}

void fsm_o3_sendGeneric(void)
{
    GLB_FsmEvents.Generic = 1;
}
