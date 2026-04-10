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

/* ---- Configuration setters ---- */
void fsm_o3_setOption(uint16 opt);
void fsm_o3_setTemperatureMonitoring(uint8 on);
void fsm_o3_setPressThreshold(int16 val);
void fsm_o3_setGenerationMode(uint8 mode);
void fsm_o3_setRefreshScreen(uint8 val);

/* ---- Storage delegates ---- */
void fsm_o3_registerStorage(
    int8 (*startSave)(void),
    int8 (*startLoad)(void),
    int8 (*writeLine)(uint16, int32),
    int8 (*readLine)(uint16, int32 *),
    void (*stop)(void)
);

#ifdef __cplusplus
}
#endif

#endif /* FSM_O3_API_H_ */
