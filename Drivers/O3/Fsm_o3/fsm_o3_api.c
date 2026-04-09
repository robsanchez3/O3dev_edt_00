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

/* ---- Storage delegates ---- */

void fsm_o3_registerStorage(
    int8 (*startSave)(void),
    int8 (*startLoad)(void),
    int8 (*writeLine)(uint16, int32),
    int8 (*readLine)(uint16, int32 *),
    void (*stop)(void))
{
    GLB_fsm_o3.StartStorageSave  = startSave;
    GLB_fsm_o3.StartStorageLoad  = startLoad;
    GLB_fsm_o3.WriteStorageLine  = writeLine;
    GLB_fsm_o3.ReadStorageLine   = readLine;
    GLB_fsm_o3.StoptStorage      = stop;
}
