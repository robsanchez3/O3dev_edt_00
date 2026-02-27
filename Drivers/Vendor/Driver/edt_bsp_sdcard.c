/**
  ******************************************************************************
  * File Name          : edt_h7xx_sdcard.c
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
/* Includes ------------------------------------------------------------------*/
#include "edt_bsp_sdcard.h"

#if defined(USE_SDCARD)

SD_HandleTypeDef uSdHandle;

static SDSTATUS sdStatus;

/**
  * @brief  Initializes the SD card device.
  * @retval SD status
  */
uint8_t EDT_SD_Init(void)
{
  
  GPIO_InitTypeDef GPIO_InitStruct;
  uint8_t sd_state = MSD_OK;

  /* uSD device interface configuration */
  uSdHandle.Instance = SDMMCx_INSTANCE;

#if ! defined (BSP_SD_HIGH_PERFORMANCE_CONFIG)
  uSdHandle.Init.ClockDiv            = 1;
#else
  /* Code for high performance */
  uSdHandle.Init.ClockDiv            = 2;
#endif /* BSP_SD_HIGH_PERFORMANCE_CONFIG  */

  uSdHandle.Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  uSdHandle.Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
  uSdHandle.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  uSdHandle.Init.BusWide             = SDMMC_BUS_WIDE_4B;
  uSdHandle.Init.TranceiverPresent   = SDMMC_TRANSCEIVER_NOT_PRESENT;

   SDMMCx_DETECT_GPIO_CLK_ENABLE()   ;
   
   /* Check if the SD card when Initial plugged in the slot */
   GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
   GPIO_InitStruct.Pull      = GPIO_PULLUP;
   GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
   GPIO_InitStruct.Pin       = SDMMCx_DETECT_PIN;   
   HAL_GPIO_Init(SDMMCx_DETECT_GPIO_PORT, &GPIO_InitStruct);
   
  /* Check if the SD card is plugged in the slot */
  if(EDT_SD_IsDetected() != SD_PRESENT)
  {
    sdStatus.SDCARD_EVENT          = MSDDISK_DISCONNECTION_EVENT;
    EDT_SD_DeInit();
    return MSD_ERROR_SD_NOT_PRESENT;
  }  
   
  /* HAL SD initialization */
  if(HAL_SD_Init(&uSdHandle) != HAL_OK)
  {
    sd_state = MSD_ERROR;
  }
    
  sdStatus.UseExtiModeDetection  = 0;
  sdStatus.SDCARD_EVENT          = MSDDISK_CONNECTION_EVENT;
  return  sd_state;
}

/**
  * @brief  DeInitializes the SD card device.
  * @retval SD status
  */
uint8_t EDT_SD_DeInit(void)
{
  uint8_t sd_state = MSD_OK;

  uSdHandle.Instance = SDMMCx_INSTANCE;

  /* Set back Mfx pin to INPUT mode in case it was in exti */
  sdStatus.UseExtiModeDetection = 0;

  /* HAL SD deinitialization */
  if(HAL_SD_DeInit(&uSdHandle) != HAL_OK)
  {
    sd_state = MSD_ERROR;
  }

  /* Msp SD deinitialization */
  uSdHandle.Instance = SDMMCx_INSTANCE;

  return  sd_state;
}

/**
  * @brief  Configures Interrupt mode for SD1 detection pin.
  * @retval Returns 0
  */
uint8_t EDT_SD_ITConfig(void)
{
  /* Configure Interrupt mode for SD detection pin */
  /* Note: disabling exti mode can be done calling BSP_SD_DeInit() */
  sdStatus.UseExtiModeDetection = 1;
  return EDT_SD_IsDetected();
}

/**
  * @brief  Configures Interrupt mode for SD1 detection pin.
  * @retval Returns 0
  */
uint8_t EDT_SD_GetSdEvent(void)
{
  return sdStatus.SDCARD_EVENT;
}
/**
 * @brief  Detects if SD card is correctly plugged in the memory slot or not.
 * @retval Returns if SD is detected or not
 */
uint8_t EDT_SD_IsDetected(void)
{
  __IO uint8_t status = SD_PRESENT;
  GPIO_InitTypeDef   GPIO_InitStructure;
 
  if(HAL_GPIO_ReadPin(SDMMCx_DETECT_GPIO_PORT,SDMMCx_DETECT_PIN) == 1)
  {
    status = SD_NOT_PRESENT;
  }
  else
    status = SD_PRESENT;

  if (sdStatus.UseExtiModeDetection)
  {
    sdStatus.UseExtiModeDetection=0;
    if (status == SD_NOT_PRESENT)
    { 
      GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;      
      GPIO_InitStructure.Pull = GPIO_PULLUP;
      GPIO_InitStructure.Pin = SDMMCx_DETECT_PIN;
      HAL_GPIO_Init(SDMMCx_DETECT_GPIO_PORT, &GPIO_InitStructure);
      
      sdStatus.SDCARD_EVENT = MSDDISK_DISCONNECTION_EVENT;

  /* Enable and set EXTI lines SDMMCx_DETECT_IRQn Interrupt to the lowest priority */
      HAL_NVIC_SetPriority(SDMMCx_DETECT_IRQn, SDMMCx_DETECT_IRQ_SetPriority, 0);
      HAL_NVIC_EnableIRQ(SDMMCx_DETECT_IRQn);
    }
    else
    {
      /* Configure PC.13 pin as input floating */
      GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
      GPIO_InitStructure.Pull = GPIO_NOPULL;
      GPIO_InitStructure.Pin = SDMMCx_DETECT_PIN;
      HAL_GPIO_Init(SDMMCx_DETECT_GPIO_PORT, &GPIO_InitStructure);

      sdStatus.SDCARD_EVENT = MSDDISK_CONNECTION_EVENT;
    /* Enable and set EXTI SDMMCx_DETECT_IRQn Interrupt to the lowest priority */
      HAL_NVIC_SetPriority(SDMMCx_DETECT_IRQn, SDMMCx_DETECT_IRQ_SetPriority, 0);
      HAL_NVIC_EnableIRQ(SDMMCx_DETECT_IRQn);
    }
  }

  return status;
}


