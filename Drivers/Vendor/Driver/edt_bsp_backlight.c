/**
  ******************************************************************************  
  * File Name          : edt_bsp_backlight.c
  * @author            : EDT Embedded Application Team
  * Description        : This file provides a set of functions needed to manage 
  *                      the LCD Backlight on smart embedded display board.  
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
#include "edt_bsp_backlight.h"
#include "tim.h" // PWM config parameter setting   

#include "adc.h"
ADC_HandleTypeDef hadc_ts;

TIM_HandleTypeDef hPwmBL  = {.Instance=LCD_BL_PWM_TIMER,};

BL_StructDef backlight = {.pwm = 90,.adj = 100,.state = false};

static void  EDT_BL_Update(void);
static float TEMP_SENSOR_GetValue(void);
static void setPWM(TIM_HandleTypeDef timer, uint32_t channel, uint16_t period, uint16_t pulse);
extern osThreadId_t BLPwmTaskHandle;

void EDT_BL_Set_Handle(TIM_HandleTypeDef htim, ADC_HandleTypeDef hts)
{
  hPwmBL = htim ;
  hadc_ts = hts ;
}
/******************************************************************************
  * @brief  setPWM
  * @param  htim    : TIM_HandleTypeDef hPwmBL.
  * @param  channel : BL_PWM_CHANNEL
  * @param  period  : BL_Period
  * @param  pulse   : Value
  * @retval NONE
  * @note   EDT DISPLAY Backlight setPWM
*******************************************************************************/
static void setPWM(TIM_HandleTypeDef timer, uint32_t channel, uint16_t period, uint16_t pulse)
{
  __HAL_TIM_SET_COMPARE(&timer, channel, pulse);
}
/*****************************************************************
  * @brief  EDT_BL_Update
  * @param  NONE
  * @note   EDT_LCD Backlight Realy time process
  * @retval NONE
  * automatically adjust backlight brightness
  * 50~70 degrees, Reference IC temperature to automatically adjust backlight brightness
  * Under 50 degrees, the maximum brightness is 100%
  * Above 70 degrees, the maximum brightness is 70% 
*****************************************************************/
void EDT_BL_Update(void)
{
  static float judgeTempValue = 0.0;  
  uint8_t newpwm;
  newpwm = backlight.pwm;
  
  int16_t t_value, ic_temp = 0;
  TEMP_SENSOR_GetValue();
  for ( uint8_t i=0; i<TEMP_TH_DIV; i++) {     
    // Calculates the average value of the IC temperature every second. 
    t_value = (int32_t) TEMP_SENSOR_GetValue();
    if ( t_value != 999 ) {
      ic_temp += t_value;
    } else i--;
  }
  judgeTempValue = 1.0*ic_temp / TEMP_TH_DIV; // average Temperature
  
  /* Simulate LCD center surface temperature */
  if ( backlight.pwm >= 70 ) {    
    // if PWM setting is higher than 70%. It needs to check the temp VS PWM curve
    if ( judgeTempValue > TEMP_TH_MAX) {
    // when the temperature above 70 degrees. The PWM setting must be 70% MAX    
      backlight.adj = 70;       
    } else {
      if ( judgeTempValue <= TEMP_TH ) {             
    // when the temperature below 50 degrees. The PWM setting does not change
        backlight.adj = backlight.pwm; 
      } else {
        if ( backlight.pwm <= ( 100 - ( judgeTempValue - TEMP_TH )*1.5f )) {
          backlight.adj = backlight.pwm ;
        } else {
   /* The temperature is between 50 and 70 degrees. When the PWM value is higher 
      than the temperature and PWM curve. For every 1 degree increase, the PWM 
      decreases by 1.5%*/
          backlight.adj = (uint8_t)( 100 - ( judgeTempValue - TEMP_TH ) * 1.5f ) ; 
        }
      }
    }
    newpwm = backlight.adj;
  }
       backlight.MCUtemp = (int32_t) judgeTempValue;
  
  if (( judgeTempValue > TEMP_TH ) && ( backlight.pwm >= 70 )) {
        setPWM( hPwmBL, LCD_BL_PWM_CHANNEL, BL_Period, newpwm * ( BL_Period - 1 )/ BL_Percentage );

  }
}
/*****************************************************************
  * @brief  TS_CAL
  * @param  NONE
  * @retval NONE
  * @note   Compute TEMP_SENSOR initial factors
*****************************************************************/
uint16_t TS_CAL1;
uint16_t TS_CAL2;
float Factor1; 
int32_t Raw_MAX,Raw_MIN; 

