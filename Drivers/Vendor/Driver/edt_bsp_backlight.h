/**
  ******************************************************************************  
  * File Name          : edt_bsp_backlight.h
  * @author            : EDT Embedded Application Team
  * Description        : This file contains the common defines and functions 
  *                      prototypes for the edt_bsp_backlight.c driver.
  * @brief             : EDT <https://smartembeddeddisplay.com/>
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT(c) 2023 Emerging Display Technologies Corp.
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Emerging Display Technologies Corp. nor the names of 
  *      its contributors may be used to endorse or promote products derived from 
  *      this software without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EDT_BSP_BACKLIGHT_H
#define __EDT_BSP_BACKLIGHT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "main.h"   
#include <stdbool.h>

extern TIM_HandleTypeDef hPwmBL;
extern ADC_HandleTypeDef hadc_ts;

typedef struct
{
  uint8_t pwm;          //backlight pwm value 0-100% 
  uint8_t adj;
  bool state;           //backlight ON/OFF
  int32_t MCUtemp;      // Use MCU temperature parameters to estimate LCD surface temperature
}BL_StructDef;

/****************************************
bACKLIGHT PWM frequrncy :
  AHB1 / BL_Prescaler / BL_Period / 2 
  200,000,000 / (2500-1) / (200-1) = 402Hz
******************************************/
#define  BL_Prescaler    2500 - 1
#define  BL_Period       200-1  /* Period Value  */
#define  BL_Pulse         50

#define LCD_BL_INV       false
#define BL_Percentage   (BL_Period+1)/100

/*The temperature sensor can measure the junction temperature (TJ) of the device
 in the �V40 to 125 �XC temperature range. */
#define MAX_OP_Temp             125
#define MIN_OP_Temp             -40
#define Diff_CAL_Temp           (130-30)
#define TEMP_REFRESH_PERIOD     1000    /* Internal temperature refresh period */
#define TEMP_TH_DIV             8
#define TEMP_TH                 55    //when 50 deg.C  
#define TEMP_TH_MAX             75   //when 70 deg.C 
#define Temp_offset             53.0


void    EDT_LCD_BL_ON(void);
void    EDT_LCD_BL_OFF(void);
void    EDT_LCD_BL_PWM_Init(TIM_HandleTypeDef *htim);
void    EDT_LCD_BL_SetPwm(uint8_t value);
uint8_t EDT_LCD_BL_GetPwm(void);
void    EDT_SuspendPwmTask(void);
void    EDT_ResumePwmTask(void);
void    EDT_TEMPSENSOR_Init(void);
void    EDT_BL_Set_Handle(TIM_HandleTypeDef htim, ADC_HandleTypeDef hts);


#ifdef __cplusplus
}
#endif
#endif /* __EDT_BSP_BACKLIGHT_H */

 /*******(C) COPYRIGHT Emerging Display Technologies Corp. **** END OF FILE ***/