/**
  * @brief  Reads block(s) from a specified address in an SD card, in polling mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read
  * @param  NumOfBlocks: Number of SD blocks to read
  * @param  Timeout: Timeout for read operation
  * @retval SD status
  */
uint8_t EDT_SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
  if( HAL_SD_ReadBlocks(&uSdHandle, (uint8_t *)pData, ReadAddr, NumOfBlocks, Timeout) == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }

}

/**
  * @brief  Writes block(s) to a specified address in an SD card, in polling mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written
  * @param  NumOfBlocks: Number of SD blocks to write
  * @param  Timeout: Timeout for write operation
  * @retval SD status
  */
uint8_t EDT_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{

  if( HAL_SD_WriteBlocks(&uSdHandle, (uint8_t *)pData, WriteAddr, NumOfBlocks, Timeout) == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}

/**
  * @brief  Reads block(s) from a specified address in an SD card, in DMA mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read
  * @param  NumOfBlocks: Number of SD blocks to read
  * @retval SD status
  */
uint8_t EDT_SD_ReadBlocks_DMA(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks)
{

  if( HAL_SD_ReadBlocks_DMA(&uSdHandle, (uint8_t *)pData, ReadAddr, NumOfBlocks) == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}

/**
  * @brief  Writes block(s) to a specified address in an SD card, in DMA mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written
  * @param  NumOfBlocks: Number of SD blocks to write
  * @retval SD status
  */
uint8_t EDT_SD_WriteBlocks_DMA(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks)
{

  if( HAL_SD_WriteBlocks_DMA(&uSdHandle, (uint8_t *)pData, WriteAddr, NumOfBlocks) == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }

}

/**
  * @brief  Erases the specified memory area of the given SD card.
  * @param  StartAddr: Start byte address
  * @param  EndAddr: End byte address
  * @retval SD status
  */
uint8_t EDT_SD_Erase(uint32_t StartAddr, uint32_t EndAddr)
{

  if( HAL_SD_Erase(&uSdHandle, StartAddr, EndAddr) == HAL_OK)
  {
    return MSD_OK;
  }
  else
  {
    return MSD_ERROR;
  }
}

/**
  * @brief  Gets the current SD card data status.
  * @retval Data transfer state.
  *          This value can be one of the following values:
  *            @arg  SD_TRANSFER_OK: No data transfer is acting
  *            @arg  SD_TRANSFER_BUSY: Data transfer is acting
  */
uint8_t EDT_SD_GetCardState(void)
{
  return((HAL_SD_GetCardState(&uSdHandle) == HAL_SD_CARD_TRANSFER ) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
}


/**
  * @brief  Get SD information about specific SD card.
  * @param  CardInfo: Pointer to HAL_SD_CardInfoTypedef structure
  * @retval None
  */
void EDT_SD_GetCardInfo(EDT_SD_CardInfo *CardInfo)
{
  HAL_SD_GetCardInfo(&uSdHandle, CardInfo);
}


/**
  * @}
  */

/**
  * @brief SD Abort callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_AbortCallback(SD_HandleTypeDef *hsd)
{
  EDT_SD_AbortCallback();
}


/**
  * @brief Tx Transfer completed callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
  EDT_SD_WriteCpltCallback();
}

/**
  * @brief Rx Transfer completed callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
  EDT_SD_ReadCpltCallback();
}

/**
  * @brief Error callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd)
{
  EDT_SD_ErrorCallback();
}
/**
  * @brief SDMMCx_IRQ Handler 
  * @retval None
  */
void SDMMCx_IRQHandler(void)
{
     EDT_SDMMCx_IRQHandler(); 
}
/**
  * @brief SDMMCx_IRQ Handler 
  * @retval None
  */
__weak void EDT_SDMMCx_IRQHandler(void)
{
    HAL_SD_IRQHandler(&uSdHandle);
}
/**
  * @brief BSP SD Abort callbacks
  * @retval None
  */
__weak void EDT_SD_AbortCallback(void)
{
}

/**
  * @brief BSP Tx Transfer completed callbacks
  * @retval None
  */
__weak void EDT_SD_WriteCpltCallback(void)
{
}

/**
  * @brief BSP Rx Transfer completed callbacks
  * @retval None
  */
__weak void EDT_SD_ReadCpltCallback(void)
{
}


/**
  * @brief BSP Error callbacks
  * @retval None
  */
__weak void EDT_SD_ErrorCallback(void)
{

}

void SDMMCx_DETECT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(SDMMCx_DETECT_PIN);  
  if(EDT_SD_ITConfig()==SD_PRESENT) //if sdcard SD_NOT_PRESENT
    {
      EDT_SD_Init();     
    }
    else
    {
      EDT_SD_DeInit();
    }
}

#endif /*defined(USE_SDCARD)*/

 /************************ (C) COPYRIGHT EDT *****END OF FILE****/
