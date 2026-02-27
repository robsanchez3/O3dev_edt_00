/*!
 * \defgroup    Timer_group Timer.
 */

/*!
 * \file        Timer.h
 * \brief       This module implements general timer control process.\n
 *              The module need a external counter to compute time.\n
 *              It's not the module responsibility to actualize external
 *              counter variable.\n\n
 *              (The maximum count range is a 32 bit value).
 *
 * \author      Fernando Alcojor & Roberto Sanchez.
 * \version
 * \htmlonly
 *              <A HREF="path-to-tag">TAG</A>
 * \endhtmlonly
 * \date        17/02/2014.
 * \remarks
 * \attention
 * \warning
 * \copyright   SEDECAL S.A.
 * \ingroup     Timer_group
 */


#ifndef TIMER_H_
#define TIMER_H_

#include "../TypeDef.h"

/*!
 * \brief Timer states enumeration.
 */
typedef enum timer_state_e
{
  TIMER_STATE_STOP = 0, /*!< Stop state.         */
  TIMER_STATE_STARTED,  /*!< Timer started.      */
  TIMER_STATE_EXPIRED,  /*!< Timer time expired. */
  TIMER_STATE_PAUSED,   /*!< Timer paused.       */
  TIMER_STATE_MAX
}TIMER_STATE_E;

/*!
 * \brief Module context.
 */
typedef struct timer_ctx_t
{
  uint32       *Counter;          /*!< Pointer to external counter.       */
  uint32        TimeOut;          /*!< Time when the counter will expire. */
  uint32        StartTime;        /*!< Time when the timer was started.   */
  uint32        PauseStartTime;   /*!< Time when the timer was paused.    */
  uint32        CumulatedPause;   /*!< Total time paused.                 */
  TIMER_STATE_E State;            /*!< Current timer state.               */
}TIMER_CTX_T;

/*!
 * \brief  Context initialization function.
 *
 * This function initializes all the variables of context structure
 * using parameter and default values.
 *
 * \param  [in]  *Ctx             Pointer to context structure.
 * \param  [in]  *Counter         Pointer to external counter.
 *
 * \return void
 */
void TIMER_Init(TIMER_CTX_T *Ctx, uint32 *Counter);

/*!
 * \brief  Launch timer function.
 *
 * This function starts timer count and computes the
 * time out counter value when the timer will expire.
 *
 * \param  [in]  *Ctx             Pointer to context structure.
 * \param  [in]   TimeOut         Time out value (max 32 bit value)
 *
 * \return void
 */
void TIMER_Start(TIMER_CTX_T *Ctx, uint32 TimeOut);

/*!
 * \brief  Stop timer function.
 *
 * This function stops timer count.
 *
 * \param  [in]  *Ctx             Pointer to context structure.
 *
 * \return void
 */
void TIMER_Stop(TIMER_CTX_T *Ctx);

/*!
 * \brief  Pause timer function.
 *
 * This function pauses timer count.
 *
 * \param  [in]  *Ctx             Pointer to context structure.
 *
 * \return void
 */
void TIMER_Pause(TIMER_CTX_T *Ctx);

/*!
 * \brief  ReStart timer function.
 *
 * This function re-starts timer count after a pause.
 *
 * \param  [in]  *Ctx             Pointer to context structure.
 *
 * \return void
 */
void TIMER_ReStart(TIMER_CTX_T *Ctx);

/*!
 * \brief  Returns timer state.
 *
 * This function returns timer state.
 *
 * \param  [in]  *Ctx             Pointer to context structure.
 *
 * \return TIMER_STATE_E
 */
TIMER_STATE_E TIMER_State(TIMER_CTX_T *Ctx);

/*!
 * \brief  Returns timer count.
 *
 * This function returns timer count since it was started.
 *
 * \param  [in]  *Ctx             Pointer to context structure.
 *
 * \return uint32                 TimerCount (Maximum count range is a 32 bit value).
 */
uint32 TIMER_Count(TIMER_CTX_T *Ctx);


#endif /* TIMER_H_ */
