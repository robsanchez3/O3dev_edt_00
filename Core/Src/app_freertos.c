/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os2.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "fsm_o3.h" //rsm
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 512 * 4
};
/* Definitions for TouchGFXTask */
osThreadId_t TouchGFXTaskHandle;
const osThreadAttr_t TouchGFXTask_attributes = {
  .name = "TouchGFXTask",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 8192 * 4
};
/* Definitions for LCDSleepTask */
osThreadId_t LCDSleepTaskHandle;
const osThreadAttr_t LCDSleepTask_attributes = {
  .name = "LCDSleepTask",
  .priority = (osPriority_t) osPriorityBelowNormal,
  .stack_size = 512 * 4
};
/* Definitions for BLPwmTask */
osThreadId_t BLPwmTaskHandle;
const osThreadAttr_t BLPwmTask_attributes = {
  .name = "BLPwmTask",
  .priority = (osPriority_t) osPriorityBelowNormal,
  .stack_size = 512 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
extern void TouchGFX_Task(void *argument);
void StartLCDSleepTask(void *argument);
void StartPwmTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationIdleHook(void);

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of TouchGFXTask */
  TouchGFXTaskHandle = osThreadNew(TouchGFX_Task, NULL, &TouchGFXTask_attributes);

  /* creation of LCDSleepTask */
  LCDSleepTaskHandle = osThreadNew(StartLCDSleepTask, NULL, &LCDSleepTask_attributes);

  /* creation of BLPwmTask */
  BLPwmTaskHandle = osThreadNew(StartPwmTask, NULL, &BLPwmTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief Function implementing the defaultTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN defaultTask */
	MX_USB_DEVICE_Init();
  /* Infinite loop */
  for(;;)
  {
	fsm_o3_main();
    osDelay(1);
  }
  /* USER CODE END defaultTask */
}

/* USER CODE BEGIN Header_StartLCDSleepTask */
/**
* @brief Function implementing the LCDSleepTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLCDSleepTask */
__weak void StartLCDSleepTask(void *argument)
{
  /* USER CODE BEGIN LCDSleepTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END LCDSleepTask */
}

/* USER CODE BEGIN Header_StartPwmTask */
/**
* @brief Function implementing the BLPwmTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPwmTask */
__weak void StartPwmTask(void *argument)
{
  /* USER CODE BEGIN BLPwmTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END BLPwmTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


static uint8_t _IsSD(void)// TODO: remove... just testing during integration of SD card and filesystem
{
	uint8_t status;

	if (HAL_GPIO_ReadPin(SD_DETECT_GPIO_Port, SD_DETECT_Pin) == GPIO_PIN_SET)
	{
		status = HAL_OK;
	}
	else
	{
		status = HAL_ERROR;
	}
	return status;
}

#include "fatfs.h"
//#include <stdio.h>

void list_root_dir(void) {// TODO: remove... just testing during integration of SD card and filesystem
    DIR dir;
    FILINFO fno;
    FRESULT res;

    res = f_opendir(&dir, "/"); // Open root directory
    if (res != FR_OK) {
        printf("Error opening root directory: %d\n", res);
        return;
    }
    else
    {
    	printf("Root directory opened successfully.\n");
    }

    printf("Root directory contents:\n");
    while (1) {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0) break;
        if (fno.fattrib & AM_DIR) {
            printf("[DIR]  %s\n", fno.fname);
        } else {
            printf("[FILE] %s\n", fno.fname);
        }
    }
    f_closedir(&dir);
}

/**
 * @brief List the contents of the /config/hw/ directory in the root filesystem.
 */
void list_config_hw_dir(void) {// TODO: remove... just testing during integration of SD card and filesystem
    DIR dir;
    FILINFO fno;
    FRESULT res;

    res = f_opendir(&dir, "/config/hw/"); // Open /config/hw/ directory
    if (res != FR_OK) {
        printf("Error opening /config/hw/ directory: %d\n", res);
        return;
    } else {
        printf("/config/hw/ directory opened successfully.\n");
    }

    printf("/config/hw/ directory contents:\n");
    while (1) {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0) break;
        if (fno.fattrib & AM_DIR) {
            printf("[DIR]  %s\n", fno.fname);
        } else {
            printf("[FILE] %s\n", fno.fname);
        }
    }
    f_closedir(&dir);
}
/**
 * @brief Checks if the filesystem is mounted.
 * @retval 1 if mounted, 0 if not mounted
 */
uint8_t _is_fs_mounted(void)// TODO: remove... just testing during integration of SD card and filesystem
{
    FATFS *fs;
    DWORD fre_clust, fre_sect, tot_sect;
    FRESULT res;

    // Try to get free space to check if the filesystem is mounted
    res = f_getfree("0:", &fre_clust, &fs);
    if (res == FR_OK) {
        printf("Filesystem is mounted.\n");
        // Optionally, print some info about the filesystem
        tot_sect = (fs->n_fatent - 2) * fs->csize;
        fre_sect = fre_clust * fs->csize;
        printf("Total sectors: %lu, Free sectors: %lu\n", tot_sect, fre_sect);
        return 1;
    } else {
        printf("Filesystem is NOT mounted. f_getfree error: %d\n", res);
        return 0;
    }
}

void Test_fileManager(void) // TODO: remove... just testing during integration of SD card and filesystem
{
	return; // skip test for now

	printf("Testing FikeManager...\n");
	for (int i = 0; i < 5; i++)
	{
		if(_IsSD())
		{
			if(_is_fs_mounted())
			{
				list_root_dir();
				list_config_hw_dir();
			}
		}
		osDelay(1000);
	}
	printf("FikeManager test completed.\n");
}

/* USER CODE END Application */

