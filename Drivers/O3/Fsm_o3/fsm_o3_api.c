#include "fsm_o3_api.h"
#include "fsm_o3_operation.h"

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

/* ---- Therapy Params (GUI <-> FSM) ---- */

void fsm_o3_setTherapyParam(uint8 paramId, uint32 value)
{
    switch(paramId) {
        case TTV_CONCENTRATION:   GLB_fsm_o3.ConfiguredO3Concentration = (uint16)value; break;
        case TTV_FLOW:            GLB_fsm_o3.ConfiguredFlow             = (uint16)value; break;
        case TTV_TIME:            GLB_fsm_o3.ConfiguredTime             = (uint16)value; break;
        case TTV_VOLUME:          GLB_fsm_o3.ConfiguredVolume           = value;         break;
        case TTV_DOSE:            GLB_fsm_o3.ConfiguredDose             = (uint16)value; break;
        case TTV_PRESSURE:        GLB_fsm_o3.ConfiguredPressure         = (uint16)value; break;
        case TTV_VACUUM_TIME:     GLB_fsm_o3.ConfiguredVacuumTime       = (uint16)value; break;
        case TTV_VACUUM_PRESSURE: GLB_fsm_o3.ConfiguredVacuumPressure   = (uint16)value; break;
        case TTV_CALIBRATION_VAL_0: GLB_fsm_o3.CalibrationValue_0       = (uint16)value; break;
        case TTV_CALIBRATION_VAL_1: GLB_fsm_o3.CalibrationValue_1       = (uint16)value; break;
        case TTV_CALIBRATION_VAL_2: GLB_fsm_o3.CalibrationValue_2       = (uint16)value; break;
        default: return;
    }
    UpdateSecondSelectorValue();
}

uint32 fsm_o3_getTherapyParam(uint8 paramId)
{
    switch(paramId) {
        case TTV_CONCENTRATION:   return GLB_fsm_o3.ConfiguredO3Concentration;
        case TTV_FLOW:            return GLB_fsm_o3.ConfiguredFlow;
        case TTV_TIME:            return GLB_fsm_o3.ConfiguredTime;
        case TTV_VOLUME:          return GLB_fsm_o3.ConfiguredVolume;
        case TTV_DOSE:            return GLB_fsm_o3.ConfiguredDose;
        case TTV_PRESSURE:        return GLB_fsm_o3.ConfiguredPressure;
        case TTV_VACUUM_TIME:     return GLB_fsm_o3.ConfiguredVacuumTime;
        case TTV_VACUUM_PRESSURE: return GLB_fsm_o3.ConfiguredVacuumPressure;
        case TTV_CALIBRATION_VAL_0: return GLB_fsm_o3.CalibrationValue_0;
        case TTV_CALIBRATION_VAL_1: return GLB_fsm_o3.CalibrationValue_1;
        case TTV_CALIBRATION_VAL_2: return GLB_fsm_o3.CalibrationValue_2;
        default: return 0;
    }
}

/* ---- Configuration setters ---- */

void fsm_o3_setOption(uint16 opt)
{
    GLB_fsm_o3.Option = (OPERATION_MODE_E)opt;
}

void fsm_o3_setTemperatureMonitoring(uint8 on)
{
    GLB_fsm_o3.TemperatureMonitoring = on;
}

void fsm_o3_setPressThreshold(int16 val)
{
    GLB_fsm_o3.PressThreshold = val;
}

void fsm_o3_setGenerationMode(uint8 mode)
{
    GLB_fsm_o3.UsrConfig.userGenerationMode = mode;
}

void fsm_o3_setRefreshScreen(uint8 val)
{
    GLB_fsm_o3.RefreshScreen = val;
}

/* ---- State getters (read-only) ---- */

uint16 fsm_o3_getOption(void)
{
    return (uint16)GLB_fsm_o3.Option;
}

FSM_O3_STATE_ID_E fsm_o3_getStateId(void)
{
    return GLB_fsm_o3.CurrentState->State_ID;
}

uint16 fsm_o3_getErrorState(void)
{
    return GLB_fsm_o3.ErrorState;
}

uint8 fsm_o3_getRemainingMinutes(void)
{
    return GLB_fsm_o3.RemainingMinutes;
}

uint8 fsm_o3_getRemainingSeconds(void)
{
    return GLB_fsm_o3.RemainingSeconds;
}

uint8 fsm_o3_getWashingSeconds(void)
{
    return GLB_fsm_o3.WashingSeconds;
}

int16 fsm_o3_getOperatingPressure(void)
{
    return GLB_fsm_o3.CurrentOperatingPressure + GLB_fsm_o3.PressAtm;
}

uint32 fsm_o3_getTotalDose(void)
{
    return GLB_fsm_o3.CurrentTotalDose;
}

uint32 fsm_o3_getOutputVolume(void)
{
    return GLB_fsm_o3.CurrentOutputVolume;
}

int32 fsm_o3_getTime(void)
{
    return GLB_Time;
}

uint8 fsm_o3_getRefreshScreen(void)
{
    return GLB_fsm_o3.RefreshScreen;
}

uint8 fsm_o3_hasO3Sensor(void)
{
    return (GLB_fsm_o3.HwConfig.O3Sensor != NO_O3_SENSOR) ? 1 : 0;
}

uint8 fsm_o3_hasVacuumPump(void)
{
    return GLB_fsm_o3.HwConfig.VPump ? 1 : 0;
}

