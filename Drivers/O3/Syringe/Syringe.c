/*
 * Syringe.c
 *
 *  Created on: 24/11/2015
 *      Author: Roberto.Sanchez
 */
#include <TypeDef.h>
#include "Syringe.h"

void SYRINGE_InitContext(SYRINGE_CTX_T *Ctx, uint32 *BaseTime, uint16 *SyringePattern)
{
  Ctx->BaseTime = BaseTime;
  Ctx->SyringePattern = SyringePattern;
}

void SYRINGE_InitProcess(SYRINGE_CTX_T *Ctx, int16 CurrentPressure)
{
  Ctx->PeakDetected = 0;
  Ctx->DeltaPressureLimit = INITIAL_DELTA_PRESSURE_LIMIT;
  Ctx->DeltaPressureBefore = 0;
  Ctx->EndOfFillingCounter = 0;
  Ctx->OperatingPressureBefore = CurrentPressure;
  Ctx->IniTime = *(Ctx->BaseTime);
  Ctx->FillingTime = 0;
  Ctx->DetachTime = 0;
  Ctx->PressStableTime = 0;
  Ctx->ModifyDeltaPressureLimitTime = *(Ctx->BaseTime);
}

uint8 SYRINGE_Process(SYRINGE_CTX_T *Ctx, int16 CurrentPressure)
{
  int16  DeltaPressure;
  uint8  EndOfSiringe = 0;


  DeltaPressure = CurrentPressure - Ctx->OperatingPressureBefore;

  if(Ctx->PeakDetected)
  {
    if(DeltaPressure > Ctx->DeltaPressureLimit)
    {
      if(++Ctx->EndOfFillingCounter == POSITIVE_END_OF_FILLING_DETECTION_NUMBER)
      {
        EndOfSiringe = 1;
        Ctx->FillingTime = *(Ctx->BaseTime) - Ctx->IniTime;
      }
      else
      {
        /* Maintain values for next iteration waiting for next end of filling situation */
        DeltaPressure = Ctx->DeltaPressureBefore;
        CurrentPressure = Ctx->OperatingPressureBefore;
      }
    }
    else
    {
      /* Update pressure stable time, reset end counter to avoid pressure glitches */
      Ctx->EndOfFillingCounter = 0;
      Ctx->PressStableTime = *(Ctx->BaseTime) - Ctx->IniTime - Ctx->DetachTime;
    }
  }

  /* Look for first pressure peak after plunger detach */
  if(DeltaPressure < Ctx->DeltaPressureBefore)
  {
    Ctx->PeakDetected = 1;

    if(Ctx->DetachTime == 0)
    {
      Ctx->DetachTime = *(Ctx->BaseTime) - Ctx->IniTime;
    }
  }

  /* Update values for next iteration */
  Ctx->OperatingPressureBefore = CurrentPressure;

  if(DeltaPressure >= 0)
  {
    Ctx->DeltaPressureBefore = DeltaPressure;
  }

  /* Decrement delta pressure if necessary */
  if((*(Ctx->BaseTime) - Ctx->ModifyDeltaPressureLimitTime) > MODIFY_DELTA_PRESSURE_LIMIT_TOUT)
  {
    Ctx->DeltaPressureLimit -= DELTA_PRESSURE_LIMIT_DECREMENT;
    Ctx->ModifyDeltaPressureLimitTime = *(Ctx->BaseTime);
  }

  return(EndOfSiringe);
}

#ifdef SYRINGE_DISCRIMINATOR_ALG_T_P
/* (Filling time * 512) / End pressure */
uint8 SYRINGE_Type(SYRINGE_CTX_T *Ctx)
{
  uint8  i = 0;
  uint32 Percentage;

  Percentage = Ctx->FillingTime;
  Percentage = Percentage << 9;
  Percentage = Percentage / Ctx->OperatingPressureBefore; /* last pressure registered */

  while(*((Ctx->SyringePattern) + i))
  {
    if(Percentage < *((Ctx->SyringePattern) + i))
    {
      break;
    }
    i++;
  }
  return(i);
}
#endif

#ifdef SYRINGE_DISCRIMINATOR_ALG_T_PERCENTAGE
/* Percentage of stable time over filling time */
uint8 SYRINGE_Type(SYRINGE_CTX_T *Ctx)
{
  uint8  i = 0;
  uint32 Percentage;

  Percentage = (Ctx->PressStableTime);
  Percentage = Percentage * (uint32)100;
  Percentage = Percentage / Ctx->FillingTime;

  while(*((Ctx->SyringePattern) + i))
  {
    if((uint16)Percentage < *((Ctx->SyringePattern) + i))
    {
      break;
    }
    i++;
  }
  return(i);
}
#endif


