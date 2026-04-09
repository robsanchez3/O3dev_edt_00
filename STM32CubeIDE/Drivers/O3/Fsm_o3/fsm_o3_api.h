#ifndef FSM_O3_API_H_
#define FSM_O3_API_H_

#include "../Typedef.h"

/* Therapy parameter IDs */
typedef enum {
    TTV_CONCENTRATION,
    TTV_FLOW,
    TTV_TIME,
    TTV_VOLUME,
    TTV_DOSE,
    TTV_PRESSURE,
    TTV_VACUUM_TIME,
    TTV_VACUUM_PRESSURE,
    TTV_CALIBRATION_VAL_0,
    TTV_CALIBRATION_VAL_1,
    TTV_CALIBRATION_VAL_2,
    TTV_MAX
} THERAPY_TARGET_VALUE_E;

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Events (GUI -> FSM) ---- */
void fsm_o3_sendEnter(void);
void fsm_o3_sendCancel(void);
void fsm_o3_sendGeneric(void);

/* ---- Therapy Params (GUI <-> FSM) ---- */
void   fsm_o3_setTherapyParam(uint8 paramId, uint32 value);
uint32 fsm_o3_getTherapyParam(uint8 paramId);

#ifdef __cplusplus
}
#endif

#endif /* FSM_O3_API_H_ */
