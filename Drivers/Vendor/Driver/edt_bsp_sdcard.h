/**
  ******************************************************************************
  * File Name          : edt_h7xx_sdcard.h
  * @author            : EDT Embedded Application Team
  * Description        : This file provides code for the configuration
  *                      of the SDCARD instances To stm32h750.
  * @version           : V1.0.2
  * @date              : 18-Nov-2020
  * @brief             : EDT <https://smartembeddeddisplay.com/>
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 EDT</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
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
/* Includes ------------------------------------------------------------------*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EDT_H7XX_SDCARD_H
#define __EDT_H7XX_SDCARD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/**
  * @brief SD Card information structure
  */
#define EDT_SD_CardInfo HAL_SD_CardInfoTypeDef
/**
  * @}
  */
#if defined(STM32F746xx)||defined(STM32F750xx)      
#define  USE_SD_TRANSCEIVER           0U        /* use uSD Transceiver */
#endif
   
/**
  * @brief  SD status structure definition
  */
#define   MSD_OK                        ((uint8_t)0x00)
#define   MSD_ERROR                     ((uint8_t)0x01)
#define   MSD_ERROR_SD_NOT_PRESENT      ((uint8_t)0x02)

/**
  * @brief  SD transfer state definition
  */
#define   SD_TRANSFER_OK                ((uint8_t)0x00)
#define   SD_TRANSFER_BUSY              ((uint8_t)0x01)

/** @defgroup STM32H743I_EVAL_SD_Exported_Constants SD Exported Constants
  * @{
  */
#define SD_PRESENT               ((uint8_t)0x01)
#define SD_NOT_PRESENT           ((uint8_t)0x00)

#define SD_DATATIMEOUT           ((uint32_t)100000000)

typedef struct _sdStatus
{
  uint8_t UseExtiModeDetection;
  uint8_t SDCARD_EVENT;
}SDSTATUS;


#ifndef USE_SD_TRANSCEIVER
#define USE_SD_TRANSCEIVER 1
#endif /* USE_SD_TRANSCEIVER */
    
typedef enum
{
  NONEDISK_EVENT              = 0,
  USBDISK_DISCONNECTION_EVENT = 1,  
  USBDISK_CONNECTION_EVENT = 2,  
  MSDDISK_DISCONNECTION_EVENT = 3,  
  MSDDISK_CONNECTION_EVENT = 4,    
}   
STORAGE_EventTypeDef;

/**
  * @}
  */

uint8_t EDT_SD_Init(void);
uint8_t EDT_SD_DeInit(void);
uint8_t EDT_SD_ITConfig(void);

uint8_t EDT_SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout);
uint8_t EDT_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks,  uint32_t Timeout);
uint8_t EDT_SD_ReadBlocks_DMA(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks);
uint8_t EDT_SD_WriteBlocks_DMA(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks);
uint8_t EDT_SD_Erase(uint32_t StartAddr, uint32_t EndAddr);
uint8_t EDT_SD_GetCardState(void);
void    EDT_SD_GetCardInfo(EDT_SD_CardInfo *CardInfo);
uint8_t EDT_SD_IsDetected(void);
uint8_t EDT_SD_GetSdEvent(void);

/* These functions can be modified in case the current settings (e.g. DMA stream)
   need to be changed for specific application needs */
void    EDT_SD_AbortCallback(void);
void    EDT_SD_WriteCpltCallback(void);
void    EDT_SD_ReadCpltCallback(void);
void    EDT_SD_ErrorCallback(void);
void    EDT_SDMMCx_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /*__EDT_H7XX_SDCARD_H */

/************************ (C) COPYRIGHT EDT*****END OF FILE****/
