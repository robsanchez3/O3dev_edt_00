/**
  ******************************************************************************  
  * File Name          : edt_bsp_lcd.c
  * @author            : EDT Embedded Application Team
  * Description        : This file includes the driver for LCD/TFT module on
  *                      smart embedded display board.
  * @brief             : EDT <https://smartembeddeddisplay.com/>
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT(c) 2020 Emerging Display Technologies Corp.
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
#include "edt_bsp_lcd.h"

/*******LCD Sleep function*****/
LCD_StructDef LCD = {.width = TFT_WIDTH, .height = TFT_HEIGHT, .lcdstate = false};
uint16_t Sleep_cunter=0;
uint16_t Sleep_Time=180;
static bool SleepDetected;
extern osThreadId_t LCDSleepTaskHandle;


/******************************************************************************
  * @brief  EDT_LCD_GetXSize
  * @param  NONE
  * @retval DISPLAY Width uint32_t
  * @note   EDT DISPLAY Get Width Size
*******************************************************************************/
uint32_t EDT_LCD_GetXSize(void)
{
    return LCD.width;
}
/******************************************************************************
  * @brief  EDT_LCD_GetYSize
  * @param  NONE
  * @retval DISPLAY Height uint32_t
  * @note   EDT DISPLAY Get Height Size
*******************************************************************************/
uint32_t EDT_LCD_GetYSize(void)
{
    return LCD.height;
}
/******************************************************************************
  * @brief  EDT_LCD_SetSize
  * @param  NONE
  * @retval NONE
  * @note   EDT Set DISPLAY Size
*******************************************************************************/
void EDT_LCD_SetSize(LCD_StructDef * lcd, uint16_t width, uint16_t height)
{
  lcd->width = width;
  lcd->height = height;
}
/******************************************************************************
  * @brief  EDT_LCD_DisplayOn
  * @param  NONE
  * @retval NONE
  * @note   EDT DISPLAY ON Ctrl hltdc/ RESET PIN
*******************************************************************************/
void EDT_LCD_DisplayOn(void)
{
  HAL_GPIO_WritePin(LCD_CTRL_GPIO_Port, LCD_CTRL_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);

  EDT_LCD_BL_ON();
  EDT_LCD_SetDisplayStatus( true );
  
}
/******************************************************************************
  * @brief  EDT_LCD_DisplayOff
  * @param  NONE
  * @retval NONE
  * @note   EDT DISPLAY ON Ctrl hltdc/RESET PIN
*******************************************************************************/
void EDT_LCD_DisplayOff(void)
{
      EDT_LCD_BL_OFF();
  
	  HAL_GPIO_WritePin(LCD_CTRL_GPIO_Port, LCD_CTRL_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);

   EDT_LCD_SetDisplayStatus( false );
  
}
/******************************************************************************
  * @brief  EDT_LCD_SetDisplayStatus
  * @param  lcdstatus  : true :display enabled / false :display disable
  * @retval NONE
  * @note   EDT DISPLAY ON Ctrl hltdc/RESET PIN
*******************************************************************************/
void EDT_LCD_SetDisplayStatus(bool lcdstatus)
{
  LCD.lcdstate = lcdstatus;
}
/******************************************************************************
  * @brief  EDT_LCD_DisplayOff
  * @param  NONE
  * @retval lcdstatus  : true :display enabled / false :display disable
  * @note   EDT DISPLAY ON Ctrl hltdc/RESET PIN
*******************************************************************************/
bool EDT_LCD_GetDisplayStatus(void)
{
 return LCD.lcdstate ;
}

/******************************************************************************
  * @brief  EDT_LCD_Reset
  * @param  NONE
  * @retval NONE
  * @note   EDT DISPLAY RESET Control PIN
*******************************************************************************/
void EDT_LCD_Reset(void)
{
  HAL_GPIO_WritePin(LCD_CTRL_GPIO_Port, LCD_CTRL_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);

  EDT_LCD_DisplayOn();
//  EDT_LCD_SetDisplayStatus(true);
}
/*****************************************************************
  * @brief  Sleep Function
  * @param  
  * @param  
  * @retval None
  * @note   turn off  LCD display and backlight
*****************************************************************/
void StartLCDSleepTask(void * argument)
{
  while(1) {
    #if defined( USE_LCD_SleepFunction )  
      EDT_LCD_Sleep_Function();
    #endif
//      osDelay( TEMP_REFRESH_PERIOD );
      osDelay( 1000 );
    }   
}
/************************************************/
void EDT_LCD_Sleep_Time(uint16_t value)
{
  Sleep_Time = value;
}
/************************************************/
void EDT_LCD_Sleep_Function(void)
{  
  Sleep_cunter++; 

  if ( Sleep_cunter >= Sleep_Time ) {
    if ( LCD.lcdstate == true ) {
      Sleep_cunter = 0;
      EDT_LCD_DisplayOff();
    }
    SleepDetected = true; 
  }

  if ( EDT_TS_GetDetected() ) {
    Sleep_cunter = 0;
    EDT_TS_SetDetected( false );
    if ( LCD.lcdstate == false ) {
      EDT_LCD_DisplayOn();   
    }
  } 
}

bool EDT_Sleep_GetDetected(void) 
{
  return  SleepDetected;
}

void EDT_Sleep_SetDetected(bool bl) 
{
  SleepDetected = bl;
}
/******************************************************************************
  * @brief  SuspendLCDSleepTask
  * @param
  * @param
  * @note   Stop LCDSleep Task
  * @retval Create Task For LCDSleep
*******************************************************************************/
void EDT_Sleep_SuspendLCDSleepTask(void)
{
	osThreadSuspend(LCDSleepTaskHandle);
//    vTaskSuspend(osThreadGetId());
    Sleep_cunter=0;
}
/******************************************************************************
  * @brief  ResumeLCDSleepTask
  * @param
  * @param
  * @note   Start LCDSleep Task
  * @retval Create Task For LCDSleep
*******************************************************************************/
void EDT_Sleep_ResumeLCDSleepTask(void)
{
	osThreadResume(LCDSleepTaskHandle);
//    vTaskResume(osThreadGetId());
    Sleep_cunter=0;
}

 /*******(C) COPYRIGHT Emerging Display Technologies Corp. **** END OF FILE ***/
