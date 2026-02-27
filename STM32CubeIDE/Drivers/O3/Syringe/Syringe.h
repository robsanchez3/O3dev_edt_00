/*
 * Syringe.h
 *
 *  Created on: 24/11/2015
 *      Author: Roberto.Sanchez
 */

#ifndef SYRINGE_H_
#define SYRINGE_H_

#include "../TypeDef.h"


#define SYRINGE_DISCRIMINATOR_ALG_T_P
/*#define SYRINGE_DISCRIMINATOR_ALG_T_PERCENTAGE*/

#define INITIAL_DELTA_PRESSURE_LIMIT              75 + 10
#define POSITIVE_END_OF_FILLING_DETECTION_NUMBER  2
#define DELTA_PRESSURE_LIMIT_DECREMENT            2
#define MODIFY_DELTA_PRESSURE_LIMIT_TOUT          400  /* ms */

typedef struct syringe_ctx_t
{
  int8   PeakDetected;                  /*!< Boolean variable to show when pressure is "stable" after plunger detach (first time pressure increment is lower than pressure increment before). */
  int16  DeltaPressureLimit;            /*!< Pressure increment value to detect syringe filled (dynamic value).                           */
  int16  DeltaPressureBefore;           /*!< Pressure increment in last program iteration.                                                */
  int16  EndOfFillingCounter;           /*!< Number of end of filling positive detections.                                                */
  int16  OperatingPressureBefore;       /*!< Pressure value in last program iteration.                                                    */
  uint32 IniTime;                       /*!< Initial time reference (when the filling process starts).                                    */
  uint32 FillingTime;                   /*!< Time of the whole filling process.                                                           */
  uint32 DetachTime;                    /*!< Time from the beginning to plunger detach instant.                                           */
  uint32 PressStableTime;               /*!< Time from detach instant to first end of filling detection.                                  */
  uint32 ModifyDeltaPressureLimitTime;  /*!< Time to compute next decrease of DeltaPressureLimit.                                         */
  uint32 *BaseTime;                     /*!< External base time (milliseconds counter).                                                   */
  uint16 *SyringePattern;               /*!< Pointer to and array of limits to determine syringe type (MUST finish with 0 value element). */
}SYRINGE_CTX_T;

void  SYRINGE_InitContext(SYRINGE_CTX_T *Ctx, uint32 *BaseTime, uint16 *SyringePattern);
void  SYRINGE_InitProcess(SYRINGE_CTX_T *Ctx, int16 CurrentPressure);
uint8 SYRINGE_Process(SYRINGE_CTX_T *Ctx, int16 CurrentPressure);
uint8 SYRINGE_Type(SYRINGE_CTX_T *Ctx);

#endif /* SYRINGE_H_ */
