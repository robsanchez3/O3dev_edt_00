/*
 * Timer.c
 *
 *  Created on: 14/02/2014
 *      Author: Roberto.Sanchez
 */

//#include <Timer/Timer.h>
#include "Timer.h"

void TIMER_Init(TIMER_CTX_T *Ctx, uint32 *Counter)
{

  Ctx->Counter = Counter;
  Ctx->TimeOut = 0;
  Ctx->StartTime = 0;
  Ctx->PauseStartTime = 0;
  Ctx->CumulatedPause = 0;
  Ctx->State = TIMER_STATE_STOP;
}

void TIMER_Start(TIMER_CTX_T *Ctx, uint32 TimeOut)
{
  Ctx->PauseStartTime = 0;
  Ctx->CumulatedPause = 0;
  Ctx->StartTime = *(Ctx->Counter);
  Ctx->TimeOut = Ctx->StartTime + TimeOut;
  Ctx->State = TIMER_STATE_STARTED;
}

void TIMER_Stop(TIMER_CTX_T *Ctx)
{
  Ctx->State = TIMER_STATE_STOP;
}

void TIMER_Pause(TIMER_CTX_T *Ctx)
{
  if(Ctx->State == TIMER_STATE_STARTED)
  {
    Ctx->State = TIMER_STATE_PAUSED;
    Ctx->PauseStartTime = *(Ctx->Counter);
  }
}

void TIMER_ReStart(TIMER_CTX_T *Ctx)
{
  if(Ctx->State == TIMER_STATE_PAUSED)
  {
    Ctx->State = TIMER_STATE_STARTED;
    Ctx->CumulatedPause += *(Ctx->Counter) - Ctx->PauseStartTime;
    Ctx->PauseStartTime = 0;
  }
}

TIMER_STATE_E TIMER_State(TIMER_CTX_T *Ctx)
{
  if( (Ctx->State == TIMER_STATE_STARTED) && ((*(Ctx->Counter) - Ctx->CumulatedPause) >= Ctx->TimeOut) )
  {
      Ctx->State = TIMER_STATE_EXPIRED;
  }
  return Ctx->State;
}

uint32 TIMER_Count(TIMER_CTX_T *Ctx)
{
  if(Ctx->State == TIMER_STATE_STARTED)
  {
    return *(Ctx->Counter) - Ctx->StartTime - Ctx->CumulatedPause;
  }
  return 0;
}




