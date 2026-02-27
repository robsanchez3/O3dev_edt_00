/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "cmsis_os2.h"
#include "se21194_sys_conf.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void hw_init ( void );
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CAN_STB_Pin GPIO_PIN_5
#define CAN_STB_GPIO_Port GPIOB
#define SD_DETECT_Pin GPIO_PIN_7
#define SD_DETECT_GPIO_Port GPIOC
#define SD_DETECT_EXTI_IRQn EXTI7_IRQn
#define CTP_RST_Pin GPIO_PIN_6
#define CTP_RST_GPIO_Port GPIOH
#define LCD_BL_PWM_Pin GPIO_PIN_5
#define LCD_BL_PWM_GPIO_Port GPIOE
#define FS_PW_SW_Pin GPIO_PIN_15
#define FS_PW_SW_GPIO_Port GPIOI
#define FS_OV_Current_Pin GPIO_PIN_14
#define FS_OV_Current_GPIO_Port GPIOI
#define LCD_RESET_Pin GPIO_PIN_15
#define LCD_RESET_GPIO_Port GPIOH
#define LCD_CTRL_Pin GPIO_PIN_13
#define LCD_CTRL_GPIO_Port GPIOH
#define RTC_TS_LED_Pin GPIO_PIN_14
#define RTC_TS_LED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
