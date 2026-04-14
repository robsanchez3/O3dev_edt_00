#ifndef FSM_O3_API_H_
#define FSM_O3_API_H_

#include "../Typedef.h"
#include "fsm_o3_types.h"

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

/* ---- Callback placeholders ---- */
void fsm_o3_no_op(void);  /* no-op callback for therapy templates */

/* ---- Calibration callbacks (used as function pointers in TherapyTemplates) ---- */
void fsm_o3_calibratePressureInit(void);
void fsm_o3_calibratePressureStep1(void);
void fsm_o3_calibratePressureStep2(void);
void fsm_o3_calibrateFlowStep1Ok(void);
void fsm_o3_calibrateFlowStep1Value(void);
void fsm_o3_calibrateFlowStep2(void);
void fsm_o3_calibrateO3Ref1Ok(void);
void fsm_o3_calibrateO3Ref1Value(void);
void fsm_o3_calibrateO3Ref2Ok(void);
void fsm_o3_calibrateO3Ref2Value(void);
void fsm_o3_calibrateO3Ref3Ok(void);
void fsm_o3_calibrateO3Ref3Value(void);

/* ---- FSM navigation (called from Model) ---- */
void fsm_o3_refreshParameters(void);
void fsm_o3_refreshStartupInfo(void);
void fsm_o3_refreshGeneratorVersion(void);
int16 fsm_o3_getPressureSensor(void);
int16 fsm_o3_getTemperatureSensor(void);
void fsm_o3_gotoCalibratePeriod(void);
void fsm_o3_gotoSaveParameters(void);
void fsm_o3_gotoLoadParameters(void);
void fsm_o3_gotoCalibrateFlow(void);
void fsm_o3_gotoCalibrateO3(void);
void fsm_o3_setMaxAllowedFlow(void);
void fsm_o3_setMaxAllowedTime(void);
OPERATION_MODE_E fsm_o3_convertTherapyOption(OPERATION_MODE_E option);

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

/* ---- State getters (read-only) ---- */
uint16 fsm_o3_getOption(void);
FSM_O3_STATE_ID_E fsm_o3_getStateId(void);
uint16 fsm_o3_getErrorState(void);
uint8  fsm_o3_getRemainingMinutes(void);
uint8  fsm_o3_getRemainingSeconds(void);
uint8  fsm_o3_getWashingSeconds(void);
int16  fsm_o3_getOperatingPressure(void);
uint32 fsm_o3_getTotalDose(void);
uint32 fsm_o3_getOutputVolume(void);
int32  fsm_o3_getTime(void);
uint8  fsm_o3_getRefreshScreen(void);
uint8  fsm_o3_hasO3Sensor(void);
uint8  fsm_o3_hasVacuumPump(void);
uint8  fsm_o3_getGenerationMode(void);
uint8  fsm_o3_hasCalibrationErrorDuringStartUp(void);
uint8  fsm_o3_getDepressureSeconds(void);
uint8  fsm_o3_getInsufflationState(void);
uint8  fsm_o3_getSyringeManualState(void);
VACUUM_LOGICAL_STATE_E fsm_o3_getVacuumStatus(void);
uint8  fsm_o3_getMaxAllowedFlow(void);
uint8  fsm_o3_getMaxAllowedTime(void);
int16  fsm_o3_getPressThreshold(void);
char  *fsm_o3_getSharedBuffer(void);
uint16 fsm_o3_getDebugStateId(void);

/* Config access (callers cast void* to actual type) */
HW_CONFIG_T  *fsm_o3_getHwConfig(void);
USR_CONFIG_T *fsm_o3_getUsrConfig(void);
uint16 *fsm_o3_getSyringePattern(void);

/* ---- Storage delegates ---- */
void fsm_o3_registerStorage(
    int8 (*openWrite)(void),
    int8 (*openRead)(void),
    int8 (*write)(uint16, int32),
    int8 (*read)(uint16, int32 *),
    void (*stop)(void)
);

#ifdef __cplusplus
}
#endif

#endif /* FSM_O3_API_H_ */