static void TS_CAL(void)
{ 
  TS_CAL1 = *(__IO uint16_t *)(0x0BFA0710);
  TS_CAL2 = *(__IO uint16_t *)(0x0BFA0742);

  TS_CAL2 -= TS_CAL1;
  Factor1 = (float)((float)Diff_CAL_Temp/(float)TS_CAL2);
  
  Raw_MAX = (MAX_OP_Temp - (int8_t)Temp_offset-30)* TS_CAL2;
  Raw_MIN = (MIN_OP_Temp - (int8_t)Temp_offset-30)* TS_CAL2;
  Raw_MAX = Raw_MAX/Diff_CAL_Temp + TS_CAL1;
  Raw_MIN = Raw_MIN/Diff_CAL_Temp + TS_CAL1;
}
/*****************************************************************
  * @brief  TEMP_SENSOR_GetValue
  * @param  NONE
  * @retval Temperature sensor value
  * @note   EDT_CPU_Temperature GetValue
*****************************************************************/ 
static float TEMP_SENSOR_GetValue(void)
{
   int32_t Raw_tempa;
   float Vtemp_sensor;
   if (HAL_ADC_Start(&hadc1) == HAL_OK) {
     if (HAL_ADC_PollForConversion(&hadc1, 200) == HAL_OK) {
       Raw_tempa = HAL_ADC_GetValue(&hadc1);
       if ((Raw_tempa < Raw_MAX)&&(Raw_tempa > Raw_MIN)) {
         Raw_tempa -= TS_CAL1; 
         Vtemp_sensor = (float)Factor1 * (float)Raw_tempa + (float)Temp_offset;   // compare IC and ambient temp.
         return (float) Vtemp_sensor; 
       }
     }
   }
   return 999;
}
/*******************************************************************************
  * @brief  EDT_LCD_BL_ON
  * @param  NONE
  * @retval NONE
  * @note   EDT DISPLAY ON Ctrl BackLight Control PIN
*******************************************************************************/
void EDT_LCD_BL_ON(void)
{
	HAL_TIM_PWM_Start(&hPwmBL,LCD_BL_PWM_CHANNEL);
	EDT_LCD_BL_SetPwm(backlight.pwm);
    backlight.state = true;
}
/******************************************************************************
  * @brief  EDT_LCD_BL_OFF
  * @param  NONE
  * @retval NONE
  * @note   EDT DISPLAY ON Ctrl BackLight Control PIN
*******************************************************************************/
void EDT_LCD_BL_OFF(void)
{
	HAL_TIM_PWM_Stop(&hPwmBL,LCD_BL_PWM_CHANNEL);

  backlight.state = false;
}

