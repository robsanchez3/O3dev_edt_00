/**
  ******************************************************************************  
  * File Name          : edt_board_config.c
  * @author            : EDT Embedded Application Team
  * Description        : This file includes the initial function of hardware on
  *                      smart embedded display board. 
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"   
#include "gpio.h"  
#include "crc.h"  
#include "dma2d.h"  
#include "ltdc.h"  
#include "i2c.h"  
#include "adc.h"
#include "tim.h"


void hw_init ( void )
{
  MX_GPIO_Init();

  MX_LTDC_Init();

  EDT_LCD_Reset();

  MX_I2C2_Init();   // FOT CTP
  EDT_TS_Set_Handle(hi2c2);

#if defined(USE_BL_PWM)
   #if 1
      MX_TIM3_Init();
      MX_ADC1_Init();
      EDT_BL_Set_Handle(htim3,hadc1);
   #else
      EDT_LCD_BL_PWM_Init(&hPwmBL);
      EDT_TEMPSENSOR_Init();  
   #endif
/*while using BL_Prescaler & BL_Period set in edt_bsp_backlight.h*/      
#endif
  
#if defined(USE_BUZZER)
  MX_TIM8_Init(); 
#endif
  
#if defined(USE_GPIO)  
  EDT_LED_Init(LED1);
  EDT_LED_On(LED1);
#endif 
}

 /*******(C) COPYRIGHT Emerging Display Technologies Corp. **** END OF FILE ***/

