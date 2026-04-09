#ifndef FSM_O3_API_H_
#define FSM_O3_API_H_

#include "fsm_o3_operation.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Events (GUI -> FSM) ---- */
void fsm_o3_sendEnter(void);
void fsm_o3_sendCancel(void);
void fsm_o3_sendGeneric(void);

#ifdef __cplusplus
}
#endif

#endif /* FSM_O3_API_H_ */