/***************************************************************
  Initialize TIMx peripheral as follows:
        SystemCoreClock = 400Mhz
        TIM15CLK with in AHB2 Time count as 200Mhz

       + Prescaler = (SystemCoreClock / (2*80000)) - 1 = 10 (with 80Khz)
       + Period = (4 - 1) Tim15 setting    80Khz / 4 = 20khz 
       + Period = (200 - 1) Tim15 setting    80Khz / 200 = 400hz 
       + ClockDivision = 0
       + Counter direction = Up
       Tim prescaler setting in BSP_DEVICE/lcd/
**************************************************************/
void  EDT_LCD_BL_PWM_Init(TIM_HandleTypeDef *htim)
{
  TIM_OC_InitTypeDef sConfig;  

  htim->Instance               =  LCD_BL_PWM_TIMER;
  htim->Init.Prescaler         =  BL_Prescaler ;
  htim->Init.Period            =  BL_Period  ;
  htim->Init.ClockDivision     =  0;
  htim->Init.CounterMode       =  TIM_COUNTERMODE_UP;  
    
  LCD_BL_PWM_TIMER_CLK();
  
  HAL_TIM_PWM_Init(htim);
  
  sConfig.OCMode       = TIM_OCMODE_PWM1;
  sConfig.Pulse        = BL_Pulse ;           
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
  
  HAL_TIM_PWM_ConfigChannel(htim, &sConfig, LCD_BL_PWM_CHANNEL);
  
  HAL_TIM_MspPostInit(htim);
}
/*****************************************************************
  * @brief  EDT_LCD_BL_SetPwm
  * @param  0 - 100 %
  * @param  
  * @note   EDT_LCD Backlight PWM setting 
  * @retval None
*****************************************************************/
void EDT_LCD_BL_SetPwm(uint8_t value)
{
  backlight.pwm = value;

  if (value <= 0) {
//    EDT_LCD_BL_OFF();
//    backlight.state = false;
    
  } else if ( value <= 100) {
#if (LCD_BL_INV == true ) 
    value = 100 - value;
#endif
//    setPWM(hPwmBL,LCD_BL_PWM_CHANNEL, BL_Period, ( BL_Period + 1) - value * BL_Percentage);
//    setPWM(hPwmBL,LCD_BL_PWM_CHANNEL,BL_Period,BL_Period-value*(BL_Period / BL_Percentage));
    setPWM(hPwmBL,LCD_BL_PWM_CHANNEL, BL_Period, value * BL_Percentage);
    if ( backlight.state == false ){
//      EDT_LCD_BL_ON();
//      backlight.state = true;
    }
  }
}
/******************************************************************************
  * @brief  EDT_LCD_BL_GetPwm
  * @param  value    : 0~100
  * @retval NONE
  * @note   EDT DISPLAY Backlight setPWM For User
*******************************************************************************/
uint8_t EDT_LCD_BL_GetPwm(void)
{
  return backlight.pwm ;
}
/*****************************************************************
  * @brief  StartPwmTask
  * @param  
  * @param  
  * @note   EDT_LCD Backlight PWM Start Task
  * @retval None
*****************************************************************/
void StartPwmTask(void *argument)
{
//    vTaskDelay(100);
    EDT_LCD_DisplayOn();
    osDelay(200);
    EDT_LCD_BL_SetPwm(100);
//    EDT_LCD_BL_ON();
    HAL_TIM_PWM_Start(&hPwmBL,LCD_BL_PWM_CHANNEL);
    TS_CAL();

    for(;;) {
      EDT_BL_Update();
      osDelay(TEMP_REFRESH_PERIOD);
  }
}
/******************************************************************************
  * @brief  SuspendPwmTask
  * @param  
  * @param  
  * @note   StartPwmTask
  * @retval Create Task For Pwm Upgrad
*******************************************************************************/
void EDT_SuspendPwmTask(void)
{
	osThreadSuspend(BLPwmTaskHandle);
    EDT_LCD_BL_SetPwm(backlight.pwm );
//    EDT_LCD_BL_ON();
}
/******************************************************************************
  * @brief  ResumePwmTask
  * @param  
  * @param  
  * @note   StartPwmTask
  * @retval Create Task For Pwm Upgrad
*******************************************************************************/
void EDT_ResumePwmTask(void)
{     
	osThreadResume(BLPwmTaskHandle);
    EDT_LCD_BL_SetPwm(backlight.pwm );	
//    EDT_LCD_BL_ON();
}
/******************************************************************************
  * @brief  ResumePwmTask
  * @param  
  * @param  
  * @note   StartPwmTask
  * @retval Create Task For Pwm Upgrad
*******************************************************************************/
bool Get_EDT_BacklightStatus(void)
{     
  return  backlight.state;
}
 /*******(C) COPYRIGHT Emerging Display Technologies Corp. **** END OF FILE ***/