uint8 fsm_o3_getGenerationMode(void)
{
    return GLB_fsm_o3.UsrConfig.userGenerationMode;
}

uint8 fsm_o3_hasCalibrationErrorDuringStartUp(void)
{
    return GLB_fsm_o3.CalibrationErrorDuringStartUp;
}

uint8 fsm_o3_getDepressureSeconds(void)
{
    return GLB_fsm_o3.DepressureSeconds;
}

uint8 fsm_o3_getInsufflationState(void)
{
    return GLB_fsm_o3.InsufflationState;
}

uint8 fsm_o3_getSyringeManualState(void)
{
    return GLB_fsm_o3.SyringeManualState;
}

VACUUM_LOGICAL_STATE_E fsm_o3_getVacuumStatus(void)
{
    return (VACUUM_LOGICAL_STATE_E)GLB_fsm_o3.VacuumStatus;
}

uint8 fsm_o3_getMaxAllowedFlow(void)
{
    return GLB_fsm_o3.MaxAllowedFlow;
}

uint8 fsm_o3_getMaxAllowedTime(void)
{
    return GLB_fsm_o3.MaxAllowedTime;
}

int16 fsm_o3_getPressThreshold(void)
{
    return GLB_fsm_o3.PressThreshold;
}

char *fsm_o3_getSharedBuffer(void)
{
    return (char *)GLB_fsm_o3.SharedBuffer;
}

uint16 fsm_o3_getDebugStateId(void)
{
    return (GLB_fsm_o3.CurrentState == GLB_fsm_o3.LastState)
        ? (uint16)GLB_fsm_o3.CurrentState->State_ID
        : (uint16)GLB_fsm_o3.LastState->State_ID;
}

HW_CONFIG_T *fsm_o3_getHwConfig(void)
{
    return &GLB_fsm_o3.HwConfig;
}

USR_CONFIG_T *fsm_o3_getUsrConfig(void)
{
    return &GLB_fsm_o3.UsrConfig;
}

uint16 *fsm_o3_getSyringePattern(void)
{
    return GLB_fsm_o3.SyringeCtrl.SyringePattern;
}

/* ---- Callback placeholders ---- */

void fsm_o3_no_op(void) {}

/* ---- Calibration callbacks ---- */

void fsm_o3_calibratePressureInit(void)  { CalibratePressureInit(); }
void fsm_o3_calibratePressureStep1(void) { CalibratePressureStep1(); }
void fsm_o3_calibratePressureStep2(void) { CalibratePressureStep2(); }
void fsm_o3_calibrateFlowStep1Ok(void)   { CalibrateFlowStep1_Ok(); }
void fsm_o3_calibrateFlowStep1Value(void){ CalibrateFlowStep1_Value(); }
void fsm_o3_calibrateFlowStep2(void)     { CalibrateFlowStep2(); }
void fsm_o3_calibrateO3Ref1Ok(void)      { CalibrateO3_Ref_1_Ok(); }
void fsm_o3_calibrateO3Ref1Value(void)   { CalibrateO3_Ref_1_Value(); }
void fsm_o3_calibrateO3Ref2Ok(void)      { CalibrateO3_Ref_2_Ok(); }
void fsm_o3_calibrateO3Ref2Value(void)   { CalibrateO3_Ref_2_Value(); }
void fsm_o3_calibrateO3Ref3Ok(void)      { CalibrateO3_Ref_3_Ok(); }
void fsm_o3_calibrateO3Ref3Value(void)   { CalibrateO3_Ref_3_Value(); }

/* ---- FSM navigation ---- */

void fsm_o3_refreshParameters(void)      { refreshParameters(); }
void fsm_o3_refreshStartupInfo(void)     { refreshStartupInfo(); }
void fsm_o3_refreshGeneratorVersion(void){ refreshGeneratorVersion(); }
int16 fsm_o3_getPressureSensor(void)     { return getPressureSensor(); }
int16 fsm_o3_getTemperatureSensor(void)  { return getTemperatureSensor(); }
void fsm_o3_gotoCalibratePeriod(void)    { GotoCalibratePeriod(); }
void fsm_o3_gotoSaveParameters(void)     { GotoSaveParameters(); }
void fsm_o3_gotoLoadParameters(void)     { GotoLoadParameters(); }
void fsm_o3_gotoCalibrateFlow(void)      { GotoCalibrateFlow(); }
void fsm_o3_gotoCalibrateO3(void)        { GotoCalibrateO3(); }
void fsm_o3_setMaxAllowedFlow(void)      { SetMaxAllowedFlow(); }
void fsm_o3_setMaxAllowedTime(void)      { SetMaxAllowedTime(); }

OPERATION_MODE_E fsm_o3_convertTherapyOption(OPERATION_MODE_E option)
{
    return ConvertTherapyOption(option);
}

/* ---- Storage delegates ---- */

void fsm_o3_registerStorage(
    int8 (*openWrite)(void),
    int8 (*openRead)(void),
    int8 (*write)(uint16, int32),
    int8 (*read)(uint16, int32 *),
    void (*stop)(void))
{
    GLB_fsm_o3.storageOpenWrite  = openWrite;
    GLB_fsm_o3.storageOpenRead  = openRead;
    GLB_fsm_o3.storageWrite  = write;
    GLB_fsm_o3.storageRead   = read;
    GLB_fsm_o3.storageClose      = stop;
}
